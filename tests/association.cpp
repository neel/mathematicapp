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

struct point{
    int x;
    int y;
    std::string name;
    double weight;
};

struct point2{
    std::pair<int, int> location;
    std::string name;
    double weight;
};

namespace mathematica{
    template <>
    struct association<point>: dictionary<association<point>, point, int, int, std::string, double>{
        static auto detail(property<0>){return std::make_pair("x",          &point::x);}
        static auto detail(property<1>){return std::make_pair("y",          &point::y);}
        static auto detail(property<2>){return std::make_pair("name",       &point::name);}
        static auto detail(property<3>){return std::make_pair("weight",     &point::weight);}
    };
}

namespace mathematica{
    typedef std::pair<int, int> pair_ii;
    template <>
    struct association<pair_ii>: dictionary<association<pair_ii>, pair_ii, int, int>{
        static auto detail(property<0>){return std::make_pair("first",   &pair_ii::first);}
        static auto detail(property<1>){return std::make_pair("second",  &pair_ii::second);}
    };
}

namespace mathematica{
    template <>
    struct association<point2>: dictionary<association<point2>, point2, std::pair<int, int>, std::string, double>{
        static auto detail(property<0>){return std::make_pair("location",   &point2::location);}
        static auto detail(property<1>){return std::make_pair("name",       &point2::name);}
        static auto detail(property<2>){return std::make_pair("weight",     &point2::weight);}
    };
}

using namespace mathematica;


BOOST_AUTO_TEST_SUITE(assoc)

BOOST_AUTO_TEST_CASE(association_struct){
    connector shell;
    BOOST_CHECK(shell.connected());
    {
        value result;
        association<point> pm;
        point pt;
        pt.x = 0;
        pt.y = 0;
        pt.name = "hi";
        pt.weight = 0.0f;
        mathematica::m assoc = pm.serialize(pt);
        shell << assoc;
        shell >> result;
        std::cout << result << std::endl;
    }
    {
        value result;
        association<point2> pm;
        point2 pt;
        pt.location = std::make_pair(0, 0);
        pt.name = "hi";
        pt.weight = 0.0f;
        mathematica::m assoc = pm.serialize(pt);
        shell << assoc;
        shell >> result;
        std::cout << result << std::endl;
    }
}

BOOST_AUTO_TEST_SUITE_END()
