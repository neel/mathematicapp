#include <fstream>
#include <mathematica++/mathematica++.h>
#include <mathematica++/transport.h>
#include <mathematica++/defs.h>
#include <mathematica++/library.h>
#include <boost/tuple/tuple_io.hpp>

using namespace mathematica;
using namespace library;

MATHEMATICA_DECLARE(Dot);
MATHEMATICA_DECLARE(EvaluatePacket);

EXTERN_C DLLEXPORT mint WolframLibrary_getVersion(){return WolframLibraryVersion;}
EXTERN_C DLLEXPORT mint WolframLibrary_initialize(WolframLibraryData libData){return 0;}
EXTERN_C DLLEXPORT void WolframLibrary_uninitialize(WolframLibraryData libData){return;}

EXTERN_C DLLEXPORT int SomeFunctionMX(WolframLibraryData libData, mint argc, MArgument* argv, MArgument res){
    mathematica::mtransport shell(libData, argc, argv, res);
    boost::tuple<double, std::complex<int>> args = shell;
    double dbl;
    std::complex<int> cmplx;
    boost::tie(dbl, cmplx) = args;
    std::clog << args << std::endl;
    shell = std::complex<double>(dbl+cmplx.real(), dbl-cmplx.imag());
    return LIBRARY_NO_ERROR;
}

EXTERN_C DLLEXPORT int SomeFunctionMXT(WolframLibraryData libData, mint argc, MArgument* argv, MArgument res){
//     std::ofstream out("/tmp/log");
//     std::clog.rdbuf(out.rdbuf());
    
    mathematica::mtransport shell(libData, argc, argv, res);
    typedef std::vector<std::vector<double>> matrix_type;
    boost::tuple<matrix_type, matrix_type> args = shell;
    try{
        matrix_type matl, matr, mato;
        boost::tie(matl, matr) = args;
        shell << EvaluatePacket(Dot(matl, matr));
        shell >> mato;
        shell = mato;
    }catch(const std::exception& ex){
        libData->Message(ex.what());
    }
    return LIBRARY_NO_ERROR;
}

// SomeFunctionMX  = LibraryFunctionLoad["/home/neel/Projects/mathematicapp/build/examples/libmod03.so", "SomeFunctionMX", {Real, Integer}, Real]
// SomeFunctionMXT = LibraryFunctionLoad["/home/neel/Projects/mathematicapp/build/examples/libmod03.so", "SomeFunctionMXT", {{Real, 2}, {Real, 2}}, {Real, 2}]
