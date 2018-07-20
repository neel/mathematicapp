/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2017  <copyright holder> <email>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#ifndef SHORTHAND_H
#define SHORTHAND_H

#include <string>
#include <vector>
#include <stack>
#include <deque>
#include <list>
#include "accessor.h"
#include "drivers.h"
#include <boost/bind.hpp>
#include <iostream>
#include <initializer_list>
#include <boost/algorithm/string/predicate.hpp>
#include "symbol.h"
#include "rules.h"

#define M(x)  (mathematica::m((x)))
#include "defs.h"

namespace mathematica{
namespace driver{
namespace ws{
struct connection;
}
}
}

namespace mathematica{
namespace detail{
struct abstract_delayed_call{
	virtual void invoke(mathematica::driver::ws::connection& link) = 0;
    virtual ~abstract_delayed_call(){}
};
typedef boost::shared_ptr<abstract_delayed_call> abstract_delayed_call_ptr;
template <typename T>
struct delayed_call: public abstract_delayed_call{
	T _ftor;
	
	delayed_call(T f): _ftor(f){}
	virtual void invoke(mathematica::driver::ws::connection& link){
		_ftor(link);
	}
    virtual ~delayed_call(){}
};
template <typename T>
abstract_delayed_call_ptr make_deyaled_call(T f){
    return abstract_delayed_call_ptr(new delayed_call<T>(f));
}

namespace M_Helper{
template <typename T>
struct argument{};

template <>
struct argument<int>{
    detail::abstract_delayed_call_ptr operator()(const int& arg){
        return detail::abstract_delayed_call_ptr(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::integer, _1, arg)));
    }
};
    
template <>
struct argument<unsigned int>{
    detail::abstract_delayed_call_ptr operator()(const unsigned int& arg){
        return detail::abstract_delayed_call_ptr(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::uinteger, _1, arg)));
    }
};

template <>
struct argument<long>{
    detail::abstract_delayed_call_ptr operator()(const int& arg){
        return detail::abstract_delayed_call_ptr(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::long_integer, _1, arg)));
    }
};

template <>
struct argument<double>{
    detail::abstract_delayed_call_ptr operator()(const double& arg){
        return detail::abstract_delayed_call_ptr(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::real, _1, arg)));
    }
};

template <>
struct argument<std::string>{
    detail::abstract_delayed_call_ptr operator()(const std::string& arg){
        return detail::abstract_delayed_call_ptr(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::str, _1, arg)));
    }
};

template <>
struct argument<mathematica::symbol>{
    detail::abstract_delayed_call_ptr operator()(const mathematica::symbol& arg){
        return detail::abstract_delayed_call_ptr(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::symbol, _1, boost::bind(&mathematica::symbol::name, arg))));
    }
};

template <typename T>
struct argument<std::complex<T>>{
    typedef argument<std::complex<T>> self_type;
    
    static void generate_ftor(mathematica::driver::ws::connection& link, std::complex<T> z){
        mathematica::driver::ws::impl::function(link, "Complex", 2L);
        mathematica::driver::ws::impl::put(link, z.real());
        mathematica::driver::ws::impl::put(link, z.imag());
    }
    
    detail::abstract_delayed_call_ptr operator()(const std::complex<T>& arg){
        return detail::abstract_delayed_call_ptr(detail::make_deyaled_call(boost::bind(&self_type::generate_ftor, _1, arg)));
    }
};


// template <typename T>
// struct argument<std::vector<T>>{
//     typedef argument<std::vector<T>> self_type;
//     
//     static void generate_ftor(mathematica::driver::ws::connection& link, std::vector<T> l){
//         mathematica::driver::ws::impl::function(link, "List", l.size());
//         for(const T& v: l){
//             
//         }
//     }
//     
//     detail::abstract_delayed_call_ptr operator()(const std::vector<T>& arg){
//         return detail::abstract_delayed_call_ptr(detail::make_deyaled_call(boost::bind(&self_type::generate_ftor, _1, arg)));
//     }
// };

template <typename T>
struct argument<mathematica::rule<T>>{
    typedef argument<mathematica::rule<T>> self_type;
    
