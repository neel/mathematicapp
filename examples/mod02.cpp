#include <fstream>
#include <mathematica++/mathematica++.h>
#include <mathematica++/transport.h>
#include <mathematica++/defs.h>
#include <mathematica++/library.h>

using namespace mathematica;

MATHEMATICA_DECLARE(Complex);
MATHEMATICA_DECLARE(GeoPosition);

template <typename T>
struct point_2d{
    T x;
    T y;
};

template <typename T>
std::ostream& operator<<(std::ostream& stream, const point_2d<T>& pt){
    stream << "(" << pt.x << " " << pt.y << ")"; 
    return stream;
}

namespace mathematica{
template <typename T>
struct association<point_2d<T>>: mathematica::typemap<association<point_2d<T>>, point_2d<T>, boost::tuple<boost::tuple<T, T>>>{
    mathematica::m operator()(const point_2d<T>& obj){
        return GeoPosition(List(obj.y, obj.x));
    }
    point_2d<T> operator()(const boost::tuple<boost::tuple<T, T>>& captured){
        boost::tuple<double, double> pt_data = boost::get<0>(captured);
        point_2d<T> pt;
        std::clog << pt << std::endl;
        pt.y = boost::get<0>(pt_data);
        pt.x = boost::get<1>(pt_data);
        return pt;
    }
};
}

double some_function_impl_geo(mathematica::transport& shell, point_2d<double> p1, point_2d<double> p2){
    return p1.x+p2.y;
}

double some_function_impl_complex(std::complex<double> p1, std::complex<double> p2){
    return p1.real()+p2.imag();;
}
int some_function_impl_binary(double x, double y){
    return 10;
}
mathematica::m some_function_impl_unary(double x){
    return Complex(x+2, x);
}

EXTERN_C DLLEXPORT mint WolframLibrary_getVersion(){return WolframLibraryVersion;}
EXTERN_C DLLEXPORT mint WolframLibrary_initialize(WolframLibraryData libData){return 0;}
EXTERN_C DLLEXPORT void WolframLibrary_uninitialize(WolframLibraryData libData){return;}

EXTERN_C DLLEXPORT int SomeFunctionX(WolframLibraryData libData, WMK_LINK native_link){
    mathematica::wtransport shell(libData, native_link);
    mathematica::resolver resolver(shell);

    try{
        resolver, overload(&some_function_impl_geo, shell) = {"GeoPosition", "GeoPosition"}
                , overload(&some_function_impl_complex) = {"Complex", "Complex"}
                , overload(&some_function_impl_binary)
                , overload(&some_function_impl_unary);
    }catch(const mathematica::library::error& err){
        return err.code();
    }

    if(resolver.resolved()){
        return LIBRARY_NO_ERROR;
    }
    return LIBRARY_TYPE_ERROR;
}


// SomeFunctionX = LibraryFunctionLoad["/home/neel/Projects/mathematicapp/build/examples/libmod02.so", "SomeFunctionX", LinkObject, LinkObject]
// SomeFunctionX[1]
// SomeFunctionX[1, 2]
// SomeFunctionX[2 + 2 I, 4 + 5 I]
// SomeFunctionX[GeoPosition[{1, 2}], GeoPosition[{3, 4}]]
