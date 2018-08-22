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

#include <cmath>
#include <iostream>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "mathematica++ Unit Test (Module::Storage)"
#include <boost/test/unit_test.hpp>
#include <functional>
#include <numeric>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include "mathematica++/m.h"
#include "mathematica++/io.h"
#include "mathematica++/connector.h"
#include "mathematica++/tokens.h"
#include "mathematica++/variant.h"
#include "mathematica++/declares.h"
#include "mathematica++/tokens.h"
#include "mathematica++/accessor.h"
#include "mathematica++/variant.h"
#include "mathematica++/operators.h"

MATHEMATICA_DECLARE(Simplify)
MATHEMATICA_DECLARE(Integrate)
// MATHEMATICA_DECLARE(Power)

using namespace mathematica;

BOOST_AUTO_TEST_SUITE(storage)

BOOST_AUTO_TEST_CASE(simple){
    connector shell;
    BOOST_CHECK(shell.connected());
    {
        symbol x("x");
        value result;
        
        shell.save() << Integrate(Sin(x), x);
        shell << shell.last() + Cos(x);
        shell >> result;
        
        BOOST_CHECK(*result == 0);
    }
    
    {
        symbol x("x");
        value result;
                
        shell.save("pluto") << Integrate(Sin(x), x);
        BOOST_CHECK(shell.last_key() == "pluto");
        shell << shell.ref("pluto")+Cos(x);
        shell >> result;
        BOOST_CHECK(*result == 0);
    }
    
    {
        symbol x("x");
        value result;
        
        std::string left_key, right_key;
        
        shell.save() << Integrate(2*x, x);
        left_key = shell.last_key();
        shell.save() << Integrate(-2/Power(x,3), x);
        right_key = shell.last_key();
        shell << shell.ref(left_key)*shell.ref(right_key);
        shell >> result;
        BOOST_CHECK(*result == 1);
    }
    
    {
//         value result;
//         std::string key;
//         
//         shell.begin();
//         shell.save() << Sin(0);
//         key = shell.last_key();
//         shell << shell.ref(key);
//         shell >> result;
//         BOOST_CHECK(*result == 0);
//         shell.end(true);
//         shell << shell.ref(key);
//         shell >> result;
//         std::cout << result << std::endl;
//         symbol x("x"), y("y");
//         mathematica::m expr = 2*Sin(x)+3*Cos(y);
    }
}

BOOST_AUTO_TEST_SUITE_END()
