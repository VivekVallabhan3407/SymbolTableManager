#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h> // For parsing utility functions

// --- 1. CONFIGURATION AND DATA STRUCTURES ---
#define MAX_SYMBOLS 100
#define GLOBAL_START_ADDRESS 1000

typedef enum
{
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_UNKNOWN
} DataType;

typedef struct
{
    char name[30];
    DataType type;
    char scope[20];
    int size;
    int address_or_offset;
    int status;
} SymbolEntry;

SymbolEntry symbolTable[MAX_SYMBOLS];
int symbolCount = 0;

int nextGlobalAddress = GLOBAL_START_ADDRESS;
int nextLocalOffset = 0;
char currentScope[20] = "Global";
int compilationErrorCount = 0;

// --- 2. HELPER FUNCTIONS ---
int getSize(DataType type)
{
    switch (type)
    {
    case TYPE_INT:
        return 4;
    case TYPE_FLOAT:
        return 8;
    case TYPE_CHAR:
        return 1;
    default:
        return 0;
    }
}

DataType stringToType(const char *typeStr)
{
    if (strcmp(typeStr, "int") == 0)
        return TYPE_INT;
    if (strcmp(typeStr, "float") == 0)
        return TYPE_FLOAT;
    if (strcmp(typeStr, "char") == 0)
        return TYPE_CHAR;
    return TYPE_UNKNOWN;
}

const char *typeToString(DataType type)
{
    switch (type)
    {
    case TYPE_INT:
        return "INT";
    case TYPE_FLOAT:
        return "FLOAT";
    case TYPE_CHAR:
        return "CHAR";
    default:
        return "UNKNOWN";
    }
}

char *trim_whitespace(char *str)
{
    char *end;
    while (isspace((unsigned char)*str))
        str++;
    if (*str == 0)
        return str;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;
    end[1] = '\0';
    return str;
}

// --- 3. SCOPE MANAGEMENT ---
void pushScope(const char *scopeName)
{
    strncpy(currentScope, scopeName, sizeof(currentScope) - 1);
    currentScope[sizeof(currentScope) - 1] = '\0';
    nextLocalOffset = 0;
    printf("\n\n-> SCOPE CHANGE: Pushing new Local Scope: '%s'. Local Offset reset to 0 (Base Pointer).\n", currentScope);
}

void popScope()
{
    if (strcmp(currentScope, "Global") != 0)
    {
        printf("\n<- SCOPE CHANGE: Popping Scope: '%s'. Reverting to Global Scope.\n", currentScope);
        strcpy(currentScope, "Global");
    }
}

// --- 4. CORE SYMBOL TABLE OPERATIONS ---
int lookupSymbol(const char *name)
{
    for (int i = 0; i < symbolCount; i++)
    {
        if (strcmp(symbolTable[i].name, name) == 0)
        {
            return i;
        }
    }
    return -1;
}

int insertSymbol(const char *name, DataType type, const char *scope)
{
    if (symbolCount >= MAX_SYMBOLS)
    {
        printf("FATAL ERROR: Symbol Table capacity reached. Cannot insert %s.\n", name);
        compilationErrorCount++;
        return 0;
    }

    for (int i = 0; i < symbolCount; i++)
    {
        if (strcmp(symbolTable[i].name, name) == 0 && strcmp(symbolTable[i].scope, scope) == 0)
        {
            printf("ERROR (Semantic): Variable '%s' already declared in scope '%s'.\n", name, scope);
            compilationErrorCount++;
            return 0;
        }
    }

    SymbolEntry *newEntry = &symbolTable[symbolCount];
    strcpy(newEntry->name, name);
    newEntry->type = type;
    strcpy(newEntry->scope, scope);
    newEntry->size = getSize(type);
    newEntry->status = 1;

    if (strcmp(scope, "Global") == 0)
    {
        newEntry->address_or_offset = nextGlobalAddress;
        nextGlobalAddress += newEntry->size;
        printf("  -> INSERTED: '%s' [Type: %s, Scope: %s] @ Address %d.\n",
               name, typeToString(type), scope, newEntry->address_or_offset);
    }
    else
    {
        newEntry->address_or_offset = nextLocalOffset;
        nextLocalOffset += newEntry->size;
        printf("  -> INSERTED: '%s' [Type: %s, Scope: %s] @ Offset %d (Size: %d Bytes).\n",
               name, typeToString(type), scope, newEntry->address_or_offset, newEntry->size);
    }

    symbolCount++;
    return 1;
}

