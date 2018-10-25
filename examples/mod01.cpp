#include <fstream>
#include <mathematica++/mathematica++.h>
#include <mathematica++/transport.h>
#include <mathematica++/defs.h>

using namespace mathematica;

MATHEMATICA_DECLARE(Print);
MATHEMATICA_DECLARE(Append);
MATHEMATICA_DECLARE(Length);
MATHEMATICA_DECLARE(EvaluatePacket);
MATHEMATICA_DECLARE(ReturnPacket);
MATHEMATICA_DECLARE(MessagePacket);
MATHEMATICA_DECLARE(TextPacket);
MATHEMATICA_DECLARE(Permutations);

EXTERN_C DLLEXPORT mint WolframLibrary_getVersion(){
    return WolframLibraryVersion;
}
EXTERN_C DLLEXPORT int WolframLibrary_initialize( WolframLibraryData libData){
    return 0;
}
EXTERN_C DLLEXPORT void WolframLibrary_uninitialize( WolframLibraryData libData){
    return;
}

EXTERN_C DLLEXPORT int EchoX(WolframLibraryData libData, WMK_LINK native_link){
    wtransport shell(libData, native_link);
    value input = shell.input();
    return shell = input->stringify();
}

EXTERN_C DLLEXPORT int ArgsListX(WolframLibraryData libData, WMK_LINK native_link){  
    std::ofstream out("/tmp/log");
    std::clog.rdbuf(out.rdbuf());
    
    wtransport shell(libData, native_link);
    value input = shell.input();
    
    value output;
    
    shell << Length(input);
    shell >> output;
    
    int nargs = *output;
    
    shell << Permutations(Append(input, nargs));
    shell >> output;
    
    return shell = output;
}

// EchoX = LibraryFunctionLoad["/home/neel/Projects/mathematicapp/build/examples/libmod01.so", "EchoX", LinkObject, LinkObject]
// ArgsListX = LibraryFunctionLoad["/home/neel/Projects/mathematicapp/build/examples/libmod01.so", "ArgsListX", LinkObject, LinkObject]
// ArgsListX[1, "Algorithm", "Hypothesis", Graphics@Disk[]]
