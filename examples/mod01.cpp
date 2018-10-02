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

#define MATHEMATICA_LIBRARY_EXPORT(M, T, C)                                             \
    EXTERN_C DLLEXPORT int M(WolframLibraryData libData, WMK_LINK native_link){         \
        wtransport shell(libData, native_link);                                         \
        value input = shell.input();                                                    \
        T data = mathematica::cast<T>(input);                                           \
        auto out = C(data);                                                             \
        return shell(out);                                                              \
    }

template <typename T>
struct point_2d{
    T x;
    T y;
};

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
    return shell(input->stringify());
}

// EXTERN_C DLLEXPORT int EchoX(WolframLibraryData libData, WMK_LINK native_link){
//     auto link = driver::link(native_link);
//     connector shell(link);
//     token::ptr input = link->fetch_token(&shell);
//     shell << input->stringify();
//     return LIBRARY_NO_ERROR;
// }

EXTERN_C DLLEXPORT int ArgsListX(WolframLibraryData libData, WMK_LINK native_link){  
    std::ofstream out("/tmp/log");
    std::clog.rdbuf(out.rdbuf());
    
    wtransport shell(libData, native_link);
    value input = shell.input();
    
    value output;
    
    shell << EvaluatePacket(Length(input));
    shell >> output;
    
    int nargs = *output;
    
    shell << EvaluatePacket(Permutations(Append(input, nargs)));
    shell >> output;
    
    return shell(output);
}



// EXTERN_C DLLEXPORT int ArgsListX(WolframLibraryData libData, WMK_LINK native_link){
//     std::ofstream out("/tmp/log");
//     auto old_rdbuf = std::clog.rdbuf();
//     std::clog.rdbuf(out.rdbuf());
//     
//     auto link = driver::link(native_link);
//     connector input_shell(link);
//     token::ptr input = link->fetch_token(&input_shell);
//     WMK_LINK native_io_link = libData->getWSLINK(libData);
//     auto io_link = driver::link(native_io_link);
//     connector shell(io_link);
//     value output;
//     
//     shell << MessagePacket(symbol("ArgsListX"), TextPacket(input->stringify()));
//     libData->processWSLINK(native_io_link);
//     shell.ignore();
//     
//     input_shell << MessagePacket(symbol("ArgsListX"), TextPacket(input->stringify()));
//     libData->processWSLINK(native_link);
//     input_shell.ignore();
//     
//     shell << EvaluatePacket(Length(input));
//     libData->processWSLINK(native_io_link);
//     shell >> output;
//     
//     int nargs = *output;
//     
//     shell << EvaluatePacket(Permutations(Append(input, nargs)));
//     libData->processWSLINK(native_io_link);
//     shell >> output;
//     
//     input_shell << output;
//     
//     std::clog.rdbuf(old_rdbuf);
//     return LIBRARY_NO_ERROR;
// }

// EchoX = LibraryFunctionLoad["/home/neel/Projects/mathematicapp/build/examples/libmod01.so", "EchoX", LinkObject, LinkObject]
// ArgsListX = LibraryFunctionLoad["/home/neel/Projects/mathematicapp/build/examples/libmod01.so", "ArgsListX", LinkObject, LinkObject]
// ArgsListX[1, "Algorithm", "Hypothesis", Graphics@Disk[]]
