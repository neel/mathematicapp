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

MATHEMATICA_DECLARE(Integrate)

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
                
        shell.save("pluto") << Integrate(Sin(x), x);
        std::cout << shell.last_key();
    }
}

BOOST_AUTO_TEST_SUITE_END()
