#include <fstream>
#include <mathematica++/mathematica++.h>
#include <mathematica++/transport.h>
#include <mathematica++/defs.h>

#include <boost/noncopyable.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/function.hpp>
#include <boost/fusion/container.hpp>
#include <boost/fusion/sequence.hpp>
#include <boost/mpl/int.hpp>
#include <boost/fusion/tuple.hpp>
#include <boost/fusion/functional/invocation/invoke.hpp>

using namespace mathematica;

MATHEMATICA_DECLARE(Complex);

namespace mathematica{

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
    

    // https://stackoverflow.com/a/52667660/256007
    template<std::size_t...Is, class T>
    auto to_fusion(std::index_sequence<Is...>, T&& in ) {
        using boost::get;
        using std::get;
        return boost::fusion::make_tuple( get<Is>(std::forward<T>(in))... );
    }
    template<class...Ts>
    auto to_fusion(boost::tuple<Ts...> in ) {
        return to_fusion(std::make_index_sequence<::boost::tuples::length< boost::tuple<Ts...>>::value>{}, std::move(in) );
    }
    template<class...Ts>
    boost::fusion::tuple<Ts...> to_fusion(std::tuple<Ts...> in ) {
        return to_fusion(std::make_index_sequence<sizeof...(Ts)>{}, std::move(in) );
    }
    
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
    module_overload(const self_type& other): _function(other._function), _heads(other._heads){}
    
    self_type& operator=(std::initializer_list<std::string> heads){
        std::copy(heads.begin(), heads.end(), std::back_inserter(_heads));
        return *this;
    }
    
    /**
     * check number of arguments supplied on runtime and number of arguments with which this overload has been prepared at compile time.
     */
    bool feasible(boost::shared_ptr<mathematica::tokens::function> args) const{
        std::clog << "feasible " << __LINE__ << std::endl;
        if(_heads.empty()){
            std::clog << "feasible " << (boost::fusion::result_of::size<arguments_type>::type::value  == args->nargs()) << __LINE__ << std::endl;
            return boost::fusion::result_of::size<arguments_type>::type::value  == args->nargs();
        }else{
            if(_heads.size() != args->nargs()) return false;
            std::vector<std::string>::const_iterator it = _heads.cbegin();
            for(mathematica::value v: args->_args){
                boost::shared_ptr<mathematica::tokens::function> f = boost::dynamic_pointer_cast<mathematica::tokens::function>(v);
                if(!f || f->name() != *it++){
                    std::clog << "feasible " << false << __LINE__ << std::endl;
                    return false;
                }
            }
            std::clog << "feasible " << true << __LINE__ << std::endl;
            return true;
        }
    }
    bool feasible(mathematica::value input) const{
        boost::shared_ptr<mathematica::tokens::function> args = boost::dynamic_pointer_cast<mathematica::tokens::function>(input);
        return feasible(args);
    }
    return_type call(mathematica::value input){
        boost::shared_ptr<mathematica::tokens::function> args = boost::dynamic_pointer_cast<mathematica::tokens::function>(input);
        return call(args);
    }
    return_type call(boost::shared_ptr<mathematica::tokens::function> args){
        tuple_type tuple = cast<tuple_type>(args);
        arguments_type arguments = internal::to_fusion(tuple);
//         std::clog << arguments << std::endl;
//         // https://www.boost.org/doc/libs/1_68_0/libs/fusion/doc/html/fusion/functional/invocation/functions/invoke.html
        return boost::fusion::invoke(_function, arguments);
    }
    return_type operator()(boost::shared_ptr<mathematica::tokens::function> args){
        return call(args);
    }
    return_type operator()(mathematica::value input){
        return call(input);
    }
    void out(boost::shared_ptr<mathematica::tokens::function> args, mathematica::wtransport& shell){
        shell(call(args));
    }
    void out(mathematica::value input, mathematica::wtransport& shell){
        shell(call(input));
    }
};

template <typename F>
module_overload<F> overload(F ftor){
    return module_overload<F>(ftor);
}

struct resolver: private boost::noncopyable{
    mathematica::wtransport& _shell;
    bool _resolved;
    
