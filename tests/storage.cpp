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
