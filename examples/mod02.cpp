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

namespace internal{
    template <typename T>
    struct function_signature{};
    
    template <typename R, typename... Args>
    struct function_signature<R (*)(Args...)>{
        typedef R                               return_type;
        typedef boost::tuple<Args...>           tuple_type;
        typedef boost::fusion::tuple<Args...>   arguments_type;
    };
    
    template <typename R, typename... Args>
    struct function_signature<boost::function<R (Args...)> >{
        typedef R                               return_type;
        typedef boost::tuple<Args...>           tuple_type;
        typedef boost::fusion::tuple<Args...>   arguments_type;
    };
}

template <typename Function>
struct module_overload{
    typedef Function                                                           function_type;
    typedef module_overload<Function>                                          self_type;
    typedef typename internal::function_signature<Function>::return_type       return_type;
    typedef typename internal::function_signature<Function>::tuple_type        tuple_type;
    typedef typename internal::function_signature<Function>::arguments_type    arguments_type;
    
    function_type _function;
    std::vector<std::string> _heads;
    
    module_overload(function_type f): _function(f){}
    
    self_type& operator=(std::initializer_list<std::string> heads){
        std::copy(heads.begin(), heads.end(), std::back_inserter(_heads));
        return *this;
    }
    
    /**
     * check number of arguments supplied on runtime and number of arguments with which this overload has been prepared at compile time.
     */
    bool feasible(boost::shared_ptr<mathematica::tokens::function> args) const{
        if(_heads.empty()){
            return boost::fusion::result_of::size<arguments_type>::type::value  == args->nargs();
        }else{
            if(_heads.size() != args->nargs()) return false;
            std::vector<std::string>::const_iterator it = _heads.cbegin();
            for(mathematica::value v: args->_args){
                boost::shared_ptr<mathematica::tokens::function> f = boost::dynamic_pointer_cast<mathematica::tokens::function>(v);
                if(!f || f->name() != *it++){
                    return false;
                }
            }
            return true;
        }
    }
    return_type call(boost::shared_ptr<mathematica::tokens::function> args){
        arguments_type arguments = cast<arguments_type>(args);
        // https://www.boost.org/doc/libs/1_68_0/libs/fusion/doc/html/fusion/functional/invocation/functions/invoke.html
        return boost::fusion::invoke(_function, arguments);
    }
    return_type operator()(boost::shared_ptr<mathematica::tokens::function> args){
        return call(args);
    }
    void out(boost::shared_ptr<mathematica::tokens::function> args, mathematica::wtransport& shell){
        shell(call(args));
    }
};

template <typename F>
module_overload<F> resolve_overload(F ftor){
    return module_overload<F>(ftor);
}

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
    return 1;
}
int some_function_impl2(double x){
    return 2;
}
int some_function_impl0(point_2d<double> p1, point_2d<double> p2){
    return 0;
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
        auto resolver = resolve_overload(&some_function_impl0) = {"GeoPosition", "GeoPosition"};
        if(resolver.feasible(args)){                                        // args is a shared pointer to a function token boost::shared_ptr<mathematica::tokens::function>
            resolver.out(args, shell);                                      // redirects the call to callback function and writes the returned output to shell
            return LIBRARY_NO_ERROR;
        }
    }
    
    {
        auto resolver = resolve_overload(&some_function_impl1);
        if(resolver.feasible(args)){                                        // args is a shared pointer to a function token boost::shared_ptr<mathematica::tokens::function>
            resolver.out(args, shell);                                      // redirects the call to callback function and writes the returned output to shell
            return LIBRARY_NO_ERROR;
        }
    }

    {
        auto resolver = resolve_overload(&some_function_impl2);
        if(resolver.feasible(args)){                                        // args is a shared pointer to a function token boost::shared_ptr<mathematica::tokens::function>
            resolver.out(args, shell);                                      // redirects the call to callback function and writes the returned output to shell
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