    static void generate_ftor(mathematica::driver::ws::connection& link, mathematica::rule<T> rule){
        mathematica::driver::ws::impl::function(link, "Rule", 2L);
        std::string key = rule.key();
        if(boost::starts_with(key, "!")){
            mathematica::driver::ws::impl::put(link, mathematica::symbol(key.substr(1)));
        }else{
            mathematica::driver::ws::impl::put(link, key);
        }
        mathematica::driver::ws::impl::put(link, rule.value());
    }
    
    detail::abstract_delayed_call_ptr operator()(const mathematica::rule<T>& arg){
        return detail::abstract_delayed_call_ptr(detail::make_deyaled_call(boost::bind(&self_type::generate_ftor, _1, arg)));
    }
};

/**
 * create a lazy scaler of value of type T
 */
template <typename T>
detail::abstract_delayed_call_ptr make_argument(const T& value){
    return M_Helper::argument<T>()(value);
}

mathematica::m convert(mathematica::token::ptr val);

template <typename T, typename M_Type>
struct argument_helper{
    typedef std::deque<detail::abstract_delayed_call_ptr> queue_type;
    queue_type& _q;
    
    argument_helper(queue_type& queue): _q(queue){}
    void operator()(const T& arg){
        _q.push_back(detail::M_Helper::make_argument(arg));
    }
};
    
template <typename T, typename U, typename M_Type>
struct argument_helper<mathematica::rules_helper<T, U>, M_Type>{
    typedef std::deque<detail::abstract_delayed_call_ptr> queue_type;
    queue_type& _q;
    
    argument_helper(queue_type& queue): _q(queue){}
    void operator()(const mathematica::rules_helper<T, U>& arg){
        _q.push_back(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::function, _1, "List", arg.size())));
        arg.operator()([&](auto r){
            _q.push_back(detail::M_Helper::make_argument(r));
        });
    }
};

/**
 * second parameter to argument_helper template is always mathematica::m so here we capture nesting of m
 */
template <typename T>
struct argument_helper<T, T>{
    typedef std::deque<detail::abstract_delayed_call_ptr> queue_type;
    queue_type& _q;
    
    argument_helper(queue_type& queue): _q(queue){}
    void operator()(const T& arg){
        while(!arg._queue.empty()){
            _q.push_back(arg._queue.front());
            const_cast<T&>(arg)._queue.pop_front();
        }
    }
};
    
template <typename M_Type>
struct argument_helper<mathematica::rule<M_Type>, M_Type>{
    typedef std::deque<detail::abstract_delayed_call_ptr> queue_type;
    queue_type& _q;
    
    argument_helper(queue_type& queue): _q(queue){}
    void operator()(const mathematica::rule<M_Type>& arg){
        _q.push_back(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::function, _1, "Rule", 2L)));
        _q.push_back(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::str, _1, arg.key())));
        detail::M_Helper::argument_helper<M_Type, M_Type> helper(_q);
        helper(arg.value());
    }
};
    
template <typename T, typename M_Type>
    struct argument_helper<mathematica::rule<std::vector<T>>, M_Type>{
    typedef std::deque<detail::abstract_delayed_call_ptr> queue_type;
    queue_type& _q;
    
    argument_helper(queue_type& queue): _q(queue){}
    void operator()(const mathematica::rule<std::vector<T>>& arg){
        _q.push_back(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::function, _1, "Rule", 2L)));
        _q.push_back(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::str, _1, arg.key())));
        detail::M_Helper::argument_helper<std::vector<T>, M_Type> helper(_q);
        helper(arg.value());
    }
};
    
template <typename T, typename M_Type>
struct argument_helper<mathematica::rule<std::list<T>>, M_Type>{
    typedef std::deque<detail::abstract_delayed_call_ptr> queue_type;
    queue_type& _q;
    
