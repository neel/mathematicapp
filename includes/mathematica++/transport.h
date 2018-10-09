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

#ifndef MATHEMATICAPP_TRANSPORT_H
#define MATHEMATICAPP_TRANSPORT_H

#include <mathematica++/connection.h>
#include <WolframLibrary.h>
#include <mathematica++/compatibility.h>
#include <mathematica++/connector.h>
#include <mathematica++/io.h>
#include <mathematica++/m.h>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <cstring>
#include <complex>

namespace mathematica{
    
/**
 * argument adapter only handles integer, real, complex types and string with some restrictions
 * conversion from MArgument to std::string or char* is supported
 * however from std::string is not supported, only from char* is supported
 * currently MTensor is not supported inside MArgument, user code may use LinkObject to transfer such expressions easily
 */
struct argument_adapter{
    MArgument _source;
    
    argument_adapter(MArgument src): _source(src){}
    
    template <typename T>
    typename boost::enable_if<boost::is_integral<T>, argument_adapter&>::type operator=(T val){
        MArgument_getInteger(_source) = val;
        return *this;
    }
    template <typename T>
    typename boost::enable_if<boost::is_floating_point<T>, argument_adapter&>::type operator=(T val){
        MArgument_getReal(_source) = val;
        return *this;
    }
    argument_adapter& operator=(char* val){
        MArgument_getUTF8String(_source) = val;
        return *this;
    }
    template <typename T>
    argument_adapter& operator=(const std::complex<T> val){
        mcomplex& cmplx = MArgument_getComplex(_source);
        mcreal(cmplx) = val.real();
        mcimag(cmplx) = val.imag();
        return *this;
    }
    operator char* const(){
        return MArgument_getUTF8String(_source);
    }
    operator std::string() const{
        const char* str = MArgument_getUTF8String(_source);
        int len = strlen(str);
        return std::string(str, len);
    }
    template <typename T>
    operator std::complex<T>() const{
        mcomplex cmplx = MArgument_getComplex(_source);
        return std::complex<T>(mcreal(cmplx), mcimag(cmplx));
    }
    template <typename T>
    operator typename boost::enable_if<boost::is_integral<T>, T>::type const(){
        return MArgument_getInteger(_source);
    }
    template <typename T>
    operator typename boost::enable_if<boost::is_floating_point<T>, T>::type const(){
        return MArgument_getReal(_source);
    }
};


struct basic_transport{
    typedef WMK_LINK link_type;
    
    WMK_LINK _link;
    boost::shared_ptr<driver::io::connection> _connection;
    mathematica::connector _shell;
    
    basic_transport(link_type link);
    template <typename T>
    void send(const T& expr){
        _shell << expr;
    }
    template <typename T>
    void recv(T& val){
        _shell >> val;
    }
    mathematica::value recv();
    link_type link() const{return _link;}
};

struct transport{
    typedef basic_transport::link_type link_type;
    
    WolframLibraryData _data;
    basic_transport _io;
    
    transport(WolframLibraryData data);
    template <typename T>
    void send(const T& expr){
        _io.send(expr);
        _data->processWSLINK(_io.link());
    }
    template <typename T>
    void recv(T& val){
        _io.recv(val);
    }
    void ignore(){_io._shell.ignore();}
};
    
struct wtransport: transport{
    basic_transport _control;
    boost::shared_ptr<mathematica::tokens::function> _input;
    
    wtransport(WolframLibraryData data, link_type link);
    /**
     * return a value from the library function
     */
    template <typename T>
    int operator()(const T& val){
        _control.send(val);
        return LIBRARY_NO_ERROR;
    }
    boost::shared_ptr<mathematica::tokens::function> input() const{return _input;}
    template <typename T>
    T input() const{
        mathematica::value val = input();
        T ret = cast<T>(val);
        return ret;
    }
};

namespace internal{
template <typename T, int N=0>
struct argument_to_tuple{
    typedef T tuple_type;
    
    template <typename U>
    static void convert(T& tuple, U& shell){
        boost::get<N>(tuple) = shell.arg(N);
        argument_to_tuple<T, N+1>::convert(tuple, shell);
    }
};
}

struct mtransport: transport{
    int _argc;
    MArgument* _argv;
    MArgument _res;
    std::vector<argument_adapter*> _arguments;
    argument_adapter _result;
    
    mtransport(WolframLibraryData data, int argc, MArgument* argv, MArgument res);
    ~mtransport();
    const argument_adapter& arg(std::size_t index) const;
    argument_adapter& res();
    std::size_t nargs() const;
    
    template <typename... T>
    boost::tuple<T...> as() const{
        typedef boost::tuple<T...> tuple_type;
        tuple_type tuple;
        internal::argument_to_tuple<boost::tuple<T...>>::convert(tuple, *this);
        return tuple;
    }
    template <typename... T>
    operator boost::tuple<T...>() const{
        return as<T...>();
    }
};

template <typename T>
transport& operator<<(transport& stream, const T& expr){
    stream.send(expr);
    return stream;
}

template <typename T>
transport& operator>>(transport& stream, T& val){
    stream.recv(val);
    return stream;
}

}

#endif // TRANSPORT_H
