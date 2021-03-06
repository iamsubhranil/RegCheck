#ifndef Symbols_H
#define Symbols_H

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t Symbol;

Symbol sym_new(char s);
#define sym_id(id) (id)
#define sym_iseq(sym1, sym2) (sym1 == sym2)
void sym_print_single(Symbol s);

typedef struct{
    uint64_t numSymbols;
    Symbol *symbols;
} Symbols;

Symbols* sym_newcol();
void sym_add(Symbols *collection, Symbol symbol);
void sym_free(Symbols *sym);
bool sym_contains(Symbols *sym, Symbol symbol);
void sym_print(Symbols *sym);
#endif
