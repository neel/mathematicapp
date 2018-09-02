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

#ifndef MATHEMATICAPP_PACKET_H
#define MATHEMATICAPP_PACKET_H

#include <map>
#include <string>
#include <vector>
#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include "mathematica++/defs.h"
#include "mathematica++/symbol.h"
#include "mathematica++/tokens.h"

namespace mathematica{
struct packet{  
    typedef boost::shared_ptr<packet> ptr;
    
    packet(mathematica::packet_type type);
    virtual ~packet(){}
    mathematica::packet_type type() const;
    virtual std::ostream& write(std::ostream& stream) const = 0;
    private:
        mathematica::packet_type _type;
};
namespace packets{

template <mathematica::packet_type pkt>
struct packet: public mathematica::packet{
    typedef packet<pkt> self_type;
    typedef boost::shared_ptr<self_type> ptr;
    
    mathematica::token::ptr _token;
    
    packet(mathematica::token::ptr token): mathematica::packet(pkt), _token(token){}
    virtual ~packet(){}
    virtual std::ostream& write(std::ostream& stream) const{
        stream << boost::format("<packet %1%>") % type();
        stream << _token;
        return stream;
    }
};

template <>
struct packet<mathematica::packet_INPUTNAME>: public mathematica::packet{
    typedef packet<mathematica::packet_INPUTNAME> self_type;
    typedef boost::shared_ptr<self_type> ptr;
    
    std::string _prompt;
    
    packet(const std::string& prompt): mathematica::packet(mathematica::packet_INPUTNAME), _prompt(prompt){}
    packet(mathematica::token::ptr token): mathematica::packet(mathematica::packet_INPUTNAME), _prompt(token->stringify()){}
    virtual ~packet(){}
    std::string prompt() const{return _prompt;}
    virtual std::ostream& write(std::ostream& stream) const{
        stream << boost::format("<packet %1%>") % packet_INPUTNAME;
        stream << _prompt;
        return stream;
    }
};
typedef packet<mathematica::packet_INPUTNAME> InputName;

template <>
struct packet<mathematica::packet_OUTPUTNAME>: public mathematica::packet{
    typedef packet<mathematica::packet_OUTPUTNAME> self_type;
    typedef boost::shared_ptr<self_type> ptr;
    
    std::string _prompt;
    
    packet(const std::string& prompt): mathematica::packet(mathematica::packet_OUTPUTNAME), _prompt(prompt){}
    packet(mathematica::token::ptr token): mathematica::packet(mathematica::packet_OUTPUTNAME), _prompt(token->stringify()){}
    virtual ~packet(){}
    std::string prompt() const{return _prompt;}
    virtual std::ostream& write(std::ostream& stream) const{
        stream << boost::format("<packet %1%>") % packet_OUTPUTNAME;
        stream << _prompt;
        return stream;
    }
};
typedef packet<mathematica::packet_OUTPUTNAME> OutputName;

template <>
struct packet<mathematica::packet_TEXT>: public mathematica::packet{
    typedef packet<mathematica::packet_TEXT> self_type;
    typedef boost::shared_ptr<self_type> ptr;
    
    mathematica::token::ptr _text;
    
    packet(mathematica::token::ptr token): mathematica::packet(mathematica::packet_TEXT), _text(token){
        std::cout << token << std::endl;
    }
    virtual ~packet(){}
    std::string text() const{return _text->stringify();}
    virtual std::ostream& write(std::ostream& stream) const{
        stream << boost::format("packet %1% %2%") % packet_MESSAGE % _text;
        return stream;
    }
};
typedef packet<mathematica::packet_TEXT> Text;

template <>
struct packet<mathematica::packet_MESSAGE>: public mathematica::packet{
    typedef packet<mathematica::packet_MESSAGE> self_type;
    typedef boost::shared_ptr<self_type> ptr;
    
    mathematica::symbol _key;
    std::string _tag;
    Text::ptr _text;
    
    packet(const mathematica::symbol& key, const std::string& tag): mathematica::packet(mathematica::packet_MESSAGE), _key(key), _tag(tag){}
    packet(mathematica::token::ptr token): mathematica::packet(mathematica::packet_MESSAGE), _key(token->stringify()){}
    virtual ~packet(){}
    virtual std::ostream& write(std::ostream& stream) const{
        stream << boost::format("packet %1% %2%:%3%") % packet_MESSAGE % _key.name() % _tag;
        return stream;
    }
    void set_text(Text::ptr text){
        _text = text;
    }
    std::string message() const{return _text->text();}
};
typedef packet<mathematica::packet_MESSAGE> Message;

template <>
struct packet<mathematica::packet_RETURN>: public mathematica::packet{
    typedef std::multimap<mathematica::packet_type, mathematica::packet::ptr> tail_type;
    typedef std::pair<tail_type::const_iterator, tail_type::const_iterator> range_type;
    typedef packet<mathematica::packet_RETURN> self_type;
    typedef boost::shared_ptr<self_type> ptr;
    
    mathematica::token::ptr _token;
    tail_type _tail;
    
    packet(mathematica::token::ptr token): mathematica::packet(mathematica::packet_RETURN), _token(token){}
    virtual ~packet(){}
    mathematica::token::ptr token() const{return _token;}
    virtual std::ostream& write(std::ostream& stream) const{
        stream << boost::format("packet %1%") % packet_RETURN;
        stream << _token;
        return stream;
    }
    void relate(mathematica::packet::ptr packet){_tail.insert(std::make_pair(packet->type(), packet));}
    range_type related(mathematica::packet_type type) const{return _tail.equal_range(type);}
    tail_type::const_iterator begin(mathematica::packet_type type) const{return _tail.lower_bound(type);}
    tail_type::const_iterator end(mathematica::packet_type type) const{return _tail.upper_bound(type);}
};
typedef packet<mathematica::packet_RETURN> Return;

template <typename... Args>
boost::shared_ptr<mathematica::packet> create(mathematica::packet_type mtype, Args... args){
    switch(mtype){
        case mathematica::packet_INPUTNAME:
            return boost::shared_ptr<mathematica::packet>(new InputName(args...));
        case mathematica::packet_OUTPUTNAME:
            return boost::shared_ptr<mathematica::packet>(new OutputName(args...));
        case mathematica::packet_MESSAGE:
            return boost::shared_ptr<mathematica::packet>(new Message(args...));
        case mathematica::packet_TEXT:
            return boost::shared_ptr<mathematica::packet>(new Text(args...));
        case mathematica::packet_DISPLAY:
            return boost::shared_ptr<mathematica::packet>(new mathematica::packets::packet<mathematica::packet_DISPLAY>(args...));
        case mathematica::packet_SYNTAX:
            return boost::shared_ptr<mathematica::packet>(new mathematica::packets::packet<mathematica::packet_SYNTAX>(args...));
        case mathematica::packet_RETURN:
            return boost::shared_ptr<mathematica::packet>(new Return(args...));
        default:
            std::cout << "UNEXPECTED PACKET TYPE: " << mtype << std::endl;
            return boost::shared_ptr<mathematica::packet>();
    }
}

}

std::ostream& operator<<(std::ostream& stream, const packet& pkt);

}

#endif // MATHEMATICAPP_PACKET_H
