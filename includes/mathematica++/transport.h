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

#ifndef MATHEMATICAPP_TRANSPORT_H
#define MATHEMATICAPP_TRANSPORT_H

#include <mathematica++/connection.h>
#include <WolframLibrary.h>
#include <mathematica++/compatibility.h>
#include <mathematica++/connector.h>
#include <mathematica++/io.h>
#include <mathematica++/m.h>
#include <mathematica++/declares.h>
#include <boost/utility/enable_if.hpp>
#include <boost/variant.hpp>
#include <boost/array.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_complex.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/multi_array.hpp>
#include <boost/array.hpp>
#include <cstring>
#include <complex>
#include <stdexcept>
#include <type_traits>
#include <mathematica++/exceptions.h>

namespace mathematica{
    
namespace internal{

template <typename T>
struct is_vector{
    static bool const value = false;
};
template <typename T>
struct is_vector<std::vector<T>>{
    static bool const value = true;
};

template <typename T>
struct value_type{
    typedef T type;
};

template <typename T>
struct value_type<std::vector<T>>: value_type<T>{};

template <typename T>
struct vector_rank{
    enum{rank = 0};
    
    static void dimensions(const T& scalar, mint* dim_it){}
};

template<typename T>
struct vector_rank<std::vector<T>>{
    enum{rank = vector_rank<T>::rank +1};
    typedef boost::array<mint, rank> dimensions_type;
    
    static void dimensions(const std::vector<T>& vector, mint* dim_it){
        *dim_it = vector.size();
        vector_rank<T>::dimensions(vector[0], dim_it+1);
    }
};

template <typename T, typename U>
struct vec_clone{
    typedef U type;
};

template <typename T, typename U>
struct vec_clone<std::vector<T>, U>{
    typedef typename std::vector<typename vec_clone<T, U>::type> type;
};

template <typename T, typename E=void>
struct mtensor_value_typeid;

template <typename T>
struct vector_flatten{
    typedef T value_type;
    typedef std::vector<typename mtensor_value_typeid<value_type>::mtype> flattened_type;
    
    static void flatten(const T& scalar, typename flattened_type::iterator& flattened_it){
        *flattened_it = mtensor_value_typeid<T>::to_mtype(scalar);
        ++flattened_it;
    }
};

template <typename T>
struct vector_flatten<std::vector<T>>{
    typedef std::vector<T> vector_type;
    typedef typename value_type<T>::type value_type;
    typedef std::vector<value_type> flattened_type;
    
    static void flatten(const std::vector<T>& vector, typename flattened_type::iterator& flattened_it){
        for(typename vector_type::const_iterator it = vector.begin(); it != vector.end(); ++it){
            const T& one_dim_down = *it;
            vector_flatten<T>::flatten(one_dim_down, flattened_it);
        }
    }
};

template <typename T>
struct reserver{
    static void reserve(T& scalar, std::vector<int>::const_iterator dim){}
    template <typename M>
    static void populate(T& scalar, typename std::vector<M>::const_iterator& it){
        T val = mtensor_value_typeid<T>::to_utype(*it++);
        scalar = val;
    }
};

template <typename T>
struct reserver<std::vector<T>>{
    static void reserve(std::vector<T>& vec, std::vector<int>::const_iterator dim){
        vec.resize(*dim);
        std::vector<int>::const_iterator ndim = dim+1;
        for(int i = 0; i < *dim; ++i){
            reserver<T>::reserve(vec[i], ndim);
        }
    }
    template <typename M>
    static void populate(std::vector<T>& vec, typename std::vector<M>::const_iterator& it){
        for(typename std::vector<T>::iterator i = vec.begin(); i != vec.end(); ++i){
            reserver<T>::template populate<M>(*i, it);
        }
    }
};

template <typename T, typename M>
typename boost::enable_if<is_vector<T>, T>::type& tensor_to_vector(T& tensor, const std::vector<int> dims, const std::vector<M>& data){
    typedef T tensor_type;
    typedef typename value_type<T>::type value_type;
    typedef typename std::vector<M>::const_iterator data_iterator;
    
    reserver<T>::reserve(tensor, dims.begin());
    data_iterator it = data.cbegin();
    reserver<T>::template populate<M>(tensor, it);
    return tensor;
}

template <typename T>
void dimensions(const T& vec, typename vector_rank<T>::dimensions_type& dims){
    typedef typename vector_rank<T>::dimensions_type dimensions_type;
    typedef typename dimensions_type::iterator iterator;
    
    iterator it = dims.begin();
    vector_rank<T>::dimensions(vec, it);
}

template <typename T>
typename vector_flatten<T>::flattened_type flatten(const T& vec){
    typedef typename vector_rank<T>::dimensions_type dimensions_type;
    typename vector_flatten<T>::flattened_type flattened;
    
    dimensions_type dims;
    dimensions(vec, dims);
    
    int flattened_length = std::accumulate(dims.begin(), dims.end(), 1, std::multiplies<int>());
    flattened.resize(flattened_length);
    
    typename vector_flatten<T>::flattened_type::iterator it = flattened.begin();
    vector_flatten<T>::flatten(vec, it);
    
    return flattened;
}

template <typename T>
struct mtensor_value_typeid<T, typename boost::enable_if<boost::is_integral<T>>::type >{
    typedef mint mtype;
    
