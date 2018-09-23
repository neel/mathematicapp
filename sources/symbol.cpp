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

#include "mathematica++/symbol.h"
#include "mathematica++/m.h"

mathematica::symbol::symbol(const std::string& name): _name(name){}
mathematica::symbol::symbol(const mathematica::symbol& other): _name(other._name){}

mathematica::symbol mathematica::symbol::True(){
    return mathematica::symbol("True");
}

mathematica::symbol mathematica::symbol::False(){
    return mathematica::symbol("False");
}

mathematica::symbol mathematica::symbol::Pi(){
    return mathematica::symbol("Pi");
}

mathematica::symbol mathematica::symbol::E(){
    return mathematica::symbol("E");
}

mathematica::symbol mathematica::symbol::I(){
    return mathematica::symbol("I");
}

mathematica::symbol mathematica::symbol::Infinity(){
    return mathematica::symbol("Infinity");
}


mathematica::symbol mathematica::symbol::var(const std::string& name){
    return mathematica::symbol(name);
}

std::string mathematica::symbol::name() const{
    return _name;
}

mathematica::m mathematica::symbol::operator=(const mathematica::m& rhs){
    return mathematica::m("Set")(*this, rhs);
}

mathematica::m mathematica::symbol::sub(int n){
    return mathematica::m("Subscript")(*this, n);
}
