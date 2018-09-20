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

#ifndef MATHEMATICAPP_VARIANT_H
#define MATHEMATICAPP_VARIANT_H

#include <iostream>
#include <complex>
#include <string>
#include <vector>
#include <boost/variant.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/type_traits.hpp>
#include "mathematica++/rules.h"
#include <mathematica++/serialization.h>

namespace mathematica{

template <typename variant_type>
struct composite_type{
    typedef std::vector<variant_type> children_type;
    
    std::string   _name;
    children_type _children;
    
	composite_type(){}
    composite_type(const std::string& name, const children_type& children): _name(name), _children(children){}
    bool is_list() const{
        return _name == "List";
    }
    bool is_complex() const{
        return _name == "Complex";
    }
    bool is_rule() const{
        return _name == "Rule";
    }
    bool is_graphics() const{
        return _name == "Graphics";
    }
    children_type children()const{
        return _children;
    }
};

typedef boost::make_recursive_variant<int, long long, double, std::string, composite_type<boost::recursive_variant_>>::type variant;
typedef composite_type<variant> composite;

template <typename variant_type>
std::ostream& operator<<(std::ostream& stream, const mathematica::composite_type<variant_type>& cvar){
    stream << cvar._name << "[";
    for(typename mathematica::composite_type<variant_type>::children_type::const_iterator i = cvar._children.begin(); i != cvar._children.end(); ++i){
        if(i != cvar._children.begin())
            stream << ", ";
        stream << *i;
    }
    stream << "]";
    return stream;
}

namespace detail{
//{ coercion of scalers 

template <typename U, typename T, typename=void>
struct IsConvertible{
    static T evaluate(const U& value){
        return T();
    }
};
template <typename U, typename T>
struct IsConvertible<U, T, typename std::enable_if<boost::is_convertible<U, T>::value>::type>{
    static T evaluate(const U& value){
        return static_cast<T>(value);
    }
};

    
template <typename T, typename U>
struct force_static{
    static T evaluate(const U& value){
//         return static_cast<T>(value);
        return IsConvertible<U, T>::evaluate(value);
    }
};

template <typename T, typename U>
struct force_lexical{
    static T evaluate(const U& value){
        return boost::lexical_cast<T>(value);
    }
};
// http://stackoverflow.com/questions/27709461/check-if-type-can-be-an-argument-to-boostlexical-caststring
template <typename T, typename=void>
struct IsLexCastableR{
    static T evaluate(const std::string& value){
        return T();
    }
};
template <typename T>
struct IsLexCastableR<T, typename std::enable_if<boost::has_right_shift<T>::value>::type>: public force_lexical<T, std::string> {};

template <typename T, typename=void>
struct IsLexCastableL{
		static std::string evaluate(const T& value){
        return std::string();
    }
};
template <typename T>
struct IsLexCastableL<T, typename std::enable_if<boost::has_left_shift<T>::value>::type>: public force_lexical<std::string, T> {};

//{{ mixins
template <typename T, typename U>
struct force_coercion: public force_static<T, U>{};

template <typename T>
struct force_coercion<T, T>: public force_static<T, T>{};

template <typename T>
struct force_coercion<T, std::string>: public IsLexCastableR<T> /*public force_lexical<T, std::string>*/{};

template <typename U>
struct force_coercion<std::string, U>: public IsLexCastableL<U>/*public force_lexical<std::string, U>*/{};
//}}
template <typename T>
struct coercion_visitor: public boost::static_visitor<T>{
    typedef T coerced_type;
    template <typename U>
    using evalutor_type = force_coercion<T, U>;
    
    template <typename U>
    coerced_type operator()(const U& value) const{
        return evalutor_type<U>::evaluate(value);
    }
};

//}

//{ cast
/**
 * default implementation cast scaler U in variant to scaler T
 */
template <typename T, typename U>
struct cast_helper{
	static T cast(const U& value){
		return force_coercion<T, U>::evaluate(value);
	}
};
template <typename T>
struct cast_visitor: public boost::static_visitor<T>{
    template <typename U>
    T operator()(const U& value) const{
        return cast_helper<T, U>::cast(value);
    }
};
//}

//{ vectorify 

/**
 * default implementation creates an one element container and the only item is obtained by coercion
 */
template <typename C, typename U>
struct vector_coercion_helper{
    static void fill(C& container, const U& v){
        typedef typename C::value_type value_type;
        typedef typename coercion_visitor<value_type>::template evalutor_type<U> evaluator_type;// coercer from U to T

        container.push_back(evaluator_type::evaluate(v));
    }
};

template <typename T>
struct vector_coercion_visitor: public boost::static_visitor<T>{
   typedef T coerced_type;
   typedef coerced_type container_type;
   typedef typename T::value_type value_type;