    resolver(mathematica::wtransport& shell): _shell(shell), _resolved(false){}
    bool resolved() const{return _resolved;}
};

namespace library{
    struct error: public std::exception{
        int _err;
        
        explicit error(int err): _err(err){}
        virtual const char* what() const noexcept{
            return (boost::format("mathematica::library::error: internal library implementation returned (%1%)") % _err).str().c_str();
        }
        int code() const{return _err;}
    };
};

template <typename T>
mathematica::resolver& operator,(mathematica::resolver& r, T overload){
    if(!r._resolved){
        if(overload.feasible(r._shell.input())){           // args is a shared pointer to a function token boost::shared_ptr<mathematica::tokens::function>
            overload.out(r._shell.input(), r._shell);      // redirects the call to callback function and writes the returned output to shell
            r._resolved = true;
        }
    }
    return r;
}

}

template <typename T>
struct point_2d{
    T x;
    T y;
};

template <typename T>
std::ostream& operator<<(std::ostream& stream, const point_2d<T>& pt){
    stream << "(" << pt.x << " " << pt.y << ")"; 
    return stream;
}

namespace mathematica{
    template <typename T>
    struct association<point_2d<T>>: sequence<association<point_2d<T>>, point_2d<T>, T, T>{
        static auto detail(property<0>){return &point_2d<T>::x;}
        static auto detail(property<1>){return &point_2d<T>::y;}
    };
}

int some_function_impl0(point_2d<double> p1, point_2d<double> p2){
    return p1.x+p2.y;
}
int some_function_impl1(double x, double y){
    return 10;
}
mathematica::m some_function_impl2(double x){
    return Complex(x+2, x);
}

EXTERN_C DLLEXPORT mint WolframLibrary_getVersion(){
    return WolframLibraryVersion;
}
EXTERN_C DLLEXPORT mint WolframLibrary_initialize(WolframLibraryData libData){
    return 0;
}
EXTERN_C DLLEXPORT void WolframLibrary_uninitialize(WolframLibraryData libData){
    return;
}

EXTERN_C DLLEXPORT int SomeFunctionX(WolframLibraryData libData, WMK_LINK native_link){
    std::ofstream out("/tmp/log");
    std::clog.rdbuf(out.rdbuf());
    
    mathematica::wtransport shell(libData, native_link);
    mathematica::resolver resolver(shell);
    
  
//     {
//         auto resolver = overload(&some_function_impl0) = {"GeoPosition", "GeoPosition"};
//         if(resolver.feasible(shell.input())){                                        // args is a shared pointer to a function token boost::shared_ptr<mathematica::tokens::function>
//             resolver.out(shell.input(), shell);                                      // redirects the call to callback function and writes the returned output to shell
//             return LIBRARY_NO_ERROR;
//         }
//     }
//     
//     {
//         auto resolver = overload(&some_function_impl1);
//         if(resolver.feasible(shell.input())){                                        // args is a shared pointer to a function token boost::shared_ptr<mathematica::tokens::function>
//             resolver.out(shell.input(), shell);                                      // redirects the call to callback function and writes the returned output to shell
//             return LIBRARY_NO_ERROR;
//         }
//     }
// 
//     {
//         auto resolver = overload(&some_function_impl2);
//         if(resolver.feasible(shell.input())){                                        // args is a shared pointer to a function token boost::shared_ptr<mathematica::tokens::function>
//             resolver.out(shell.input(), shell);                                      // redirects the call to callback function and writes the returned output to shell
//             return LIBRARY_NO_ERROR;
//         }
//     }
    
    try{
        
        resolver, overload(&some_function_impl0) = {"GeoPosition", "GeoPosition"}
                , overload(&some_function_impl1)
                , overload(&some_function_impl2);
                
    }catch(const mathematica::library::error& err){
        std::clog << "error " << __LINE__ << std::endl;
        return err.code();
    }
    
    return LIBRARY_NO_ERROR;
}


// SomeFunctionX = LibraryFunctionLoad["/home/neel/Projects/mathematicapp/build/examples/libmod02.so", "SomeFunctionX", LinkObject, LinkObject]