// --- 5. SEMANTIC ANALYSIS ---
void handleDeclaration(const char *typeStr, const char *name)
{
    DataType type = stringToType(typeStr);
    if (type == TYPE_UNKNOWN)
    {
        printf("ERROR (Syntax): Unknown type specifier '%s'.\n", typeStr);
        compilationErrorCount++;
        return;
    }
    insertSymbol(name, type, currentScope);
}

void handleExpression(const char *targetName, const char *sourceName)
{
    int targetIndex = lookupSymbol(targetName);
    int sourceIndex = lookupSymbol(sourceName);

    printf("\n- Processing Assignment: '%s = %s;'\n", targetName, sourceName);

    if (targetIndex == -1)
    {
        printf("  -> ERROR (Semantic): Target variable '%s' is UNdeclared.\n", targetName);
        compilationErrorCount++;
    }
    if (sourceIndex == -1)
    {
        printf("  -> ERROR (Semantic): Source variable '%s' is UNdeclared.\n", sourceName);
        compilationErrorCount++;
    }

    if (targetIndex != -1 && sourceIndex != -1)
    {
        if (symbolTable[targetIndex].type != symbolTable[sourceIndex].type)
        {
            printf("  -> WARNING (Type): Assignment involves different types (%s and %s).\n",
                   typeToString(symbolTable[targetIndex].type),
                   typeToString(symbolTable[sourceIndex].type));
        }
        printf("  -> Lookup Trace: '%s' found in scope '%s'.\n", targetName, symbolTable[targetIndex].scope);
        printf("  -> Lookup Trace: '%s' found in scope '%s'.\n", sourceName, symbolTable[sourceIndex].scope);
    }
}

void handleArithmeticExpression(const char *target, const char *source, char op, const char *number)
{
    int targetIndex = lookupSymbol(target);
    int sourceIndex = lookupSymbol(source);

    printf("\n- Processing Arithmetic Assignment: '%s = %s %c %s;'\n", target, source, op, number);

    if (targetIndex == -1)
    {
        printf("  -> ERROR (Semantic): Target variable '%s' is UNdeclared.\n", target);
        compilationErrorCount++;
    }
    if (sourceIndex == -1)
    {
        printf("  -> ERROR (Semantic): Source variable '%s' is UNdeclared.\n", source);
        compilationErrorCount++;
    }

    if (targetIndex != -1 && sourceIndex != -1)
    {
        if (symbolTable[targetIndex].type != symbolTable[sourceIndex].type)
        {
            printf("  -> WARNING (Type): Assignment involves different types (%s and %s).\n",
                   typeToString(symbolTable[targetIndex].type),
                   typeToString(symbolTable[sourceIndex].type));
        }
        printf("  -> Lookup Trace: '%s' found in scope '%s'.\n", target, symbolTable[targetIndex].scope);
        printf("  -> Lookup Trace: '%s' found in scope '%s'.\n", source, symbolTable[sourceIndex].scope);
    }
}

// --- 6. DISPLAY FUNCTION ---
void displaySymbolTable()
{
    printf("\n\n###################################################################\n");
    printf("FINAL SYMBOL TABLE SUMMARY (Total Entries: %d, Errors: %d)\n", symbolCount, compilationErrorCount);
    printf("###################################################################\n");
    printf("| NAME     | TYPE  | SCOPE    | SIZE | ADDRESS/OFFSET | END_ADDR |\n");
    printf("|----------|-------|----------|------|----------------|----------|\n");

    for (int i = 0; i < symbolCount; i++)
    {
        SymbolEntry *e = &symbolTable[i];
        int startAddr = e->address_or_offset;
        int endAddr = e->address_or_offset + e->size - 1;
        const char *addrLabel = (strcmp(e->scope, "Global") == 0) ? "" : " (BP)";
        printf("| %-8s | %-5s | %-8s | %-4d | %-12d%s | %-8d |\n",
               e->name, typeToString(e->type), e->scope, e->size, startAddr, addrLabel, endAddr);
    }
    printf("===================================================================\n");
    printf("NEXT GLOBAL START ADDRESS: %d\n", nextGlobalAddress);
    printf("-------------------------------------------------------------------\n");
    if (compilationErrorCount > 0)
    {
        printf("COMPILATION STATUS: FAILED with %d Semantic/Syntax Errors.\n", compilationErrorCount);
    }
    else
    {
        printf("COMPILATION STATUS: SUCCESS (No Semantic/Syntax Errors Detected).\n");
    }
    printf("###################################################################\n");
}

