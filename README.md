# Translator

A small C++20 translator front end that performs lexical and syntax analysis for a C-like educational language.

The current executable:

1. Reads source text from `input.txt`.
2. Tokenizes it with `LexicalAnalyzer`.
3. Writes tokens to `lexemes.txt`.
4. Runs `SyntaxAnalyzer` over `lexemes.txt`.
5. Prints whether lexical and syntax analysis completed successfully, or reports the first detected error.

Semantic analysis, internal representation generation, and program execution are currently placeholders in `main.cpp`.

## Requirements

- CMake 3.23 or newer
- A C++20 compiler

## Build

From the repository root:

```sh
cmake -S . -B build
cmake --build build
```

This creates the `translator` executable in the selected build directory.

## Run

The program uses relative file paths, so run it from a directory that contains both:

- `input.txt` - source program to analyze
- `ReservedInput.txt` - reserved words, one per line

Example:

```sh
cd cmake-build-debug
./translator.exe
```

On Linux/macOS with a fresh `build` directory, the executable name is usually `translator`:

```sh
cd build
./translator
```

After a successful lexical pass, `lexemes.txt` is written in the same working directory.

## Input Files

### `input.txt`

Contains the program source to analyze.

Example:

```txt
int f(int xx, string yy) {
    return 0;
}

const float minusPI = -3.14;
int a = (((2 + minusPI) // 2 + 45 % (7 - 2)));

string main() {
    list[5] int arr = {1, 2, 3, 4, 5};
    input(a);
    print(arr[3]);
    return ")";
}
```

### `ReservedInput.txt`

Contains reserved words used by the lexical analyzer. The project sample includes:

```txt
int
return
for
if
float
bool
break
continue
main
const
print
input
while
string
void
list
true
false
```

## Language Features

The analyzer currently recognizes:

- Primitive types: `int`, `float`, `bool`, `string`
- Fixed-size lists: `list[n] type`
- Constants with `const`
- Function declarations and calls
- Required entry point: `string main()`
- Blocks with `{ ... }`
- Variable declarations and assignments
- `if` / `else`
- `while`
- `for`
- `break` and `continue` inside loops
- `return`
- `input(name);`
- `print(name);`, `print(name[index]);`, and string literals
- Integer, float, boolean, string, and list literals
- Comments beginning with `#` and continuing to the end of the line

Supported operators include:

- Assignment: `=`, `+=`, `-=`, `*=`, `/=`, `%=`, `//=`
- Arithmetic: `+`, `-`, `*`, `/`, `//`, `%`
- Comparison: `>`, `<`, `>=`, `<=`, `==`, `!=`
- Boolean: `!`, `&&`, `||`
- Indexing with `[ ... ]`

## Lexeme Output

`lexemes.txt` stores one lexeme per line:

```txt
<lexeme> <token_type> <line>
```

The analyzer uses these token type values:

- `1` - reserved word
- `2` - identifier
- `3` - numeric literal
- `4` - operator
- `5` - punctuation
- `6` - round bracket
- `7` - square bracket
- `8` - string literal
- `0` - end marker

## Project Structure

```txt
CMakeLists.txt
main.cpp
LexicalAnalyzer.h
LexicalAnalyzer.cpp
SyntaxAnalyzer.h
SyntaxAnalyzer.cpp
```

## Notes

- The program expects `input.txt` and `ReservedInput.txt` in the current working directory, not necessarily beside the source files.
- `lexemes.txt` is generated and overwritten on each successful lexical analysis run.
- The parser reports line-based syntax errors and checks bracket balance before syntax analysis.
- The repository contains `cmake-build-debug/`, which appears to be a generated CLion build directory.
