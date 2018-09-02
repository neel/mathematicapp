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

#ifndef MATHEMATICAPP_OPERATORS_H
#define MATHEMATICAPP_OPERATORS_H

#include "symbol.h"
#include "m.h"
#include <type_traits>

namespace mathematica{
	
namespace detail{
    template <typename T> struct is_operand{};
    template <> struct is_operand<mathematica::m>{typedef mathematica::m result_type;};
    template <> struct is_operand<mathematica::symbol>{typedef mathematica::m result_type;};

    template <typename U, typename V> struct operation_result: is_operand<U>, is_operand<V>{};
    template <typename U> struct operation_result<U, U>: is_operand<U>{};
    template <> struct operation_result<mathematica::m, mathematica::symbol>{typedef mathematica::m result_type;};
    template <> struct operation_result<mathematica::symbol, mathematica::m>{typedef mathematica::m result_type;};
}

template <typename U, typename V>
typename detail::operation_result<U, V>::result_type operator+(const U& left, const V& right){
	return mathematica::m("Plus")(left, right);
}

template <typename U, typename V>
typename detail::operation_result<U, V>::result_type operator-(const U& left, const V& right){
	return mathematica::m("Subtract")(left, right);
}

template <typename U, typename V>
typename detail::operation_result<U, V>::result_type operator*(const U& left, const V& right){
	return mathematica::m("Times")(left, right);
}

template <typename U, typename V>
typename detail::operation_result<U, V>::result_type operator/(const U& left, const V& right){
	return mathematica::m("Divide")(left, right);
}

template <typename U, typename V>
typename detail::operation_result<U, V>::result_type operator==(const U& left, const V& right){
	return mathematica::m("Equal")(left, right);
}

template <typename U, typename V>
typename detail::operation_result<U, V>::result_type operator!=(const U& left, const V& right){
	return mathematica::m("Unequal")(left, right);
}

template <typename U, typename V>
typename detail::operation_result<U, V>::result_type operator<(const U& left, const V& right){
	return mathematica::m("Less")(left, right);
}

template <typename U, typename V>
typename detail::operation_result<U, V>::result_type operator>(const U& left, const V& right){
	return mathematica::m("Greater")(left, right);
}

template <typename U, typename V>
typename detail::operation_result<U, V>::result_type operator<=(const U& left, const V& right){
	return mathematica::m("LessEqual")(left, right);
}

template <typename U, typename V>
typename detail::operation_result<U, V>::result_type operator>=(const U& left, const V& right){
	return mathematica::m("GreaterEqual")(left, right);
}

template <typename U, typename V>
typename detail::operation_result<U, V>::result_type operator&&(const U& left, const V& right){
	return mathematica::m("And")(left, right);
}

template <typename U, typename V>
typename detail::operation_result<U, V>::result_type operator||(const U& left, const V& right){
	return mathematica::m("Or")(left, right);
}

template <typename U, typename V>
mathematica::m pow(const U& left, const V& right){
	return mathematica::m("Power")(left, right);
}


}

#endif // MATHEMATICAPP_OPERATORS_H
