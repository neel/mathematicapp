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

#ifndef MATHEMATICAPP_SHORTHAND_H
#define MATHEMATICAPP_SHORTHAND_H

#include "mathematica++/compatibility.h"
#include <string>
#include <vector>
#include <stack>
#include <deque>
#include <functional>
#include <numeric>
#include <list>
#include "accessor.h"
#include "drivers.h"
#include <boost/bind.hpp>
#include <iostream>
#include <initializer_list>
#include <boost/algorithm/string/predicate.hpp>
#include "mathematica++/symbol.h"
#include "mathematica++/rules.h"

#ifdef USING_LIB_EIGEN
#include <Eigen/Dense>
#endif

#define M(x)  (mathematica::m((x)))
#include "defs.h"

namespace mathematica{
    template <typename T>
    struct association{};
}

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
struct argument{
    typedef mathematica::m type;
};

template <>
struct argument<int>{
    typedef int type;
    
    detail::abstract_delayed_call_ptr operator()(const int& arg){
        return detail::abstract_delayed_call_ptr(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::integer, _1, arg)));
    }
};
    
template <>
struct argument<unsigned int>{
    typedef unsigned int type;
    
    detail::abstract_delayed_call_ptr operator()(const unsigned int& arg){
        return detail::abstract_delayed_call_ptr(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::uinteger, _1, arg)));
    }
};

template <>
struct argument<long>{
    typedef long type;
    
    detail::abstract_delayed_call_ptr operator()(const long& arg){
        return detail::abstract_delayed_call_ptr(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::long_integer, _1, arg)));
    }
};

template <>
struct argument<long long>{
    typedef long long type;
    
    detail::abstract_delayed_call_ptr operator()(const long long& arg){
        return detail::abstract_delayed_call_ptr(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::long_integer, _1, arg)));
    }
};

template <>
struct argument<double>{
    typedef double type;
    
    detail::abstract_delayed_call_ptr operator()(const double& arg){
        return detail::abstract_delayed_call_ptr(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::real, _1, arg)));
    }
};

template <>
struct argument<std::string>{
    typedef std::string type;
    
    detail::abstract_delayed_call_ptr operator()(const std::string& arg){
        return detail::abstract_delayed_call_ptr(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::str, _1, arg)));
    }
};

template <>
struct argument<mathematica::symbol>{
    typedef mathematica::symbol type;
    
    detail::abstract_delayed_call_ptr operator()(const mathematica::symbol& arg){
        return detail::abstract_delayed_call_ptr(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::symbol, _1, boost::bind(&mathematica::symbol::name, arg))));
    }
};

template <typename T>
struct argument<std::complex<T>>{
    typedef std::complex<T> type;
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
    typedef mathematica::rule<T> type;
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

template <typename T>
struct argument_helper<T, typename association<T>::target_type>{
    typedef typename association<T>::target_type mtype;
    typedef std::deque<detail::abstract_delayed_call_ptr> queue_type;
    queue_type& _q;
    
