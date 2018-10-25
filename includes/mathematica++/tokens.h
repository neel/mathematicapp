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

#ifndef MATHEMATICAPP_TOKENS_H
#define MATHEMATICAPP_TOKENS_H

#include <vector>
#include <string>
#include <stack>
#include <complex>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include "mathematica++/variant.h"

namespace mathematica{

class accessor;

class token{
	int _token;
	public:
		enum token_type{
			token_null,
			token_integer,
			token_real,
			token_function,
			token_str,
			token_symbol
		};
		typedef boost::shared_ptr<token> ptr;
	protected:
		token_type _type; 
		mathematica::accessor* _accessor;
		token(mathematica::accessor* accessor, int t, token_type type);
	public:
		int mathematica_type() const;
        token_type type() const;
		virtual ~token();
		virtual void fetch() = 0;
		virtual std::string stringify() const = 0;
		virtual mathematica::variant serialize() const = 0;
    public:
        operator int() const;
        operator long long() const;
        operator double() const;
        operator std::string() const;
};

typedef token::ptr value;
std::ostream& operator<<(std::ostream& stream, const token& tkn);
std::ostream& operator<<(std::ostream& stream, const value& tkn);

template <typename T>
bool operator==(const token& tkn, const T& other){
    return other == boost::lexical_cast<T>(tkn);
}
template <typename T>
bool operator==(const T& other, const token& tkn){
    return operator==(tkn, other);
}
template <typename T>
bool operator!=(const token& tkn, const T& other){
    return !(operator==(tkn, other));
}
template <typename T>
bool operator!=(const T& other, const token& tkn){
    return !(operator==(tkn, other));
}
template <typename T>
bool operator==(token::ptr tkn, const T& other){
    return operator==(*tkn, other);
}
template <typename T>
bool operator!=(token::ptr tkn, const T& other){
    return operator!=(*tkn, other);
}

/**
 * coerce's a value to the given type
 * \note may use static_cast or lexical_cast or no cast at all
 * \warning returns default constructed T for composite types
 */
template <typename T>
T coerce(mathematica::value var){
    return coerce<T>(var->serialize());
}

/**
 * creates a container of type T having values of type T::value_type while var of children of var are coerced to T::value_type through coerce()
 * \note creates one element container for scalers
 * \warning nesting of composites not supported (cannot be supported in vector)
 */
template <typename T>
T vectorify(mathematica::value var){
   return vectorify<T>(var->serialize());
}

template <typename T>
T tuplify(mathematica::value var){
    return tuplify<T>(var->serialize());
}

template <typename T>
T cast(mathematica::value var){
    return cast<T>(var->serialize());
}

namespace tokens{
class integer: public token{
    public:
        enum data_storage{
            native,
            multiprecision
        };
    private:
        long long _data;
        std::string _data_str;
        data_storage _storage;
	public:
        typedef boost::shared_ptr<integer> ptr;
		integer(mathematica::accessor* accessor);
	public:
		virtual void fetch();
		virtual std::string stringify() const;
        virtual mathematica::variant serialize() const;
        data_storage storage() const;
        long long value() const;
        std::string value_str() const;
        
};
class real: public token{
	double _data;
	public:
        typedef boost::shared_ptr<real> ptr;
		real(mathematica::accessor* accessor);
	public:
		virtual void fetch();
		virtual std::string stringify() const;
        virtual mathematica::variant serialize() const;
		double value() const;
};
struct function: public token{
	int _nargs;
	std::string _name;
	std::vector<mathematica::value> _args;
	public:
        typedef boost::shared_ptr<function> ptr;
		function(mathematica::accessor* accessor);
	public:
		virtual void fetch();
		virtual std::string stringify() const;
        virtual mathematica::variant serialize() const;
		int nargs() const;
		std::string name() const;
        bool is_graphics() const;
	protected:
		virtual void populate();
};
class str: public token{
  std::string _message;
	public:
        typedef boost::shared_ptr<str> ptr;
		str(mathematica::accessor* accessor);
	public:
		virtual void fetch();
		virtual std::string stringify() const;
        virtual mathematica::variant serialize() const;
};
class symbol: public token{
	std::string _name;
	public:
        typedef boost::shared_ptr<symbol> ptr;
		symbol(mathematica::accessor* accessor);
	public:
		virtual void fetch();
		virtual std::string stringify() const;
        virtual mathematica::variant serialize() const;
		std::string name() const;
};
boost::shared_ptr<token> factory(accessor* accessor, int type);
}

}
#endif // MATHEMATICAPP_TOKENS_H
