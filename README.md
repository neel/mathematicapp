# Mathematica++
A C++ Library to talk with Mathematica

Dot product and Determinant calculation in Mathematica Code

```Mathematica
mata = Table[1 - Mod[i + j, 2], {i, 1, 2}, {j, 1, 2}];
matb = Table[1 - Mod[i + j, 3], {i, 1, 2}, {j, 1, 2}];
matc = Dot[mata, matb];
matd = Det[matc];

```

```C++
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
mathematica::m mata = Table(1 - Mod(i + j, 2), List(i, 1, 2), List(j, 1, 2));
mathematica::m matb = Table(1 - Mod(i + j, 3), List(i, 1, 2), List(j, 1, 2)];
mathematica::m matc = Dot(mata, matb);
mathematica::m matd = Det(matc);

// Execute mathematica constructs and fetch the response
shell << matd;
shell >> determinant;

// determinant can be converted to C++ machine sized types
std::cout << determinant << std::endl; // Prints -2
```