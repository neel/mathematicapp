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

#include <map>
#include <string>
#include <vector>
#include <boost/bind.hpp>
#include <mathematica++/m.h>
#include <boost/tuple/tuple.hpp>

namespace mathematica{
    
template <int C>
struct property{};

namespace detail{

template <typename U, typename V>
struct deduce_serialized_type{ // type U is not handled by mathematica::m as input
    typedef mathematica::m serialized_type;
    typedef mathematica::m composite_type;
};

template <typename T>
struct deduce_serialized_type<T, T>{ // type T is handled by mathematica::m as input
    typedef T serialized_type;
    typedef T simple_type;
};
    
template <typename T>
struct pack_helper: deduce_serialized_type<T, typename mathematica::detail::M_Helper::argument<T>::type>{
    template <typename ReturnT, typename ClassT, typename CallbackT>
    static typename deduce_serialized_type<ReturnT, typename mathematica::detail::M_Helper::argument<ReturnT>::type>::simple_type compile(const ClassT& obj, CallbackT callback){
        ReturnT v = boost::bind(callback, obj)();
        return v;
    }

    template <typename ReturnT, typename ClassT, typename CallbackT>
    static typename deduce_serialized_type<ReturnT, typename mathematica::detail::M_Helper::argument<ReturnT>::type>::composite_type compile(const ClassT& obj, CallbackT callback){
        association<ReturnT> assoc;
        ReturnT v = boost::bind(callback, obj)();
        return assoc.serialize(v);
    }
};

// namespace internal{
//     template <typename U, typename V>
//     struct convert_to{ // is associated
//         static U cast(int){return U();}
//         static U cast(long long){return U();}
//         static U cast(double){return U();}
//         static U cast(const std::string&){return U();}
//         static U cast(const mathematica::composite& value){
//             U obj;
//             association<U> associator;
//             associator.restore(obj, value);
//             return obj;
//         }
//     };
//     
//     template <typename U>
//     struct convert_to<U, U>{ // is not associated
//         template <typename T>
//         static U cast(const T& value){
//             return U();
//         }
//     };
//     
//     template <typename T>
//     struct convert: convert_to<T, typename association<T>::serialized_type>{};
// }
// 
// template <typename U, typename T, typename Y=void>
// struct property_converter{
//     static T convert_to(const U& value){// If T is associated then uses a different specialization
//         T val = internal::convert<T>::cast(value);
//         std::cout << __LINE__ << " " << value << std::endl;
//         return val;        
//     }
// };
// 
// template <typename U, typename T>
// struct property_converter<U, T, typename std::enable_if<boost::is_convertible<U, T>::value>::type>{
//     static T convert_to(const U& value){
//         return T(value);
//     }
// };
// 
// template <typename T>
// struct unpack_visitor{
//     template <typename U>
//     T operator()(const U& arg){
//         return property_converter<U, T>::convert_to(arg);
//     }
// };

}

template <typename D, typename U, int c=0, typename ... Ts>
struct pack{
    typedef U class_type;
    
    void build(const class_type& obj, std::vector<mathematica::m>& rules) const {}
    void restore(class_type& obj, const mathematica::composite& composite){}
};

template <typename D, typename U, int c, typename T, typename ... Ts>
struct pack<D, U, c, T, Ts...>: pack<D, U, c+1, Ts...>{
    typedef pack<D, U, c, T, Ts...> self_type;
    typedef pack<D, U, c+1, Ts...>  base_type;
    typedef T                       property_type;
    typedef D                       derived_type;
    typedef U                       class_type;
    typedef T U::*                  callback_type;
    
    typedef std::pair<std::string, callback_type>            property_meta;
    typedef typename detail::pack_helper<T>::serialized_type serialized_type;
    enum {argument_count = c};
    
    std::string _name;
    callback_type _callback;
    
    pack(){
        // call detail(property<I>) method of the derived class for the i'th property
        property_meta details = derived_type::detail(property<argument_count>()); 
        _name = details.first;
        _callback = details.second;
    }
    