    argument_helper(queue_type& queue): _q(queue){}
    void operator()(const T& arg){
        association<T> associator;
        mtype marg = associator.serialize(arg);
        argument_helper<mtype, mtype> helper(_q);
        helper(marg);
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

//{ derive the inner most type of an std::vector<std::vector<std::vector<std::vector<std::vector<T>>>>>
// typedef std::vector<std::vector<std::vector<std::vector<std::vector<T>>>>> matrix_type;
// usage bulk_transfer_inner_type<matrix_type>::inner_type
template <typename T>
struct bulk_transfer_inner_type{};
template <typename T>
struct bulk_transfer_inner_type<std::vector<T>>{
    typedef T inner_type;
};
template <typename T>
struct bulk_transfer_inner_type<std::vector<std::vector<T>>>: public bulk_transfer_inner_type<std::vector<T>>{};
//}

// { serialize a multidimentional vector to linear vector
template <typename T, typename S>
struct bulk_argument_helper{};

template <typename T>
struct bulk_argument_helper<std::vector<T>, std::vector<T>>{
    typedef std::vector<T> S;
    S& _l;
    
    bulk_argument_helper(S& list): _l(list){}
    void operator()(const std::vector<T>& arg){
        std::copy(arg.begin(), arg.end(), std::back_inserter(_l));
    }
};

template <typename T, typename S>
struct bulk_argument_helper<std::vector<std::vector<T>>, S>{
    S& _l;
    
    bulk_argument_helper(S& list): _l(list){}
    void operator()(const std::vector<std::vector<T>>& arg){
        for(const std::vector<T>& v: arg){
            bulk_argument_helper<std::vector<T>, S> helper(_l);
            helper(v);
        }
    }
};
//}

// { get dimentions of a multi dimentional vector
template <typename T>
struct bulk_dimention_helper{};

template <typename T>
struct bulk_dimention_helper<std::vector<T>>{
    typedef std::vector<int> dims_type;
    dims_type& _dims;
    
    bulk_dimention_helper(dims_type& dims): _dims(dims){}

    void operator()(const std::vector<T>& vec){
        _dims.push_back(vec.size());
    }
};

template <typename T>
struct bulk_dimention_helper<std::vector<std::vector<T>>>{
    typedef std::vector<int> dims_type;
    dims_type& _dims;
    
    bulk_dimention_helper(dims_type& dims): _dims(dims){}
    
    void operator()(const std::vector<std::vector<T>>& vec){
        _dims.push_back(vec.size());
        if(vec.size() > 0){
            bulk_dimention_helper<std::vector<T>> helper(_dims);
            helper(*vec.begin());
        }else{
            _dims.push_back(0);
        }
    }
};
//}

template <typename T, typename M_Type>
struct argument_helper<std::vector<T>, M_Type>{
    typedef std::deque<detail::abstract_delayed_call_ptr> queue_type;
    queue_type& _q;
    
    argument_helper(queue_type& queue): _q(queue){}
    
    void transfer_bulk(const std::vector<T>& arg){
        typedef typename bulk_transfer_inner_type<std::vector<T>>::inner_type value_type;
        
        std::vector<int> dimsv;
        bulk_dimention_helper<std::vector<T>> dhelper(dimsv);
        dhelper(arg);
        
        std::vector<value_type> list;
        bulk_argument_helper<std::vector<T>, std::vector<value_type>> helper(list);
        helper(arg);
        
        int expected_elements = std::accumulate(dimsv.begin(), dimsv.end(), 1, std::multiplies<int>());
        if(expected_elements == list.size()){
            typedef void (*callback_type)(mathematica::driver::ws::connection&, std::vector<value_type>, std::vector<int>);
            _q.push_back(detail::make_deyaled_call(boost::bind(static_cast<callback_type>(&mathematica::driver::ws::impl::put_array), _1, list, dimsv)));
        }else{
            transfer_chain(arg);
        }
    }
    void transfer_chain(const std::vector<T>& arg){
        _q.push_back(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::function, _1, "List", arg.size())));
        for(typename std::vector<T>::const_iterator i = arg.begin(); i != arg.end(); ++i){
            detail::M_Helper::argument_helper<T, M_Type> helper(_q);
            helper(*i);
        }
    }
    void operator()(const std::vector<T>& arg){
        if(mathematica::accessor::enabled(mathematica::bulk_io)){
            transfer_bulk(arg);
        }else{
            transfer_chain(arg);
        }
    }
};

template <typename T>
struct argument_helper<std::vector<T>, T>{
    typedef std::deque<detail::abstract_delayed_call_ptr> queue_type;
    queue_type& _q;
    
    argument_helper(queue_type& queue): _q(queue){}

    void transfer_chain(const std::vector<T>& arg){
        _q.push_back(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::function, _1, "List", arg.size())));
        for(typename std::vector<T>::const_iterator i = arg.begin(); i != arg.end(); ++i){
            detail::M_Helper::argument_helper<T, T> helper(_q);
            helper(*i);
        }
    }
    void operator()(const std::vector<T>& arg){
        transfer_chain(arg);
    }
};

#ifdef USING_LIB_EIGEN
template <typename T, int Rows, int Cols, typename M_Type>
struct argument_helper<Eigen::Matrix<T, Rows, Cols>, M_Type>{
    typedef std::deque<detail::abstract_delayed_call_ptr> queue_type;
    queue_type& _q;
    
    argument_helper(queue_type& queue): _q(queue){}    
    void operator()(const Eigen::Matrix<T, Rows, Cols>& arg){
        int mrows = arg.rows();        
        int mcols = arg.cols();
        int nvals = mrows * mcols;
        
        const T* begin = arg.data();
        const T* end = begin + nvals;
        
        std::vector<int> dims = {mrows, mcols};
        std::vector<T> elems(begin, end);
        
        typedef void (*callback_type)(mathematica::driver::ws::connection&, std::vector<T>, std::vector<int>);
        _q.push_back(detail::make_deyaled_call(boost::bind(static_cast<callback_type>(&mathematica::driver::ws::impl::put_array), _1, elems, dims)));
    }
};
#endif

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
    public:
        mathematica::symbol symbol() const{
            return mathematica::symbol(_name);
        }
};

}

#endif // MATHEMATICAPP_SHORTHAND_H
