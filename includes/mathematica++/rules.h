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

#ifndef MATHEMATICAPP_UTILS_H
#define MATHEMATICAPP_UTILS_H

#include "symbol.h"
#include <string>
#include <boost/fusion/container/list/cons.hpp>
#include <boost/fusion/include/cons.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>

namespace mathematica{
    /**
     * both T and U are rule
     */
    template <typename T, typename U>
    struct rules_helper{
        typedef T head_type;
        typedef boost::fusion::cons<U> tail_type;
        typedef boost::fusion::cons<head_type, tail_type> type;
        
        type _rules;
        
        rules_helper(const head_type& head, const U& tail): _rules(type(head, tail_type(tail))){}
        template <typename R>/// R is a rule
        rules_helper<R, type> operator,(const R& rule) const{
            return rules_helper<R, type>(rule, _rules);
        }
        typename boost::fusion::result_of::size<type>::type size() const{
            return boost::fusion::size(_rules);
        }
        template <typename F>
        void operator()(F ftor) const{
            boost::fusion::for_each(_rules, ftor);
        }
    };
		
    /**
     * rule<int>("param", 2)
     * rule<double>("param", 2.0f)
     * make_rule("MeasureFlactuation", symbol::False());
     * Rule("Param1", 2)
     * Rule("Param1")= 2
     */
    template <typename T>
    struct rule{
        public:
            typedef T       value_type;
            typedef rule<T> self_type;
        private:
            std::string _key;
            value_type  _value;
        public:
            rule(){}
            rule(const std::string& key): _key(key){}
            rule(const self_type& other): _key(other._key), _value(other._value){}
            rule(const std::string& key, const value_type& value): _key(key), _value(value){}
            
            self_type& operator=(const value_type& value){
                _value = value;
                return *this;
            }
            template <typename U>
            bool operator==(const rule<U>& other) const{
                return _key == other.key() && _value == other.value();
            }
            template <typename U>
            bool operator!=(const rule<U>& other) const{
                return !(*this == other);
            }
            template <typename U>
            rules_helper<self_type, U> operator,(const U& rule){
                return rules_helper<self_type, U>(*this, rule);
            }
        public:
            std::string key() const{return _key;}
            value_type  value() const{return _value;}
    };
    
    /**
     * auto rules = (mathematica::make_rule("Param1", 2), mathematica::make_rule("Param2", mathematica::symbol::True()), mathematica::make_rule("Param3", 3.1415));
     */
    template <typename T>
    rule<T> make_rule(const std::string& key, const T& value){
        return rule<T>(key, value);
    }
    
    struct rule_proxy{
        std::string _name;
        rule_proxy(const std::string& name): _name(name){}
        
        template <typename T>
        rule<T> operator=(const T& value){
            return make_rule<T>(_name, value);
        }
    };
    
    /**
     * auto rules = (mathematica::Rule("Param1")= 2, mathematica::Rule("Param2")= mathematica::symbol::True(), mathematica::Rule("Param3")= 3.1415);
     */
    rule_proxy Rule(const std::string& key);
    /**
     * auto rules = (mathematica::Rule("Param1", 2), mathematica::Rule("Param2", mathematica::symbol::True()), mathematica::Rule("Param3", 3.1415));
     */
    template <typename T>
    rule<T> Rule(const std::string& key, const T& value){
        return make_rule<T>(key, value);
    }

    template <typename T, typename... U>
    struct rules_helper<T, boost::fusion::cons<U...>>{
        typedef T head_type;
        typedef boost::fusion::cons<U...> tail_type;
        typedef boost::fusion::cons<head_type, tail_type> type;
        
        type _rules;
        
        rules_helper(const head_type& head, const tail_type& tail): _rules(type(head, tail)){}
        template <typename R>/// R is a rule
        rules_helper<R, type> operator,(const R& rule) const{
            return rules_helper<R, type>(rule, _rules);
        }
        typename boost::fusion::result_of::size<type>::type size() const{
            return boost::fusion::size(_rules);
        }
        /**
         * runs a function on all rules
         */
        template <typename F>
        void operator()(F ftor) const{
            boost::fusion::for_each(_rules, ftor);
        }
    };
    
    template <typename T>
    std::ostream& operator<<(std::ostream& stream, const rule<T>& rule){
        stream << rule.key() << "->" << rule.value();
        return stream;
    }
    
    std::ostream& operator<<(std::ostream& stream, const symbol& s);
    
namespace detail{
    template <typename S>
    struct rules_io{
        S& _stream;
        
        rules_io(S& stream): _stream(stream){}
        
        template <typename T>
        void operator()(const mathematica::rule<T>& rule) const{
            _stream << rule;
        }
    };
}
    
    template <typename R1, typename R2>
    std::ostream& operator<<(std::ostream& stream, const rules_helper<R1, R2>& rules){
        detail::rules_io<std::ostream> io(stream);
        rules(io);
        return stream;
    }
}

#endif // MATHEMATICAPP_UTILS_H
