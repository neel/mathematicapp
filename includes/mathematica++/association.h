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

#include <boost/type_traits/is_pod.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/bind.hpp>
#include <string>
#include <vector>
#include <map>
#include <mathematica++/m.h>

namespace mathematica{

template <typename T>
struct association{};
    
template <int C>
struct property{

};
    
template <typename D, typename U, int c=0, typename ... Ts>
struct pack{
    typedef U                       class_type;
    
    void build(const class_type& obj, std::vector<mathematica::m>& rules) const {}
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
    property_type value(const class_type& obj) const{
        property_type v = boost::bind(_callback, obj)();
        return v;
    }
    
    void build(const class_type& obj, std::vector<mathematica::m>& rules) const{
        property_type v = value(obj);
        rules.push_back(mathematica::m("Rule")(_name, v));
        base_type::build(obj, rules);
    }
};

template <typename D, typename U, typename ... Ts>
struct dictionary: pack<D, U, 0, Ts...> {
    typedef U                       class_type;
    typedef pack<D, U, 0, Ts...>    pack_type;
    typedef boost::tuple<Ts...>     tuple_type;
    typedef mathematica::m          target_type;
    
    mathematica::m serialize(const class_type& obj){
        std::vector<m> rules;
        pack_type::build(obj, rules);
        return mathematica::m("Association")(rules);
    }
};

}



#endif // MATHEMATICAPP_ASSOCIATION_H
