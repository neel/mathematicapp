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

namespace mathematica{

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