    static int const id = MType_Integer;
    static constexpr const char* const name = "Integer";
    
    /**
     * flattened mint data of the tensor
     */
    static mint* data(WolframLibraryData data, MTensor tensor){
        return data->MTensor_getIntegerData(tensor);
    }
    static T to_utype(mint mdata){
        return static_cast<T>(mdata);
    }
    static mint to_mtype(T data){
        return static_cast<mint>(data);
    }
};
template <typename T>
struct mtensor_value_typeid<T, typename boost::enable_if<boost::is_floating_point<T>>::type >{
    typedef mreal mtype;
    
    static int const id = MType_Real;
    static constexpr const char* const name = "Real";
    
    /**
     * flattened mreal data of the tensor
     */
    static double* data(WolframLibraryData data, MTensor tensor){
        return data->MTensor_getRealData(tensor);
    }
    static T to_utype(mreal mdata){
        return static_cast<T>(mdata);
    }
    static mreal to_mtype(T data){
        return static_cast<mreal>(data);
    }
};
template <>
struct mtensor_value_typeid<mcomplex>{
    typedef mcomplex mtype;
    
    static int const id = MType_Real;
    static constexpr const char* const name = "Complex";
    
    /**
     * flattened mcomplex data of the tensor
     */
    static mcomplex* data(WolframLibraryData data, MTensor tensor){
        return data->MTensor_getComplexData(tensor);
    }
    static mcomplex to_utype(mcomplex mdata){
        return mdata;
    }
    static mcomplex to_mtype(mcomplex data){
        return data;
    }
};

template <typename T>
struct mtensor_value_typeid<std::complex<T>>{
    typedef mcomplex mtype;
    
    static int const id = MType_Real;
    static constexpr const char* const name = "Complex";
    
    /**
     * flattened mcomplex data of the tensor
     */
    static mcomplex* data(WolframLibraryData data, MTensor tensor){
        return data->MTensor_getComplexData(tensor);
    }
    static std::complex<T> to_utype(mcomplex mdata){
        return std::complex<T>(static_cast<T>(mcreal(mdata)), static_cast<T>(mcimag(mdata)));
    }
    static mcomplex to_mtype(const std::complex<T> data){
        mcomplex cplx;
        mcreal(cplx) = data.re();
        mcimag(cplx) = data.imag();
        return cplx;
    }
};

}



struct mtensor_adapter{
    WolframLibraryData _data;
   
    mtensor_adapter(WolframLibraryData data);

    template <typename T>
    typename boost::enable_if<internal::is_vector<T>, T>::type vectorify(MTensor tens) const{
        T tensor;// the vector we are supposed to populate and return
        
        int trank = _data->MTensor_getRank(tens);
//         int type = _data->MTensor_getType(tens);
        
        //{ dimensions
        const mint* dims = _data->MTensor_getDimensions(tens);
        std::vector<int> dimensions;
        std::copy(dims, dims+trank, std::back_inserter(dimensions));
        //}
        
        //{ flattened data
        typedef typename internal::value_type<T>::type vtype;
        typedef typename internal::mtensor_value_typeid<vtype>::mtype mtype;
        mtype* raw = internal::mtensor_value_typeid<vtype>::data(_data, tens);
        if(!raw){
            throw library::exceptions::library_error(LIBRARY_TYPE_ERROR, messages::tensortype() % std::string(internal::mtensor_value_typeid<vtype>::name), "Failed to vectorify tensor");
        }
        mint len = _data->MTensor_getFlattenedLength(tens);
        std::vector<mtype> raw_vec;
        std::copy(raw, raw+len, std::back_inserter(raw_vec));
        // raw_vec.assign(raw, raw+len-1);
        //}
        
        //{ check rank compatiability
        int rank = internal::vector_rank<T>::rank;
        if(rank != trank){
            throw library::exceptions::library_error(LIBRARY_RANK_ERROR, messages::rankerror() % trank % rank, "Failed to vectorify tensor");
        }
        //}
        
        //{ build the vector from the tensor 
        internal::tensor_to_vector(tensor, dimensions, raw_vec);
        //} 
        
        return tensor;
    }
    
