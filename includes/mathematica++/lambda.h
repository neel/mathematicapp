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

#ifndef MATHEMATICAPP_LAMBDA_H
#define MATHEMATICAPP_LAMBDA_H

#include <mathematica++/m.h>
#include <mathematica++/tokens.h>
#include <mathematica++/connector.h>
#include <mathematica++/declares.h>

namespace mathematica{

struct lambda{
    struct cached{
        std::string _scope;
        std::string _key;
        mathematica::connector& _shell;
        
        cached(mathematica::connector& shell, mathematica::token::ptr expr);
        ~cached();
        template <typename... T>
        cached& operator()(T... args){
            Apply(_shell.ref(_key), Sequence(args...));
            return *this;
        }
    };
    
    mathematica::token::ptr _expr;
    
    lambda(){}
    lambda(mathematica::token::ptr expr): _expr(expr){}
    lambda(const lambda& other): _expr(other._expr){}
    template <typename... T>
    mathematica::m operator()(T... args){
        return Apply(_expr, List(args...));
    }
    cached cache(mathematica::connector& shell) const;
};

}

#endif // MATHEMATICAPP_LAMBDA_H
