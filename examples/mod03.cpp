#include <fstream>
#include <mathematica++/mathematica++.h>
#include <mathematica++/transport.h>
#include <mathematica++/defs.h>
#include <mathematica++/library.h>
#include <boost/tuple/tuple_io.hpp>
#include <mathematica++/exceptions.h>

using namespace mathematica;
using namespace library;

MATHEMATICA_DECLARE(Dot);

EXTERN_C DLLEXPORT mint WolframLibrary_getVersion(){return WolframLibraryVersion;}
EXTERN_C DLLEXPORT mint WolframLibrary_initialize(WolframLibraryData libData){
    mathematica::initializer(libData, "SomeFunctionMX");
    mathematica::initializer(libData, "SomeFunctionMXT");
    return 0;
}
EXTERN_C DLLEXPORT void WolframLibrary_uninitialize(WolframLibraryData libData){return;}

EXTERN_C DLLEXPORT int SomeFunctionMX(WolframLibraryData libData, mint argc, MArgument* argv, MArgument res){
    mathematica::mtransport shell(libData, argc, argv, res, "SomeFunctionMX");
    boost::tuple<double, std::complex<int>> args = shell;
    double dbl;
    std::complex<int> cmplx;
    boost::tie(dbl, cmplx) = args;
    std::clog << args << std::endl;
    shell = std::complex<double>(dbl+cmplx.real(), dbl-cmplx.imag());
    return LIBRARY_NO_ERROR;
}

EXTERN_C DLLEXPORT int SomeFunctionMXT(WolframLibraryData libData, mint argc, MArgument* argv, MArgument res){
    mathematica::mtransport shell(libData, argc, argv, res, "SomeFunctionMXT");
    typedef std::vector<std::vector<double>> matrix_type;
    try{
        boost::tuple<matrix_type, matrix_type> args = shell;
        matrix_type matl, matr, mato;
        boost::tie(matl, matr) = args;
        shell << Dot(matl, matr);
        shell >> mato;
        shell = mato;
    }catch(...){
        return shell.pass();
    }
    return LIBRARY_NO_ERROR;
}

// SomeFunctionMX  = LibraryFunctionLoad["/home/neel/Projects/mathematicapp/build/examples/libmod03.so", "SomeFunctionMX", {Real, Integer}, Real]
// SomeFunctionMXT = LibraryFunctionLoad["/home/neel/Projects/mathematicapp/build/examples/libmod03.so", "SomeFunctionMXT", {{_, _}, {_, _}}, {_, _}]
// SomeFunctionMXT[{{0, 1}, {0, 0}}, {{0, 0}, {1, 0}}]
// SomeFunctionMXT[{{0.0, 1.0}, {0.0, 0.0}}, {{0.0, 0.0}, {1.0, 0.0}}]
// SomeFunctionMXT[{0.0, 1.0}, {0.0, 0.0}]