    template <typename T>
    MTensor tensorify(const std::vector<T>& vec){
        MTensor tensor;
        
        typedef std::vector<T> vector_type;
        typedef typename internal::vector_rank<vector_type>::dimensions_type dimensions_type;
        typedef typename internal::value_type<vector_type>::type vtype;
        typedef typename internal::mtensor_value_typeid<vtype>::mtype mtype;
        
        int mtype_id = internal::mtensor_value_typeid<vtype>::id;
        int rank = internal::vector_rank<vector_type>::rank;
        
        dimensions_type dims;
        internal::dimensions(vec, dims);
        
        _data->MTensor_new(mtype_id, rank, dims.data(), &tensor);
        
        mtype* raw = internal::mtensor_value_typeid<vtype>::data(_data, tensor);
        std::vector<mtype> data = internal::flatten(vec);
        
        for(typename std::vector<mtype>::const_iterator it = data.begin(); it != data.end(); ++it){
            *raw = *it;
            raw++;
        }
        
        return tensor;
    }

};
    
/**
 * argument adapter only handles integer, real, complex types and string
 */
struct argument_adapter{
    WolframLibraryData _data;
    MArgument _source;
    
    argument_adapter(WolframLibraryData data, MArgument src): _data(data), _source(src){}
    
    template <typename T>
    typename boost::enable_if<boost::is_integral<T>, argument_adapter&>::type operator=(T val){
        MArgument_getInteger(_source) = val;
        return *this;
    }
    template <typename T>
    typename boost::enable_if<boost::is_floating_point<T>, argument_adapter&>::type operator=(T val){
        MArgument_getReal(_source) = val;
        return *this;
    }
    argument_adapter& operator=(char* val){
        MArgument_getUTF8String(_source) = val;
        return *this;
    }
    template <typename T>
    argument_adapter& operator=(const std::complex<T> val){
        mcomplex& cmplx = MArgument_getComplex(_source);
        mcreal(cmplx) = val.real();
        mcimag(cmplx) = val.imag();
        return *this;
    }
    std::string convert() const{
        const char* str = MArgument_getUTF8String(_source);
        int len = strlen(str);
        return std::string(str, len);
    }
    template <typename T>
    typename boost::enable_if<boost::is_complex<T>, T>::type convert() const{
        mcomplex cmplx = MArgument_getComplex(_source);
        return T(mcreal(cmplx), mcimag(cmplx));
    }
    template <typename T>
    typename boost::enable_if<boost::is_integral<T>, T>::type convert() const{
        return MArgument_getInteger(_source);
    }
    template <typename T>
    typename boost::enable_if<boost::is_floating_point<T>, T>::type convert() const{
        return MArgument_getReal(_source);
    }
    template <typename T>
    typename boost::enable_if<internal::is_vector<T>, T>::type convert() const{
        MTensor tens = MArgument_getMTensor(_source);
        mtensor_adapter adapter(_data);
        return adapter.vectorify<T>(tens);
    }
    template <typename T>
    argument_adapter& operator=(const std::vector<T>& val){
        mtensor_adapter adapter(_data);
        MTensor tensor = adapter.tensorify(val);
        MArgument_setMTensor(_source, tensor);
        return *this;
    }
    template <typename T>
    operator T() const{
        T val = convert<T>();
        return val;
    }
};


struct basic_transport{
    typedef WMK_LINK link_type;
    
    WMK_LINK _link;
    boost::shared_ptr<driver::io::connection> _connection;
    mathematica::connector _shell;
    
    basic_transport(link_type link);
    template <typename T>
    void send(const T& expr){
        _shell << expr;
    }
    template <typename T>
    void recv(T& val){
        _shell >> val;
    }
    mathematica::value recv();
    link_type link() const{return _link;}
};

struct transport{
    typedef basic_transport::link_type link_type;
    