// --- 7. PARSING ---
void parseAndProcessLine(char *rawLine)
{
    char *line = trim_whitespace(rawLine);
    char token1[30], token2[30], token3[30], token4[30], op;

    if (strlen(line) == 0 || line[0] == '/')
        return;

    if (sscanf(line, "void %s() {", token1) == 1)
    {
        pushScope(token1);
    }
    else if (strcmp(line, "}") == 0)
    {
        popScope();
    }
    else if (sscanf(line, "%s %[^;];", token1, token2) == 2)
    {
        char varName[30];
        if (sscanf(token2, "%s", varName) == 1)
        {
            handleDeclaration(token1, varName);
        }
        else
        {
            printf("ERROR: Could not parse declaration on line: %s\n", rawLine);
            compilationErrorCount++;
        }
    }
    else if (sscanf(line, "%s = %s %c %s;", token1, token2, &op, token3) == 4)
    {
        handleArithmeticExpression(token1, token2, op, token3);
    }
    else if (sscanf(line, "%s = %s;", token1, token2) == 2)
    {
        handleExpression(token1, token2);
    }
    else
    {
        printf("WARNING (Syntax): Skipping unhandled syntax: %s\n", rawLine);
    }
}

void fileInputHandler(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[100];
    int lineNum = 1;
    printf("\n--- Processing File: %s ---\n", filename);

    while (fgets(line, sizeof(line), file) != NULL)
    {
        printf("\n[Line %02d]: %s", lineNum++, line);
        line[strcspn(line, "\n")] = 0;
        parseAndProcessLine(line);
    }

    fclose(file);
}

void consoleInputHandler()
{
    char line[100];
    printf("\n--- Interactive Console Mode ---\n");
    printf("Enter statements (e.g., 'int x;', 'void func() {', '}') or 'exit'.\n");

    while (1)
    {
        printf("\n[%s] > ", currentScope);
        if (fgets(line, sizeof(line), stdin) == NULL)
            break;
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "exit") == 0)
            break;
        parseAndProcessLine(line);
    }
}

// --- 8. MAIN MENU EXECUTION ---
int main()
{
    int choice;
    char again;

    do
    {
        // Reset symbol table for each run
        symbolCount = 0;
        nextGlobalAddress = GLOBAL_START_ADDRESS;
        nextLocalOffset = 0;
        strcpy(currentScope, "Global");
        compilationErrorCount = 0;

        printf("\n*** Symbol Table Manager ***\n");
        printf("This tool simulates a compiler's symbol table, managing variables, scopes,");
        printf("\nand semantic checks for C-style code with int, float, and char types.\n");
        printf("Choose an input mode to begin analysis:\n");

        printf("---------------------------------------------------\n");
        printf("1. Generate symbol table from an input file\n");
        printf("2. Interactive Console input\n");
        printf("---------------------------------------------------\n");
        printf("Enter choice (1 or 2): ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input. Exiting.\n");
            return 1;
        }

        while (getchar() != '\n')
            ; // clear input buffer

        if (choice == 1)
        {
            char filename[100];
            printf("Enter input filename (e.g., inputFiles/inputFile1.c): ");
            if (fgets(filename, sizeof(filename), stdin) == NULL)
                return 1;
            filename[strcspn(filename, "\n")] = 0;
            fileInputHandler(filename);

            displaySymbolTable();

            printf("\nDo you want to generate symbol table for another file? (y/n): ");
            scanf(" %c", &again);
            while (getchar() != '\n')
                ; // clear buffer
        }
        else if (choice == 2)
        {
            consoleInputHandler();

            displaySymbolTable();

            printf("\nDo you want to generate symbol table for another console input? (y/n): ");
            scanf(" %c", &again);
            while (getchar() != '\n')
                ; // clear buffer
        }
        else
        {
            printf("Invalid choice. Exiting.\n");
            return 0;
        }

    } while (again == 'y' || again == 'Y');

    printf("\nExiting Compiler Simulation\n");
    return 0;
}
