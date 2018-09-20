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

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "mathematica++ Unit Test (Module::Association)"
#include <boost/test/unit_test.hpp>
#include <string>
#include <cmath>
#include <complex>
#include <map>
#include <vector>
#include <iostream>
#include <boost/format.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include "boost/tuple/tuple.hpp"
#include "mathematica++/connector.h"
#include "mathematica++/m.h"
#include "mathematica++/io.h"
#include "mathematica++/declares.h"
#include "mathematica++/operators.h"
#include "mathematica++/rules.h"
#include "mathematica++/association.h"
// #include "mathematica++/serialization.h"

namespace mathematica{
    template <typename T>
    mathematica::m serialize(const T& obj){
        association<T> serializer;
        return serializer.serialize(obj);
    }
}

struct point{
    int x;
    int y;
    std::string name;
    double weight;
    std::complex<double> cw;
    
    point(): x(0), y(0), name(""), weight(0.0f), cw(0){}
    point(int x_, int y_, const std::string& name_, double weight_, const std::complex<double>& cw_): x(x_), y(y_), name(name_), weight(weight_), cw(cw_){}
};

struct point2{
    std::pair<int, int> location;
    std::string name;
    double weight;
    
    point2(): location(std::make_pair(0, 0)), name(""), weight(0.0f){}
    point2(std::pair<int, int> loc, const std::string& name_, double w): location(loc), name(name_), weight(w){}
};

MATHEMATICA_ASSOCIATE(point, int, int, std::string, double, std::complex<double>){
    MATHEMATICA_PROPERTY(0, x)
    MATHEMATICA_PROPERTY(1, y)
    MATHEMATICA_PROPERTY(2, name)
    MATHEMATICA_PROPERTY(3, weight)
    MATHEMATICA_PROPERTY(4, cw)
};

// namespace mathematica{
//     template <typename U, typename V>
//     struct association<std::pair<U, V>>: dictionary<association<std::pair<U, V>>, std::pair<U, V>, U, V>{
//         static auto detail(property<0>){return std::make_pair("first",   &std::pair<U, V>::first);}
//         static auto detail(property<1>){return std::make_pair("second",  &std::pair<U, V>::second);}
//     };
// }

MATHEMATICA_ASSOCIATE(point2, std::pair<int, int>, std::string, double){
    MATHEMATICA_PROPERTY(0, location)
    MATHEMATICA_PROPERTY(1, name)
    MATHEMATICA_PROPERTY(2, weight)
};

MATHEMATICA_DECLARE(Part)

using namespace mathematica;

BOOST_AUTO_TEST_SUITE(assoc)

BOOST_AUTO_TEST_CASE(association_struct){
    connector shell;
    BOOST_CHECK(shell.connected());
    {
        value result;
        point pt;
        mathematica::m assoc = serialize(pt);
        shell << assoc;
        shell >> result;
        std::cout << result << std::endl;
    }
    {
//         typedef std::vector<point2> pair_type;
        value result;
        point pt_0(3, 3, "Jupiter", 3.0f, std::complex<double>(2, 3));
        point2 pt_1(std::make_pair(1, 1), "Hallo", 1.0f);
        point2 pt_2(std::make_pair(2, 2), "World", 2.0f);
        {
            shell << Part(List(pt_0, pt_1, pt_2), 1);
            shell >> result;
            std::cout << result << std::endl;
            point pt = cast<point>(result);
            std::cout << pt.x << " " << pt.y << " " << pt.name << " " << pt.weight << " " << pt.cw << std::endl;
        }
        {
            shell << Part(List(pt_0, pt_1, pt_2), 2);
            shell >> result;
            std::cout << result << std::endl;
            point2 pt = cast<point2>(result);
            std::cout << pt.location.first << " " << pt.location.second << " " << pt.name << " " << pt.weight << std::endl;
        }
        {
            shell << Part(List(pt_0, pt_1, pt_2), 3);
            shell >> result;
            std::cout << result << std::endl;
            point2 pt = cast<point2>(result);
            std::cout << pt.location.first << " " << pt.location.second << " " << pt.name << " " << pt.weight << std::endl;
        }
        
//         pair_type pair = cast<pair_type>(result);
        
//         association<point2>::tuple_type pt = cast<association<point2>::tuple_type>(result);
        
//         std::cout << "output: " << pair[0].name << " " << pair[1].name << std::endl;
//         std::cout << boost::get<1>(pt) << " " << boost::get<2>(pt) << std::endl;
    }
}

BOOST_AUTO_TEST_SUITE_END()