   template <typename U>
   container_type operator()(const U& value) const{
       container_type container;
       vector_coercion_helper<container_type, U>::fill(container, value);
       return container;
   }
};

//}

//{ tuplify

template <typename T, int Index>
struct tuple_iterator: public tuple_iterator<T, Index-1>{
	tuple_iterator(T& tuple, const mathematica::composite& var): tuple_iterator<T, Index-1>(tuple, var){
		typedef typename boost::tuples::element<Index, T>::type target_type;
		mathematica::variant child = var._children[Index];
		boost::tuples::get<Index>(tuple) = boost::apply_visitor(detail::cast_visitor<target_type>(), child);
	}
};
template <typename T>
struct tuple_iterator<T, -1>{
	tuple_iterator(T& tuple, const mathematica::composite& var){}
};

template <typename U>
struct tuple_helper{
	
	template <typename TupleType>
	static void transform(TupleType& tuple, const U& value){
		typedef typename boost::tuples::element<0, TupleType>::type target_type;
		boost::tuples::get<0>(tuple) = force_coercion<target_type, U>::evaluate(value);
	}
};

template <typename T>
struct tuple_coercion_visitor: public boost::static_visitor<T>{
    typedef T tuple_type;
    
    template <typename U>
    T operator()(const U& value) const{
        tuple_type tuple;
        tuple_helper<U>::template transform<tuple_type>(tuple, value);
        return tuple;
    }
};
//}

template <typename C>
struct vector_coercion_helper<C, mathematica::composite>{
    static void fill(C& container, const mathematica::composite& v){
        typedef typename C::value_type value_type;
        for(mathematica::composite::children_type::const_iterator i = v._children.begin(); i != v._children.end(); ++i){
            value_type var = boost::apply_visitor(detail::coercion_visitor<value_type>(), *i);
            container.push_back(var);
        }
    }
};

template <>
struct tuple_helper<mathematica::composite>{
	
	template <typename TupleType>
	static void transform(TupleType& tuple, const mathematica::composite& value){
		typedef tuple_iterator<TupleType, boost::tuples::length<TupleType>::value-1> tuple_iterator_type;
		tuple_iterator_type(tuple, value);
	}
};
/**
 * coercing to the actual type of mathematica::variant
 * \note only works for scalers and std::complex<double>
 */
template <typename T>
struct force_static<T, T>{
    static T evaluate(const T& value){
        return value;
    }
};

template <>
struct force_coercion<std::string, std::string>: public force_static<std::string, std::string>{};

/**
 * coerce composite in mathematica::variant to scaler T
 * \warning invalid coercion should throw error
 */
template <typename T>
struct force_static<T, composite>{
    static T evaluate(const composite& value){
        return T();
    }
};
/**
 * coerce scaler in mathematica::variant to composite
 * \warning invalid coercion should throw error
 */
template <typename T>
struct force_static<composite, T>{
    static composite evaluate(const T& value){
        return composite();
    }
};

/**
 * coerce scaler U in mathematica::variant to std::complex<T>
 */
template <typename T, typename U>
struct force_static<std::complex<T>, U>{
	typedef std::complex<T> complex_type;
	
	static complex_type evaluate(const U& value){
		T re = static_cast<T>(value); // coerce variant of value to T
		return complex_type(re, 0);
	}
};

/**
 * coercing a composite in mathematica::variant to std::complex<T>
 * \note returns default constructed complex if composite is not complex
 * \warning may throw runtime error
 */
template <typename T>
struct force_static<std::complex<T>, composite>{
    typedef std::complex<T> complex_type;
    
