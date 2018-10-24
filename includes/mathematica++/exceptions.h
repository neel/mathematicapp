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

#ifndef MATHEMATICAPP_EXCEPTIONS_H
#define MATHEMATICAPP_EXCEPTIONS_H

#ifdef USING_ML
#include "mathlink.h"
#else
#include "wstp.h"
#endif

#ifdef USING_LL
#include <WolframLibrary.h>
#endif

#include "compatibility.h"
#include <string>
#include <stdexcept>
#include <boost/format.hpp>
#include "mathematica++/m.h"
#include <boost/core/noncopyable.hpp>

namespace mathematica{
#ifdef USING_LL
struct transport;
#endif
class wrapper;
namespace library{
namespace exceptions{
struct library_error;
}
}
    
namespace driver{
namespace io{
struct connection;
}
}

/**
 * not to be used directly, either use builtin_message or message<T> derived T instead
 */
struct basic_message{
#ifdef USING_LL
    friend struct library::exceptions::library_error;
#endif
    std::string _tag;
    mathematica::m _args;
    
    basic_message(const std::string& tag);
    basic_message(basic_message &&) = delete;
    basic_message& operator=(const basic_message&) = delete;
    basic_message& operator=(basic_message&&) = delete;
    void pass(mathematica::wrapper& shell, std::string library_name="");
#ifdef USING_LL
    void pass(mathematica::transport& shell, std::string library_name="");
#endif
    template <typename T>
    basic_message& operator%(const T& e){
        // _args = m("Append")(_args, e);
        _args.arg(e);
        return *this;
    }
    virtual ~basic_message() = 0;
    protected:
        basic_message(const basic_message& other);
};

struct message: basic_message{
    message(const std::string& tag);
    message(const message& other);
    message(message &&) = delete;
    message& operator=(message&&) = delete;
    ~message();
};

struct user_message{
    /**
     * messages are registered through mathematica::transport at the time of library initialization
     */
    virtual mathematica::m registration(const std::string& library_name) const = 0;
};

template <typename T>
struct library_message: basic_message, user_message{
    std::string _detail;
    
    library_message(): basic_message(T::tag), _detail(T::detail){}
    library_message(const library_message& other): _detail(other._detail){}
    ~library_message(){}
    mathematica::m registration(const std::string& library_name) const{
        mathematica::m expr = m("Set")(m("MessageName")(symbol(library_name), _tag), _detail);
        return expr;
    }
};

namespace messages{
    
struct libmsg: library_message<libmsg>{
    static constexpr const char* tag = "libmsg";
    static constexpr const char* detail = "`1`";
};
    
struct argx: library_message<argx>{
    static constexpr const char* tag = "argx";
    static constexpr const char* detail = "called with `1` argument(s) `2` argument(s) is expected.";
};

struct overload: library_message<overload>{
    static constexpr const char* tag = "overload";
    static constexpr const char* detail = "called with `1` arguments; no matching overloads found";
};

struct type: library_message<type>{
    static constexpr const char* tag = "type";
    static constexpr const char* detail = "unexpected type encountered";
};

struct rankerror: library_message<rankerror>{
    static constexpr const char* tag = "rankerror";
    static constexpr const char* detail = "unexpected rank `1`; expecting rank `2`";
};

struct tensortype: library_message<tensortype>{
    static constexpr const char* tag = "tensortype";
    static constexpr const char* detail = "unexpected type of tensor encountered; expecting tensor of type `1`";
};



}

namespace exceptions{
/**
 * This error is emitted from the C++ side of mathematica++ intended to the C++ user code 
 * thrown mostly when mathematica++ encounters an irrecoverable error while communicating with mathematica
 * should not be thrown from inside LibraryLink towards frontend (Intended to be caught in the C++ side)
 */
struct error: public std::runtime_error{
	int 		_code;
	std::string _context;
	std::string _message;
	
	error(int ec, const std::string& context, const std::string& message);
	int code() const{return _code;}
	const std::string& context() const{return _context;}
	virtual const char* what() const noexcept;
};

error dispatch(WMK_LINK link, const std::string& context);
error dispatch(mathematica::driver::io::connection& conn, const std::string& context);
}

#ifdef USING_LL

namespace library{
namespace exceptions{
/**
 * thrown from inside the LibraryLink based library intended to be sent to the frontend.
 * must have an error code
 * LIBRARY_NO_ERROR = 0,
 * LIBRARY_TYPE_ERROR,
 * LIBRARY_RANK_ERROR,
 * LIBRARY_DIMENSION_ERROR,
 * LIBRARY_NUMERICAL_ERROR,
 * LIBRARY_MEMORY_ERROR,
 * LIBRARY_FUNCTION_ERROR,
 * LIBRARY_VERSION_ERROR
 */
struct internal_error: public std::exception{
    int _err;
    
    explicit internal_error(int err): _err(err){}
    virtual const char* what() const noexcept{
        return (boost::format("mathematica::library::error: internal library implementation returned (%1%)") % _err).str().c_str();
    }
    virtual void pass(mathematica::transport& shell, std::string library_name="");
    int code() const{return _err;}
    
};

/**
 * Exception with a message. May optionally contain an extra error message with the exception
 */
struct library_error: public mathematica::basic_message, public internal_error {
    std::string _errmsg;
    
    library_error(const basic_message& msg, int err=LIBRARY_NO_ERROR, std::string errmsg=""): mathematica::basic_message(msg), internal_error(err), _errmsg(errmsg){}
    library_error(int err, const basic_message& msg, std::string errmsg=""): mathematica::basic_message(msg), internal_error(err), _errmsg(errmsg){}
    
    virtual const char* what() const noexcept{
        if(_errmsg.empty()){
            return internal_error::what();
        }else{
            return _errmsg.c_str();
        }
    }
    virtual void pass(mathematica::transport& shell, std::string library_name="");
};

}
}

#endif

}

#endif // MATHEMATICAPP_EXCEPTIONS_H
