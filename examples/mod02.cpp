#include <fstream>
#include <mathematica++/mathematica++.h>
#include <mathematica++/transport.h>
#include <mathematica++/defs.h>

#include <boost/tuple/tuple.hpp>
#include <boost/function.hpp>
#include <boost/fusion/container.hpp>
#include <boost/fusion/sequence.hpp>
#include <boost/mpl/int.hpp>
#include <boost/fusion/tuple.hpp>
#include <boost/fusion/functional/invocation/invoke.hpp>

using namespace mathematica;

/**
 * typedef module_overload<int, double, double> resolver_type;
 * resolver_type overload(&callback); // callback is a pointer to function of type int, (double, double)
 * if(overload.feasible(args)){                              // args is a shared pointer to a function token boost::shared_ptr<mathematica::tokens::function>
 *     resolver_type::return_type ret = overload(args);                             // redirects the call to callback function
 * }
 */
template <typename Ret, typename... Args>
struct module_overload{
    typedef Ret                             return_type;
    typedef boost::fusion::tuple<Args...>   arguments_type;
    typedef boost::function<Ret (Args...)>  function_type;
    
    function_type _function;
    
    module_overload(function_type f): _function(f){}
    /**
     * check number of arguments supplied on runtime and number of arguments with which this overload has been prepared at compile time.
     */
    bool feasible(boost::shared_ptr<mathematica::tokens::function> args) const{
        return boost::fusion::result_of::size<arguments_type>::type::value  == args->nargs();
    }
    Ret call(boost::shared_ptr<mathematica::tokens::function> args){
        arguments_type arguments = cast<arguments_type>(args);
        // https://www.boost.org/doc/libs/1_68_0/libs/fusion/doc/html/fusion/functional/invocation/functions/invoke.html
        return boost::fusion::invoke(_function, arguments);
    }
    Ret operator()(boost::shared_ptr<mathematica::tokens::function> args){
        return call(args);
    }
    void out(boost::shared_ptr<mathematica::tokens::function> args, mathematica::wtransport& shell){
        shell(call(args));
    }
};

// template <typename R, typename T>
// struct module_overload_head{
//     typedef R return_type;
//     typedef T tuple_type;
//     
//     tuple_type _heads;
//     
//     module_overload_head(const tuple_type& head): _heads(head){}
//     
//     bool feasible(boost::shared_ptr<mathematica::tokens::function> args) const{
//         std::vector<std::string> heads;
//         boost::fusion::for_each(heads, [&heads](const std::string head){
//             heads.push_back(head);
//         });
//         std::vector<std::string>::const_iterator it = heads.cbegin();
//         for(mathematica::value v: args->_args){
//             boost::shared_ptr<mathematica::tokens::function> f = boost::dynamic_pointer_cast<mathematica::tokens::function>(v);
//             if(!f || f->name() != *it++){
//                 return false;
//             }
//         }
//         return true;
//     }
//     R call(boost::shared_ptr<mathematica::tokens::function> args){
//         arguments_type arguments = cast<arguments_type>(args);
//         // https://www.boost.org/doc/libs/1_68_0/libs/fusion/doc/html/fusion/functional/invocation/functions/invoke.html
//         return boost::fusion::invoke(_function, arguments);
//     }
//     R operator()(boost::shared_ptr<mathematica::tokens::function> args){
//         return call(args);
//     }
// };


#define MATHEMATICA_MODULE(N) EXTERN_C DLLEXPORT int N(WolframLibraryData libData, WMK_LINK native_link)

template <typename T>
struct point_2d{
    T x;
    T y;
};

namespace mathematica{
    template <typename T>
    struct association<point_2d<T>>: sequence<association<point_2d<T>>, point_2d<T>, T, T>{
        static auto detail(property<0>){return &point_2d<T>::x;}
        static auto detail(property<1>){return &point_2d<T>::y;}
    };
}

int some_function_impl1(double x, double y){
    return 0;
}
int some_function_impl2(double x){
    return 1;
}


EXTERN_C DLLEXPORT mint WolframLibrary_getVersion(){
    return WolframLibraryVersion;
}
EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData){
    return 0;
}
EXTERN_C DLLEXPORT void WolframLibrary_uninitialize(WolframLibraryData libData){
    return;
}

EXTERN_C DLLEXPORT int SomeFunctionX(WolframLibraryData libData, WMK_LINK native_link){
    mathematica::wtransport shell(libData, native_link);
    mathematica::value input = shell.input();
    boost::shared_ptr<mathematica::tokens::function> args = boost::dynamic_pointer_cast<mathematica::tokens::function>(input);
    
    {
        typedef module_overload<int, double, double> resolver_type;
        resolver_type overload(&some_function_impl1);                        // callback is a pointer to function of type int, (double, double)
        if(overload.feasible(args)){                                        // args is a shared pointer to a function token boost::shared_ptr<mathematica::tokens::function>
            overload.out(args, shell);                                      // redirects the call to callback function and writes the returned output to shell
            return LIBRARY_NO_ERROR;
        }
    }
    
    {
        typedef module_overload<int, double> resolver_type;
        resolver_type overload(&some_function_impl2);                        // callback is a pointer to function of type int, (double, double)
        if(overload.feasible(args)){                                        // args is a shared pointer to a function token boost::shared_ptr<mathematica::tokens::function>
            overload.out(args, shell);                                      // redirects the call to callback function and writes the returned output to shell
            return LIBRARY_NO_ERROR;
        }
    }
    
    return LIBRARY_TYPE_ERROR;
}


// MATHEMATICA_MODULE_BEGIN(IsAcuteTriangle)
//     MATHEMATICA_MODULE_RELATE("GeoPosition", point_2d<double>)
//     MATHEMATICA_MODULE_OVERLOAD(is_acute_impl, "GeoPosition", "GeoPosition", "GeoPosition")
//     MATHEMATICA_MODULE_OVERLOAD(is_acute_impl, point_2d, point_2d, point_2d)
//     MATHEMATICA_MODULE_OVERLOAD(is_acute_impl, double, double, double, double, double, double)
// MATHEMATICA_MODULE_END()
// 
// 
// EXTERN_C DLLEXPORT int IsAcuteTriangle(WolframLibraryData libData, WMK_LINK native_link){
//     mathematica::wtransport shell(libData, native_link);
//     mathematica::value input = shell.input();
//     boost::shared_ptr<mathematica::tokens::function> list = boost::dynamic_pointer_cast<mathematica::tokens::function>(input);
//     int _nargs = list->nargs();
// 
//     if(_nargs == 3){
//         
//     }
// }


// SomeFunctionX = LibraryFunctionLoad["/home/neel/Projects/mathematicapp/build/examples/libmod02.so", "SomeFunctionX", LinkObject, LinkObject]