    argument_helper(queue_type& queue): _q(queue){}
    void operator()(const mathematica::rule<std::list<T>>& arg){
        _q.push_back(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::function, _1, "Rule", 2L)));
        _q.push_back(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::str, _1, arg.key())));
        detail::M_Helper::argument_helper<std::list<T>, M_Type> helper(_q);
        helper(arg.value());
    }
};

    
template <typename T, typename U, typename M_Type>
    struct argument_helper<mathematica::rule<mathematica::rules_helper<T, U>>, M_Type>{
    typedef std::deque<detail::abstract_delayed_call_ptr> queue_type;
    queue_type& _q;
    
    argument_helper(queue_type& queue): _q(queue){}
    void operator()(const mathematica::rule<mathematica::rules_helper<T, U>>& arg){
        _q.push_back(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::function, _1, "Rule", 2L)));
        _q.push_back(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::str, _1, arg.key())));
        detail::M_Helper::argument_helper<mathematica::rules_helper<T, U>, M_Type> helper(_q);
        helper(arg.value());
    }
};

template <typename T, typename M_Type>
struct argument_helper<std::initializer_list<T>, M_Type>{
    typedef std::deque<detail::abstract_delayed_call_ptr> queue_type;
    queue_type& _q;
    
    argument_helper(queue_type& queue): _q(queue){}
    void operator()(const std::initializer_list<T>& arg){
        _q.push_back(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::function, _1, "List", arg.size())));
        for(typename std::initializer_list<T>::const_iterator i = arg.begin(); i != arg.end(); ++i){
            _q.push_back(detail::M_Helper::make_argument(*i));
        }
    }
};

template <typename T, typename M_Type>
struct argument_helper<std::vector<T>, M_Type>{
    typedef std::deque<detail::abstract_delayed_call_ptr> queue_type;
    queue_type& _q;
    
    argument_helper(queue_type& queue): _q(queue){}
    void operator()(const std::vector<T>& arg){
        _q.push_back(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::function, _1, "List", arg.size())));
        for(typename std::vector<T>::const_iterator i = arg.begin(); i != arg.end(); ++i){
//             _q.push_back(detail::M_Helper::make_argument(*i));
            detail::M_Helper::argument_helper<T, M_Type> helper(_q);
            helper(*i);
        }
    }
};

template <typename T, typename M_Type>
struct argument_helper<std::list<T>, M_Type>{
    typedef std::deque<detail::abstract_delayed_call_ptr> queue_type;
    queue_type& _q;
    
    argument_helper(queue_type& queue): _q(queue){}
    void operator()(const std::list<T>& arg){
        _q.push_back(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::function, _1, "List", arg.size())));
        for(typename std::list<T>::const_iterator i = arg.begin(); i != arg.end(); ++i){
            _q.push_back(detail::M_Helper::make_argument(*i));
        }
    }
};

}

}
    
/**
 * A shortcut to build mathematica inputs which is mostly a chain function calls. Here this is internally expressed as a tree of held objects (delayed function calls).
 * Each of these held callables are described in detail namespace. They are invoked through the connection adapter.
 * mathematica::m("Factorinteger")(210)
 * mathematica::m("List")(10, 20, 30, 40, mathematica::m("List")(50, 60, 70, 80), 90, 100)
 * all mathematica::m objects have a queue of lazy calls to mathematica calls. whenever we pass an argument a to m, m's queue is enqueued with lazy calls to
 * incorporate that argument.
 */
struct m{
    m(const std::string& name);
    m(const m& other);
		
    m& operator()();
    template <typename Head, typename... Tail>
    m& operator()(Head head, Tail... tail){
        arg(head);
        operator()(tail...);
        return *this;
    }
    template <typename T>
    m& arg(const T& arg){
        detail::M_Helper::argument_helper<T, m> helper(_queue);
        helper(arg);
        ++_length;
        return *this;
    }
    m& arg(mathematica::value arg){
        mathematica::m argm = detail::M_Helper::convert(arg);
        return this->arg(argm);
    }
    template <typename T>
    m& part(const T& index){
        _queue.push_front(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::function, _1, "Part", 1)));
        _queue.push_back(detail::M_Helper::make_argument(index));
        return *this;
    }
    template <typename T>
    m& operator[](const T& index){
        return part<T>(index);
    }
    public:
        m& invoke(mathematica::driver::ws::connection& conn);
    public:
        std::string _name;
        std::deque<detail::abstract_delayed_call_ptr> _queue;
        unsigned int _length;
};

}

#endif // SHORTHAND_H
