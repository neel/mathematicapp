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

#include "mathematica++/m.h"
#include <boost/ref.hpp>

mathematica::m::m(const std::string& name): _name(name), _length(0){}
mathematica::m::m(const mathematica::m& other): _name(other._name), _queue(other._queue), _length(other._length){}


mathematica::m& mathematica::m::operator()(){
    _queue.push_front(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::function, _1, _name, _length)));
    return *this;
}

mathematica::m& mathematica::m::invoke(mathematica::driver::ws::connection& conn){
    _queue.push_front(detail::make_deyaled_call(boost::bind(&mathematica::driver::ws::impl::function, boost::ref(conn), "EvaluatePacket", 1)));
    while(!_queue.empty()){
        _queue.front()->invoke(conn);
        _queue.pop_front();
    }
    return *this;
}

