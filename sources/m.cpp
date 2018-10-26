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

#include "mathematica++/m.h"
#include <boost/ref.hpp>
#include <boost/assert.hpp>
#include <sstream>

mathematica::m::m(const std::string& name): _name(name), _length(0){}
mathematica::m::m(const mathematica::m& other): _name(other._name), _queue(other._queue), _length(other._length){}

mathematica::m& mathematica::m::operator()(){
    _queue.push_front(detail::make_deyaled_call(boost::bind(&mathematica::driver::io::impl::function, _1, _name, _length)));
    return *this;
}

mathematica::m& mathematica::m::invoke(mathematica::driver::io::connection& conn){
//     _queue.push_front(detail::make_deyaled_call(boost::bind(&mathematica::driver::io::impl::function, boost::ref(conn), "EvaluatePacket", 1)));
    while(!_queue.empty()){
        _queue.front()->invoke(conn);
        _queue.pop_front();
    }
    return *this;
}

void mathematica::detail::M_Helper::convert(mathematica::m& root, mathematica::token::ptr val){
    if(val->type() == mathematica::token::token_function){
        boost::shared_ptr<mathematica::tokens::function> token_fnc = boost::dynamic_pointer_cast<mathematica::tokens::function>(val);
        root._name = token_fnc->name();
        for(const mathematica::value& arg: token_fnc->_args){
            if(arg->type() == mathematica::token::token_integer){
                auto token_int = boost::dynamic_pointer_cast<mathematica::tokens::integer>(arg);
                if(token_int->storage() == mathematica::tokens::integer::multiprecision){
                    std::string valstr = token_int->value_str();
                    valstr.erase(0, 1);
                    root.arg(mathematica::m("ToExpression")(valstr));
                }else{
                    root.arg((long long)*token_int);
                }
            }else if(arg->type() == mathematica::token::token_real){
                root.arg((double)*arg);
            }else if(arg->type() == mathematica::token::token_str){
                root.arg(arg->stringify());
            }else if(arg->type() == mathematica::token::token_symbol){
                root.arg(mathematica::symbol(arg->stringify()));
            }else if(arg->type() == mathematica::token::token_function){
                mathematica::m childf("Evaluate");
                convert(childf, arg);
                root.arg(childf);
            }
        }
    }else{
        root._name = "Evaluate";
        if(val->type() == mathematica::token::token_integer){
            auto token_int = boost::dynamic_pointer_cast<mathematica::tokens::integer>(val);
            if(token_int->storage() == mathematica::tokens::integer::multiprecision){
                std::string valstr = token_int->value_str();
                valstr.erase(0, 1);
                root.arg(mathematica::m("ToExpression")(valstr));
            }else{
                root.arg((long long)*token_int);
            }
        }else if(val->type() == mathematica::token::token_real){
            root.arg((double)*val);
        }else if(val->type() == mathematica::token::token_str){
            root.arg(val->stringify());
        }else if(val->type() == mathematica::token::token_symbol){
            root.arg(mathematica::symbol(val->stringify()));
        }
    }
    root();
}

mathematica::m mathematica::detail::M_Helper::convert(const mathematica::value val){
    mathematica::m root("Evaluate");
    convert(root, val);
    return root;
}

// mathematica::m mathematica::detail::M_Helper::convert(const mathematica::value val){
//     if(val->type() == mathematica::token::token_integer){
//         auto token_int = boost::dynamic_pointer_cast<mathematica::tokens::integer>(val);
//         if(token_int->storage() == mathematica::tokens::integer::multiprecision){
//             std::string valstr = token_int->value_str();
//             valstr.erase(0, 1);
//             return mathematica::m("Evaluate")(mathematica::m("ToExpression")(valstr));
//         }else{
//             return mathematica::m("Evaluate")((long long)*token_int);
//         }
//     }else if(val->type() == mathematica::token::token_real){
//         return mathematica::m("Evaluate")((double)*val);
//     }else if(val->type() == mathematica::token::token_str){
//         return mathematica::m("Evaluate")(val->stringify());
//     }else if(val->type() == mathematica::token::token_symbol){
//         return mathematica::m("Evaluate")(mathematica::symbol(val->stringify()));
//     }else if(val->type() == mathematica::token::token_function){
//         boost::shared_ptr<mathematica::tokens::function> token_fnc = boost::dynamic_pointer_cast<mathematica::tokens::function>(val);
//         mathematica::m fnc(token_fnc->name());
//         for(const mathematica::value& arg: token_fnc->_args){
//             mathematica::m argm = convert(arg);
//             fnc.arg(argm);
//         }
//         fnc();
//         return fnc;
//     }else {
//         std::stringstream str;
//         str << "Unexpected token received of type " << val->type();
// 
//         BOOST_ASSERT_MSG(true, str.str().c_str());
//         return mathematica::m("Evaluate")(mathematica::m("ToExpression")(val->stringify()));
//     }
// }

// std::ostream& mathematica::operator<<(std::ostream& stream, const mathematica::m& expr){
//     stream << expr._name;
//     return stream;
// }