    std::string _libname;
    WolframLibraryData _data;
    basic_transport _io;
    
    transport(WolframLibraryData data, std::string lib_name="");
    template <typename T>
    void send(const T& expr){
        _io.send(mathematica::m("EvaluatePacket")(expr));
//         _io.send(expr);
        _data->processWSLINK(_io.link());
    }
    template <typename T>
    void recv(T& val){
        _io.recv(val);
    }
    void ignore(){
        _io._shell.ignore();
        _data->processWSLINK(_io.link());
    }
    
    void set_name(const std::string& name);
    std::string name() const;
    virtual int pass(bool abort=true);
};

struct initializer: transport{
    std::string _library_name;
    
    initializer(WolframLibraryData data, std::string library_name="");
    initializer& declare(const mathematica::user_message& msg);
};
    
struct wtransport: transport{
    basic_transport _control;
    boost::shared_ptr<mathematica::tokens::function> _input;
    
    wtransport(WolframLibraryData data, link_type link, std::string lib_name="");
    /**
     * return a value from the library function
     */
    template <typename T>
    int operator()(const T& val){
        _control.send(val);
        return LIBRARY_NO_ERROR;
    }
    template <typename T>
    int operator=(const T& val){
        _control.send(val);
        return LIBRARY_NO_ERROR;
    }
    template <typename T>
    int ret(T val){
        *this = val;
        return LIBRARY_NO_ERROR;
    }
    boost::shared_ptr<mathematica::tokens::function> input() const{return _input;}
    template <typename T>
    T input() const{
        mathematica::value val = input();
        T ret = cast<T>(val);
        return ret;
    }
    virtual int pass(bool abort=true);
};

namespace internal{
template <typename T, int N>
struct argument_to_tuple{
    typedef T tuple_type;
    
    template <typename U>
    static void convert(T& tuple, U& shell){
        typedef typename boost::tuples::element<N, T>::type element_type;
        element_type elem = shell.arg(N).template convert<element_type>();
        boost::get<N>(tuple) = elem;
        argument_to_tuple<T, N-1>::convert(tuple, shell);
    }
};

template <typename T>
struct argument_to_tuple<T, -1>{
    typedef T tuple_type;
    
    template <typename U>
    static void convert(T& tuple, U& shell){}
};

}

struct mtransport: transport{
    int _argc;
    MArgument* _argv;
    MArgument _res;
    std::vector<argument_adapter*> _arguments;
    argument_adapter _result;
    
    mtransport(WolframLibraryData data, int argc, MArgument* argv, MArgument res, std::string lib_name="");
    ~mtransport();
    const argument_adapter& arg(std::size_t index) const;
    argument_adapter& res();
    std::size_t nargs() const;
    
    template <typename... T>
    boost::tuple<T...> as() const{
        typedef boost::tuple<T...> tuple_type;
        int input_args = nargs();
        int requested_args = boost::tuples::length<tuple_type>::value;
        if(requested_args != input_args){
            throw library::exceptions::library_error(LIBRARY_TYPE_ERROR, messages::argx() % input_args % requested_args, "Failed to parse arguments");
        }
        tuple_type tuple;
        internal::argument_to_tuple<tuple_type, boost::tuples::length<tuple_type>::value-1>::convert(tuple, *this);
        return tuple;
    }
    template <typename... T>
    operator boost::tuple<T...>() const{
        return as<T...>();
    }
    template <typename T>
    int ret(T val){
        res() = val;
        return LIBRARY_NO_ERROR;
    }
    template <typename T>
    int operator()(T val){
        return ret<T>(val);
    }
    template <typename T>
    int operator=(T val){
        return ret<T>(val);
    }
    virtual int pass(bool abort=true);
};

template <typename T>
transport& operator<<(transport& stream, const T& expr){
    stream.send(expr);
    return stream;
}

template <typename T>
transport& operator>>(transport& stream, T& val){
    stream.recv(val);
    return stream;
}

mathematica::initializer& operator<<(mathematica::initializer& shell, const mathematica::user_message& msg);
mathematica::transport& operator<<(mathematica::transport& shell, const mathematica::basic_message& msg);
mathematica::transport& operator<<(mathematica::transport& shell, const mathematica::message& msg);
}

#endif // TRANSPORT_H
