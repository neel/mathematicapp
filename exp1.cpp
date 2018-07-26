#include <iostream>
#include <type_traits>

struct A{};
struct B{};

struct M{
    int _x;
    M(int x): _x(x){}
    operator int(){return _x;}
};

template <typename T>
struct is_operand{};
template <>
struct is_operand<A>{typedef M result_type;};
template <>
struct is_operand<B>{typedef M result_type;};

template <typename U, typename V> struct operation_result: is_operand<U>, is_operand<V>{};
template <typename U> struct operation_result<U, U>: is_operand<U>{};
template <> struct operation_result<A, B>{typedef M result_type;};
template <> struct operation_result<B, A>{typedef M result_type;};

template <typename U, typename V>
typename operation_result<U, V>::result_type operator+(U u, V v){
    std::cout << "Hallo" << std::endl;
    return M(42);
}

int main(){
    
    A a;
    B b;
    
    std::cout << 1 + 1 << std::endl;
    std::cout << a + 1 << std::endl;
    std::cout << 1 + a << std::endl;
    std::cout << b + 1 << std::endl;
    std::cout << 1 + b << std::endl;
    std::cout << a + a << std::endl;
    std::cout << b + b << std::endl;
    std::cout << a + b << std::endl;
    std::cout << b + a << std::endl;
    
    return 0;
};
