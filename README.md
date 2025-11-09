# Symbol Table Manager

The Symbol Table Manager project is an educational tool designed to simulate fundamental 
aspects of a compiler, with a specific focus on symbol table management and semantic analysis. 
The system reads C-style code either from files or through interactive console input and 
processes variable declarations, function scopes, and arithmetic expressions. The simulation 
supports three primary data types—int, float, and char—and can handle arithmetic 
operations such as addition, subtraction, multiplication, and division. The program generates 
detailed outputs, including memory addresses for global variables, local offsets for function
level variables, and comprehensive warnings or errors for semantic inconsistencies.

## Scope
### Supported data types:

1. int
2. float
3. char

### Supported operations

1. variable declaration
2. arithmetic expressions
3. assignments
4. function scope management

## Data Structures:

1. SymbolEntry Struct: Stores variable information such as name, type, scope, size, 
memory address or local offset, and status. 
2. DataType Enum: Represents supported types (int, float, char) and unknown types for 
error detection. 
3. Global State Variables: Track the symbol table, next available global address, local 
offset, current scope, and error count.

## Methodology

1. Input Processing: Users can provide code either via files or console input. For file 
input, the program requests the filename and reads the code line by line. 
2. Parsing: Each line is trimmed of whitespace and analyzed to identify its type—
 declaration, assignment, or scope change. Tokenization allows extraction of variable 
names, types, and expressions. 
3. Scope Management: The program tracks the current scope. Entering a function 
pushes a new local scope, resetting the local offset, while exiting a function pops the 
scope back to global. 
4. Symbol Table Insertion: Variables are inserted into the symbol table with their 
attributes. Global variables are assigned absolute addresses, while local variables are 
assigned offsets relative to the function’s base pointer. Redeclaration errors within the 
same scope are detected. 
5. Expression Evaluation: Arithmetic expressions are parsed and validated. The system 
supports +, -, *, and / operations. Assignments of one variable to another are also 
supported. Type mismatches generate warnings. 
6. Error Reporting: Semantic errors such as undeclared variables or redeclarations are 
reported immediately, along with type mismatch warnings for incompatible 
assignments. 
7. Output Display: A complete table summarizing all symbols, memory allocations, and 
any detected errors or warnings is displayed for user reference.

## Key Functions

1. insertSymbol() – Inserts a new variable into the symbol table, checking for 
redeclarations in the same scope. Assigns memory addresses for global variables and 
offsets for locals. 
2. lookupSymbol() – Searches for a variable in the symbol table, returning its index if 
found. 
3. handleDeclaration() – Processes variable declaration lines, converting type strings to 
enum values and invoking insertSymbol(). 
4. handleExpression() – Handles assignment and arithmetic operations. Performs type 
checks, generates warnings for type mismatches, and reports undeclared variable 
errors. 
5. pushScope() / popScope() – Manage function-level scopes, resetting local offsets 
upon entering a new function. 
6. parseAndProcessLine() – Determines the type of each code line (declaration, 
assignment, scope change) and calls the corresponding handler function. 
7. displaySymbolTable() – Prints a detailed table of all symbols, including type, scope, 
size, memory addresses, offsets, and compilation status.



## Future Scope:

1. Support for additional data types
2. Arrays and Pointers
3. Nested Function
4. Graphical User Interface
5. Extended Compilation Pipeline


## License

MIT license