    void build(const class_type& obj, std::vector<mathematica::m>& rules) const{
        serialized_type v = detail::pack_helper<T>::template compile<property_type>(obj, _callback);
        rules.push_back(mathematica::m("Rule")(_name, v));
        base_type::build(obj, rules);
    }
    void restore(class_type& obj, const mathematica::composite& composite){
        typedef mathematica::rule<property_type> rule_type;
        rule_type rule = cast<rule_type>(composite._children[argument_count]);
        property_type v = rule.value();
        obj.*_callback = v; // set the value of the property
        base_type::restore(obj, composite);
    }
};

template <typename D, typename U, int c=0, typename ... Ts>
struct chain{
    typedef U class_type;
    
    void build(const class_type& obj, mathematica::m& list) const {}
    void restore(class_type& obj, const mathematica::composite& composite){}
};

template <typename D, typename U, int c, typename T, typename ... Ts>
struct chain<D, U, c, T, Ts...>: chain<D, U, c+1, Ts...>{
    typedef chain<D, U, c, T, Ts...> self_type;
    typedef chain<D, U, c+1, Ts...>  base_type;
    typedef T                       property_type;
    typedef D                       derived_type;
    typedef U                       class_type;
    typedef T U::*                  callback_type;
    
    typedef typename detail::pack_helper<T>::serialized_type serialized_type;
    enum {argument_count = c};
    
    callback_type _callback;
    
    chain(){
        // call detail(property<I>) method of the derived class for the i'th property
        _callback = derived_type::detail(property<argument_count>());
    }
    
    void build(const class_type& obj, mathematica::m& list) const{
        serialized_type v = detail::pack_helper<T>::template compile<property_type>(obj, _callback);
        list.arg(v);
        base_type::build(obj, list);
    }
    void restore(class_type& obj, const mathematica::composite& composite){
        property_type v = cast<property_type>(composite._children[argument_count]);
        obj.*_callback = v; // set the value of the property
        base_type::restore(obj, composite);
    }
};

template <typename D, typename U, typename ... Ts>
struct dictionary: pack<D, U, 0, Ts...> {
    typedef U                       class_type;
    typedef pack<D, U, 0, Ts...>    pack_type;
    typedef boost::tuple<Ts...>     tuple_type;
    typedef mathematica::m          target_type;
    typedef mathematica::m          serialized_type;
    
    mathematica::m serialize(const class_type& obj){
        std::vector<m> rules;
        pack_type::build(obj, rules);
        return mathematica::m("Association")(rules);
    }
    class_type deserialize(const mathematica::composite& composite){
        class_type obj;
        pack_type::restore(obj, composite);
        return obj;
    }
};

template <typename D, typename U, typename ... Ts>
struct sequence: chain<D, U, 0, Ts...> {
    typedef U                       class_type;
    typedef chain<D, U, 0, Ts...>   pack_type;
    typedef boost::tuple<Ts...>     tuple_type;
    typedef mathematica::m          target_type;
    typedef mathematica::m          serialized_type;
    
    mathematica::m serialize(const class_type& obj){
        mathematica::m list("List");
        pack_type::build(obj, list);
        list();
        return list;
    }
    class_type deserialize(const mathematica::composite& composite){
        class_type obj;
        pack_type::restore(obj, composite);
        return obj;
    }
};

}

// namespace mathematica{
//     template <typename U, typename V>
//     struct association<std::pair<U, V>>: dictionary<association<std::pair<U, V>>, std::pair<U, V>, U, V>{
//         static auto detail(property<0>){return std::make_pair("first",   &std::pair<U, V>::first);}
//         static auto detail(property<1>){return std::make_pair("second",  &std::pair<U, V>::second);}
//     };
// }

namespace mathematica{
    template <typename U, typename V>
    struct association<std::pair<U, V>>: sequence<association<std::pair<U, V>>, std::pair<U, V>, U, V>{
        static auto detail(property<0>){return &std::pair<U, V>::first;}
        static auto detail(property<1>){return &std::pair<U, V>::second;}
    };
}



#endif // MATHEMATICAPP_ASSOCIATION_H
