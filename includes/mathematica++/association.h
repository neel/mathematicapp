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

#ifndef MATHEMATICAPP_ASSOCIATION_H
#define MATHEMATICAPP_ASSOCIATION_H

#include <boost/tuple/tuple.hpp>
#include <boost/bind.hpp>

#include <string>
#include <vector>
#include <map>

struct point{
    std::pair<int, int> location;
    std::string name;
    double weight;
};

namespace mathematica{

// template <typename T>
// struct assoc{
//     
// };

template <int C>
struct property{

};
    
template <typename D, typename U, int c=0, typename ... Ts>
struct pack{
    std::string build() const {
        return std::string();
    }
};

template <typename D, typename U, int c, typename T, typename ... Ts>
struct pack<D, U, c, T, Ts...>: pack<D, U, c+1, Ts...>{
    typedef pack<D, U, c, T, Ts...> self_type;
    typedef pack<D, U, c+1, Ts...>  base_type;
    typedef T                       property_type;
    typedef D                       derived_type;
    typedef U                       class_type;
    typedef T U::*                  callback_type;
    
    typedef std::pair<std::string, callback_type> property_meta;
    enum {argument_count = c};
    
    std::string _name;
    callback_type _callback;
    
    pack(){
        // https://stackoverflow.com/a/3057522/256007
        property_meta details = derived_type::detail(property<argument_count>());
        
        _name = details.first;
        _callback = details.second;
    }
    std::string build() const{
        std::string buff = _name+" ";
        return buff + base_type::build();
    }
};
    
// template <typename U, typename T>
// struct property{
//     typedef U class_type;
//     typedef T property_type;
//     typedef T U::* callback_type;
//     
//     std::string   _name;
//     callback_type _callback;
// 
//     property(const std::string& name, callback_type property): _name(name), _callback(property){}
//     
//     property<U, T>& operator()(point& p, const property_type& value){
//         boost::bind(_callback, p) = value;
//         return *this;
//     }
// };

// template <>
// struct assoc<point>{
//     property<point, std::pair<int, int>>   location;
//     property<point, std::string>           name;
//     property<point, double>                weight;
//     
//     assoc(): 
//         location("location", &point::location), 
//         name("name", &point::name), 
//         weight("weight", &point::weight){}
// };

struct point_meta: pack<point_meta, point, 0, std::pair<int, int>, std::string, double>{
    static auto detail(property<0>){return std::make_pair("location",   &point::location);}
    static auto detail(property<1>){return std::make_pair("name",       &point::name);}
    static auto detail(property<2>){return std::make_pair("weight",     &point::weight);}
};

}

#endif // MATHEMATICAPP_ASSOCIATION_H
