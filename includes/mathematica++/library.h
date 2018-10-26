/*
 * Copyright (c) 2018, Sunanda Bose (Neel Basu) (neel.basu.z@gmail.com) 
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met: 
 * 
 *  * Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer. 
 *  * Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY 
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY 
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH 
 * DAMAGE. 
 */

#ifndef MATHEMATICAPP_LIBRARY_H
#define MATHEMATICAPP_LIBRARY_H

#include <mathematica++/mathematica++.h>
#include <mathematica++/transport.h>
#include <mathematica++/defs.h>
#include <mathematica++/exceptions.h>
#include <boost/noncopyable.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/function.hpp>
#include <boost/fusion/container.hpp>
#include <boost/fusion/sequence.hpp>
#include <boost/mpl/int.hpp>
#include <boost/fusion/tuple.hpp>
#include <boost/fusion/functional/invocation/invoke.hpp>

namespace mathematica{

namespace internal{
    /**
     * extract the function signature
     */
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
    

    // convert boost::tuple to boost::fusion::tuple
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
    
    
    template <typename C, typename... Args>
    struct callable_internal{
        typedef callable_internal<C, Args...> self_type;
        typedef C callback_type;
        
        C _ftor;
        mathematica::transport& _shell;
        
        callable_internal(C cb, mathematica::transport& shell): _ftor(cb), _shell(shell){}
        
        typename function_signature<C>::return_type operator()(Args... args){
            return _ftor(_shell, args...);
        }
    };

    template <typename T, typename... U>
    struct callable{};

    template <typename R, typename... V>
    struct callable<R (*)(mathematica::transport&, V...)>: callable_internal<R (*)(mathematica::transport&, V...), V...>{
        typedef callable_internal<R (*)(mathematica::transport&, V...), V...> base_type;
        typedef boost::function<R (V...)> function_type;
        callable(typename base_type::callback_type cb, mathematica::transport& shell): base_type(cb, shell){}
    };
    
    template <typename T, int Index>
    struct cast_optionally{
        static T cast(mathematica::tokens::function::ptr args){
            return mathematica::cast<T>(args->_args[Index]);
        }
    };
    
    template <int Index>
    struct cast_optionally<mathematica::value, Index>{
        static mathematica::value cast(mathematica::tokens::function::ptr args){
            return args->_args[Index];
        }
    };
    
    template <int Index>
    struct cast_optionally<mathematica::lambda, Index>{
        static mathematica::lambda cast(mathematica::tokens::function::ptr args){
            return mathematica::lambda(args->_args[Index]);
        }
    };
    
    template <typename TupleT, int Index=boost::tuples::length<TupleT>::value-1>
    struct arg_to_tuple: arg_to_tuple<TupleT, Index-1>{
        typedef arg_to_tuple<TupleT, Index-1> base_type;
        
        static void convert(TupleT& tuple, mathematica::tokens::function::ptr args){
//             boost::get<Index>(tuple) = cast<boost::tuples::element<Index, TupleT>::type>(args->_args[Index]);
            boost::get<Index>(tuple) = cast_optionally<typename boost::tuples::element<Index, TupleT>::type, Index>::cast(args);
            base_type::convert(tuple, args);
        }
    };
    
    template <typename TupleT>
    struct arg_to_tuple<TupleT, 0>{
        static void convert(TupleT& tuple, mathematica::tokens::function::ptr args){
            boost::get<0>(tuple) = cast_optionally<typename boost::tuples::element<0, TupleT>::type, 0>::cast(args);
        }
    };
    
    template <typename TupleT>
    void arguments_to_tuple(TupleT& tuple, mathematica::tokens::function::ptr args){
        arg_to_tuple<TupleT>::convert(tuple, args);
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
    bool feasible(mathematica::tokens::function::ptr args) const{
        if(_heads.empty()){
            return boost::fusion::result_of::size<arguments_type>::type::value  == args->nargs();
        }else{
            if(_heads.size() != args->nargs()) return false;
            std::vector<std::string>::const_iterator it = _heads.cbegin();
            for(mathematica::value v: args->_args){
                mathematica::tokens::function::ptr f = boost::dynamic_pointer_cast<mathematica::tokens::function>(v);
                if(!f || f->name() != *it++){
                    return false;
                }
            }
            return true;
        }
    }
    return_type call(mathematica::tokens::function::ptr args){
//         tuple_type tuple = cast<tuple_type>(args);
        tuple_type tuple;
        internal::arguments_to_tuple(tuple, args);
        arguments_type arguments = internal::to_fusion(tuple);
        // https://www.boost.org/doc/libs/1_68_0/libs/fusion/doc/html/fusion/functional/invocation/functions/invoke.html
        return boost::fusion::invoke(_function, arguments);
    }
    return_type operator()(mathematica::tokens::function::ptr args){
        return call(args);
    }
    void out(mathematica::tokens::function::ptr args, mathematica::wtransport& shell){
        return_type ret = operator()(args);
        shell(ret);
    }
};

template <typename F>
module_overload<F> overload(F ftor){
    return module_overload<F>(ftor);
}

template <typename F>
auto overload(F ftor, mathematica::transport& transport){
    typedef internal::callable<F> callable_type;
    typedef typename callable_type::function_type function_type;
    function_type function = callable_type(ftor, transport);
    return overload(function);
}

struct resolver: private boost::noncopyable{
    mathematica::wtransport& _shell;
    bool _resolved;
    
    resolver(mathematica::wtransport& shell): _shell(shell), _resolved(false){}
    bool resolved() const{return _resolved;}
    int resolve(){
        if(!_resolved){
            mathematica::tokens::function::ptr args = _shell._input;
            mathematica::value args_val = args;
            throw library::exceptions::library_error(messages::overload() % args_val->stringify());
        }
        return LIBRARY_NO_ERROR;
    }
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

#endif // MATHEMATICAPP_LIBRARY_H
