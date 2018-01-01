#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include "machine.h"
#include "display.h"

Machine* machine_new(States *states, Symbols *symbols, States **transition, States *initialStates, States *finalStates){
    Machine *m = (Machine *)malloc(sizeof(Machine));
    m->states = states;
    m->symbols = symbols;
    m->transition = transition;
    m->initialState = initialStates;
    m->finalState = finalStates;
    return m;
}

bool machine_is_init_st(Machine *m, State st){
    return state_contains(m->initialState, st);
}

bool machine_is_final_st(Machine *m, State st){
    return state_contains(m->finalState, st);
}

States machine_next_state(Machine *m, State presentState, Symbol input){
    return m->transition[state_id(presentState)][sym_id(input)];
}

void machine_init(Machine *m){
    if(m->initialState->numStates > 1){
        err("Unable to initialize machine! Multiple initial states!");
        return;
    }
    m->presentState = m->initialState->states[0];
    m->isInitialized = 1;
}

void machine_input(Machine *m, Symbol input){
    if(!m->isInitialized){
        err("Unable to input : Machine is not initialized!");
        return;
    }
    m->presentState = m->transition[state_id(m->presentState)][sym_id(input)].states[0];
}

State machine_present_state(Machine *m){
    return m->presentState;
}

void machine_print(Machine *m){
    printf("\nSymbols : ");
    sym_print(m->symbols);
    printf("\nStates : ");
    state_print(m->states);
    printf("\nInitial State(s) : ");
    state_print(m->initialState);
    printf("\nFinal States(s) : ");
    state_print(m->finalState);
     
    printf("\nTransition table");
    printf("\n================");
    printf("\nPresent State\t");
    uint64_t i = 0, j = 0;
    while(i < m->symbols->numSymbols){
        printf("I/P=");
        sym_print_single(m->symbols->symbols[i]);
        printf("\t");
        i++;
    }
    printf("\n==============\t");
    i = 0;
    while(i < m->symbols->numSymbols){
        printf("=====\t");
        i++;
    }
    i = 0;
    printf("\n");
    while(i < m->states->numStates){
        state_print_single(m->states->states[i]);
        printf("\t");
        j = 0;
        while(j < m->symbols->numSymbols){
            state_print(&m->transition[i][j]);
            printf("\t");
            j++;
        }
        printf("\n");
        i++;
    }
}

int main(){
    info("Creating symbols..");
    Symbols *syms = sym_newcol();
    info("Adding a, b, c..");
    sym_add(syms, sym_new(0));
    sym_add(syms, sym_new(1));
    sym_add(syms, sym_new(2));
    info("Symbols created : ");
    sym_print(syms);
    
    info("Creating states..");
    States *states = state_newcol();
    info("Adding states q0, q1, q2..");
    state_add(states, state_new(0));
    state_add(states, state_new(1));
    state_add(states, state_new(2));
    info("States created : ");
    state_print(states);

    info("Creating q0 as initial state..");
    States *init = state_newcol();
    state_add(init, state_new(0));
    info("Creating q2 as final state..");
    States *end = state_newcol();
    state_add(end, state_new(2));

    info("Creating transition table..");
    States **table = NULL;
    uint64_t i = 0;
    uint64_t j = 0;
    while(i < 3){
        table = (States **)realloc(table, sizeof(States *) * (i + 1));
        table[i] = NULL;
        j = 0;
        while(j < 3){
            table[i] = (States *)realloc(table[i], sizeof(States) * (j + 1));
            States *trans = state_newcol();
            state_add(trans, state_new((i + j) % 3));
            table[i][j] = *trans;
            free(trans);
            info("Transition created : ");
            state_print_single(state_new(i));
            printf("---");
            sym_print_single((uint8_t)sym_new(j));
            printf("---> ");
            state_print_single(state_new((i + j) % 3));
            j++;
        }
        i++;
    }

    info("Creating machine..");
    Machine *m = machine_new(states, syms, table, init, end);
    machine_print(m);

    sym_free(syms);
    state_free(states);
}
