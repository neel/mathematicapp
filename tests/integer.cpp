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
#define BOOST_TEST_MODULE "mathematica++ Unit Test (Module::Integer)"
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

using namespace mathematica;

MATHEMATICA_DECLARE(Det)
MATHEMATICA_DECLARE(Dot)
MATHEMATICA_DECLARE(Mod)

BOOST_AUTO_TEST_SUITE(integer)

BOOST_AUTO_TEST_CASE(integer_size){
    connector shell;
    BOOST_CHECK(shell.connected());
    {
        value result;
        shell << Power(2, 68);
        shell >> result;
        
        std::string res_str = *result;
        res_str.erase(0, 1);
        
        // std::cout << *result << " " << res_str << std::endl;
        
        BOOST_CHECK(res_str == "295147905179352825856");
    }
    {
        value res_intermediate;
        value res_final;
        
        shell << List(Power(2, 61), Power(2, 62), Power(2, 63), Power(2, 64), Power(2, 65));
        shell >> res_intermediate;
        shell << Total(res_intermediate);
        shell >> res_final;
        
        std::string res_nstr = *res_final;
        res_nstr.erase(0, 1);
        
        // std::cout << res_intermediate << std::endl;
        // std::cout << res_final << std::endl;
        
        BOOST_CHECK(res_nstr == "71481133285624512512");        
    }
    {
        mathematica::symbol i("i");
        mathematica::symbol j("j");
        
        value res_mata;
        value res_matb;
        value res_matc;
        value res_det;

        shell << Table(Mod(i+j, 2), List(i, 1, 2), List(j, 1, 2));
        shell >> res_mata;
        shell << Table(Mod(i+j, 3), List(i, 1, 2), List(j, 1, 2));
        shell >> res_matb;
        shell << Dot(res_mata, res_matb);
        shell >> res_matc;
        shell << Det(res_matc);
        shell >> res_det;
        
        BOOST_CHECK(*res_det == -2);
    }
}

BOOST_AUTO_TEST_SUITE_END()

