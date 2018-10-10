#include <fstream>
#include <mathematica++/mathematica++.h>
#include <mathematica++/transport.h>
#include <mathematica++/defs.h>
#include <mathematica++/library.h>
#include <boost/tuple/tuple_io.hpp>

using namespace mathematica;
using namespace library;

EXTERN_C DLLEXPORT mint WolframLibrary_getVersion(){return WolframLibraryVersion;}
EXTERN_C DLLEXPORT mint WolframLibrary_initialize(WolframLibraryData libData){return 0;}
EXTERN_C DLLEXPORT void WolframLibrary_uninitialize(WolframLibraryData libData){return;}

EXTERN_C DLLEXPORT int SomeFunctionX(WolframLibraryData libData, mint argc, MArgument* argv, MArgument res){
    mathematica::mtransport shell(libData, argc, argv, res);
    boost::tuple<double, int> args = shell;
    double d;
    int i;
    boost::tie(d, i) = args;
    std::clog << args << std::endl;
    shell(d+i);
    
    return LIBRARY_NO_ERROR;
}

// SomeFunctionX = LibraryFunctionLoad["/home/neel/Projects/mathematicapp/build/examples/libmod03.so", "SomeFunctionX", {Real, Integer}, Real]
