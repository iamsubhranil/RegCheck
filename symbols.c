#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdarg.h>

#include "symbols.h"

static Symbols *symbolTable = NULL;

static bool table_has(char s){
    uint64_t i = 0;
    while(i < symbolTable->numSymbols){
        if(symbolTable->symbols[i] == s)
            return true;
        i++;
    }
    return false;
}

static uint64_t table_indexof(char s){
    uint64_t i = 0;
    while(i < symbolTable->numSymbols){
        if(symbolTable->symbols[i] == s)
            return i;
        i++;
    }
    return 0;
}

Symbol sym_new(char s){
    if(symbolTable == NULL)
        symbolTable = sym_newcol();
   if(table_has(s))
       return table_indexof(s);
   else{
        sym_add(symbolTable, s);
        return symbolTable->numSymbols - 1;
   }
}

void sym_print_single(Symbol s){
    if(s >= symbolTable->numSymbols){
        printf("UnknownSymbol");
        return;
    }
    printf("%c", symbolTable->symbols[s]);
}

Symbols* sym_newcol(){
    Symbols *s = (Symbols *)malloc(sizeof(Symbols));
    s->numSymbols = 0;
    s->symbols = NULL;
    return s;
}

void sym_add(Symbols *collection, Symbol s){
    collection->numSymbols++;
    collection->symbols = (Symbol *)realloc(collection->symbols, sizeof(Symbol) * collection->numSymbols);
    collection->symbols[collection->numSymbols - 1] = s;
}

void sym_free(Symbols *collection){
    free(collection->symbols);
    free(collection);
}

bool sym_contains(Symbols *collection, Symbol s){
    uint64_t i = 0;
    while(i < collection->numSymbols){
        if(sym_iseq(collection->symbols[i], s))
            return true;
        i++;
    }
    return false;
}

void sym_print(Symbols *collection){
    if(collection->numSymbols == 0){
        printf("{ Phi }");
        return;
    }
    printf("{ ");
    sym_print_single(collection->symbols[0]);
    uint64_t i = 1;
    while(i < collection->numSymbols){
        printf(", ");
        sym_print_single(collection->symbols[i]);
        i++;
    }
    printf(" }");
}
