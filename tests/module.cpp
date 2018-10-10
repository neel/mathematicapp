#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "mathematica++ Unit Test (Module::Module)"

#include <complex>
#include <boost/test/unit_test.hpp>
#include <mathematica++/mathematica++.h>

MATHEMATICA_DECLARE(SomeFunctionWX)
MATHEMATICA_DECLARE(GeoPosition)

using namespace mathematica;

BOOST_AUTO_TEST_SUITE(module)

BOOST_AUTO_TEST_CASE(module_library){
    connector shell;
    BOOST_CHECK(shell.connected());
    shell.import("../../examples/mod02.m");
    
    {
        std::complex<double> res;
        shell << SomeFunctionWX(1);
        shell >> res;
        
        BOOST_CHECK(res == std::complex<double>(3, 1));
    }
    {
        double res;
        shell << SomeFunctionWX(1.0f, 20.0f);
        shell >> res;
        
        BOOST_CHECK(res == 10);
    }
    {
        std::complex<double> u(1.0f, 2.0f);
        std::complex<double> v(3.0f, 4.0f);
        double res;
        shell << SomeFunctionWX(u, v);
        shell >> res;
        
        BOOST_CHECK(res == u.real()+v.imag());
    }
}

BOOST_AUTO_TEST_SUITE_END()
