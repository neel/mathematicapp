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

#include "mathematica++/history.h"

void mathematica::history::record(mathematica::packets::packet< mathematica::packet_RETURN >::ptr packet){
    _records.push_back(packet);
}

mathematica::packets::packet< mathematica::packet_RETURN >::ptr mathematica::history::last() const{
    return _records.back();
}

mathematica::history::string_list_type mathematica::history::messages() const{
    mathematica::history::string_list_type msgs;    
    mathematica::packets::Return::range_type range = last()->related(mathematica::packet_MESSAGE);
    for(auto i=range.first; i != range.second; ++i){
        mathematica::packet::ptr packet = i->second;
        mathematica::packets::Message::ptr message_packet = boost::dynamic_pointer_cast<mathematica::packets::Message>(packet);
        std::string message = message_packet->message();
        msgs.push_back(message);
    }
    return msgs;
}

std::string mathematica::history::prompt() const{
    mathematica::packets::Return::range_type range = last()->related(mathematica::packet_INPUTNAME);
    if(!std::distance(range.first, range.second)){
        return std::string();
    }
    mathematica::packet::ptr packet = range.first->second;
    mathematica::packets::InputName::ptr input_prompt_packet = boost::dynamic_pointer_cast<mathematica::packets::InputName>(packet);
    return input_prompt_packet->prompt();
}

mathematica::history::string_list_type mathematica::history::texts() const{
    mathematica::history::string_list_type texts;    
    mathematica::packets::Return::range_type range = last()->related(mathematica::packet_TEXT);
    for(auto i=range.first; i != range.second; ++i){
        mathematica::packet::ptr packet = i->second;
        mathematica::packets::Text::ptr text_packet = boost::dynamic_pointer_cast<mathematica::packets::Text>(packet);
        std::string message = text_packet->text();
        texts.push_back(message);
    }
    return texts;
}

mathematica::value mathematica::history::out(int n) const{
    packets::Return::ptr packet = _records[n];
    return packet->token();
}

mathematica::value mathematica::history::operator[](int n) const{
    return out(n);
}

mathematica::history::packets_type::size_type mathematica::history::count() const{
    return _records.size();
}