    static complex_type evaluate(const composite& value){
        if(value.is_complex()){
            T re = boost::apply_visitor(detail::coercion_visitor<T>(), value._children[0]); // coerce variant of value._children[0] to T
            T im = boost::apply_visitor(detail::coercion_visitor<T>(), value._children[1]);
            return complex_type(re, im);
        }
        return complex_type();
    }
};

// template <typename T>
// struct force_static<mathematica::rule<T>, composite>{
//     typedef mathematica::rule<T> rule_type;
//     
//     static rule_type evaluate(const composite& value){
//         if(value.is_rule()){
//             std::string key = boost::apply_visitor(detail::coercion_visitor<std::string>(), value._children[0]); // coerce variant of value._children[0] to std::string
//             T value = boost::apply_visitor(detail::coercion_visitor<T>(), value._children[1]);
//             return mathematica::make_rule(key, value);
//         }
//         return rule_type();
//     }
// };

template <typename T>
struct force_lexical<T, T>{
    static T evaluate(const T& value){
        return value;
    }
};

template <typename U, typename V>
struct IsAssociated{
    static U cast(const mathematica::composite& value){
        typedef association<U> aassociator_type;
        aassociator_type associator;
        U obj = associator.deserialize(value);
        return obj;
    }
};

template <typename T>
struct IsAssociated<T, T>{
    static T cast(const mathematica::composite& value){
        return T();
    }
};

/**
 * cast a composite to scaler T
 */
template <typename T>
struct cast_helper<T, mathematica::composite>{

	static T cast(const mathematica::composite& value){
        return IsAssociated<T, typename association<T>::serialized_type>::cast(value);
// 		return T();
	}
};

/**
 * cast a composite to tuple T
 */
template <typename... T>
struct cast_helper<boost::tuple<T...>, mathematica::composite>{

	static boost::tuple<T...> cast(const mathematica::composite& value){
		const mathematica::variant& var(value);
		return boost::apply_visitor(detail::tuple_coercion_visitor<boost::tuple<T...>>(), var);
	}
};
/**
 * cast a composite to std::complex<T>
 */
template <typename T>
struct cast_helper<std::complex<T>, mathematica::composite>{
	static std::complex<T> cast(const mathematica::composite& value){
		return force_coercion<std::complex<T>, mathematica::composite>::evaluate(value);
	}
};
/**
 * cast a composite to type T
 */
template <typename T>
struct cast_helper<std::vector<T>, mathematica::composite>{
	static std::vector<T> cast(const mathematica::composite& value){
		std::vector<T> container;
		typedef T value_type;
		for(mathematica::composite::children_type::const_iterator i = value._children.begin(); i != value._children.end(); ++i){
				value_type var = boost::apply_visitor(detail::cast_visitor<value_type>(), *i);
				container.push_back(var);
		}
		return container;
	}
};

template <typename U, typename V>
struct cast_helper<std::pair<U, V>, mathematica::composite>{
    typedef std::pair<U, V> pair_type;
    
    static pair_type cast(const mathematica::composite& value){
        U first;
        V second;
        if(value.is_list()){
            first = boost::apply_visitor(detail::cast_visitor<U>(), value._children[0]); // coerce variant of value._children[0] to T
            second = boost::apply_visitor(detail::cast_visitor<V>(), value._children[1]);
        }else{
            mathematica::rule<U> first_rule  = boost::apply_visitor(detail::cast_visitor<mathematica::rule<U>>(), value._children[0]);
            mathematica::rule<V> second_rule = boost::apply_visitor(detail::cast_visitor<mathematica::rule<V>>(), value._children[1]);
            first  = first_rule.value();
            second = second_rule.value();
        }
        return pair_type(first, second);
    }
};

template <typename T>
struct cast_helper<mathematica::rule<T>, mathematica::composite>{
    typedef mathematica::rule<T> rule_type;
    
    static rule_type cast(const mathematica::composite& value){
        if(value.is_rule()){
            std::string key = boost::apply_visitor(detail::coercion_visitor<std::string>(), value._children[0]); // coerce variant of value._children[0] to std::string
            T val = boost::apply_visitor(detail::cast_visitor<T>(), value._children.back());
            return mathematica::make_rule(key, val);
        }
        return rule_type();
    }
};

}

/**
 * transform to a scaler
 */
template <typename T>
T coerce(const mathematica::variant& var){
	return boost::apply_visitor(detail::coercion_visitor<T>(), var);
}

/**
 * transform to a vector
 */
template <typename T>
T vectorify(const mathematica::variant& var){
    return boost::apply_visitor(detail::vector_coercion_visitor<T>(), var);
}

/**
 * transform to a tuple
 */
template <typename T>
T tuplify(const mathematica::variant& var){
    return boost::apply_visitor(detail::tuple_coercion_visitor<T>(), var);
}

template <typename T>
T cast(const mathematica::variant& var){
    return boost::apply_visitor(detail::cast_visitor<T>(), var);
}

}

#endif // MATHEMATICAPP_VARIANT_H
