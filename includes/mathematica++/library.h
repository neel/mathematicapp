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
        tuple_type tuple = cast<tuple_type>(args);
        arguments_type arguments = internal::to_fusion(tuple);
        // https://www.boost.org/doc/libs/1_68_0/libs/fusion/doc/html/fusion/functional/invocation/functions/invoke.html
        return boost::fusion::invoke(_function, arguments);
    }
    return_type operator()(mathematica::tokens::function::ptr args){
        return call(args);
    }
    void out(mathematica::tokens::function::ptr args, mathematica::wtransport& shell){
        shell(call(args));
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

#endif // MATHEMATICAPP_LIBRARY_H
