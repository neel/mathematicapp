# Mathematica++
## A C++ Library that talks Mathematica

Dot product and Determinant calculation in Mathematica Language

```Mathematica
mata = Table[Mod[i + j, 2], {i, 1, 2}, {j, 1, 2}];
matb = Table[Mod[i + j, 3], {i, 1, 2}, {j, 1, 2}];
matc = Dot[mata, matb];
matd = Det[matc];

```
Equivalent C++ code with Mathematica++

```cpp
mathematica::m mata = Table(Mod(i + j, 2), List(i, 1, 2), List(j, 1, 2));
mathematica::m matb = Table(Mod(i + j, 3), List(i, 1, 2), List(j, 1, 2)];
mathematica::m matc = Dot(mata, matb);
mathematica::m matd = Det(matc);

// Execute mathematica constructs and fetch the response
shell << matd;
shell >> determinant;

// determinant can be converted to C++ machine sized types
std::cout << determinant << std::endl; // Prints -2
```
* The Mathematica functions declared with `MATHEMATICA_DECLARE` outside any function (may be inside a header) e.g. `MATHEMATICA_DECLARE(Table)`, `MATHEMATICA_DECLARE(Det)`
* A symbols created using `mathematica::symbol` e.g. `mathematica::symbol i("i")`, `mathematica::symbol j("j")`
* `mathematica::m` creates a mathematica construct
* `mathematica::value` holds the value returnd from mathematica

```cpp
// Declare Mathematica functions 
MATHEMATICA_DECLARE(Table)
MATHEMATICA_DECLARE(Mod)
MATHEMATICA_DECLARE(Dot)
MATHEMATICA_DECLARE(Det)

// connect to mathematica
int argc = 4;
char* argv[] = {(char*)"-linkname", (char*)"math -mathlink", (char*)0x0};
connector shell(argc, argv);

// Declare symbols
mathematica::symbol i("i");
mathematica::symbol j("j");

// declare variable to contain mathematica output
mathematica::value determinant;

// create mathematica constructs
mathematica::m mata = Table(Mod(i + j, 2), List(i, 1, 2), List(j, 1, 2));
mathematica::m matb = Table(Mod(i + j, 3), List(i, 1, 2), List(j, 1, 2)];
mathematica::m matc = Dot(mata, matb);
mathematica::m matd = Det(matc);

// Execute mathematica constructs and fetch the response
shell << matd;
shell >> determinant;

// determinant can be converted to C++ machine sized types
std::cout << determinant << std::endl; // Prints -2
```
## Simple Example of adding all numbers in a list

```cpp
using namespace mathematica;
symbol i("i"); // declare mathematica symbol i
value result_sum; // declare the variable to hold the result

shell << Total(Table(i, List(i, 1, 10))); // In Mathematica Total[Table[i, {i, 1, 10}]]
shell >> result_sum;
````
`result_sum` is the result object that can be converted to `int`, `double`, `std::string` and streamed to `std::ostream`. 
```cpp
std::cout << result_sum << std::endl; // Prints 55
std::cout << result_sum->stringify() << std::endl; // Prints 55
int sum1 = *result_sum; // auto coercion through type operator overloading for scaler types
int sum2 = cast<int>(result_sum);
double sum3 = *result_sum;
double sum4 = cast<double>(result_sum); 
std::cout << sum1 << " " << sum2 << " " << sum3 << " " << sum4 << std::endl; // Prints 55 55 55 55
```

## Fetching composite results (in STL containers)
`mathematica::value` can hold composite values returned from mathematica like this example of `List`
```cpp
symbol i("i"); // declare mathematica symbol i
value result_list; // declare the variable to hold the result

shell << Table(i, List(i, 1, 10)); // In Mathematica Table[i, {i, 1, 10}]
shell >> result_list;
std::cout << result_list << std::endl; // Prints List[1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
std::cout << result_list->stringify() << std::endl; // Prints List[1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
```

`mathematica::value` object can be converted to an equivalent STL container like `std::vector` using `mathematica::cast`

```cpp
std::vector<int> list;
list = cast<std::vector<int>>(result_list);
```

## Executing intermediate returned output
An `mathematica::value` object can be using to build a `mathematica::m` construct. Here `res_mata` and `res_matb` are values returned by mathematica that we are passing inside `Det`.
```cpp
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
```

## Graphics Objects saved on the fly
