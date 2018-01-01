#ifndef Machine_H
#define Machine_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "states.h"
#include "symbols.h"

typedef struct{
    States *states; // Q
    Symbols *symbols; // Sigma
    States **transition; // Delta
    States *initialState; // q0
    States *finalState; // F

    State presentState;
    uint8_t isInitialized;
} Machine;

Machine* machine_new(States *states, Symbols *symbols, States  **transition, States *initialStates, States *finalStates);

// Peeking machine states
// These functions does not actually change machine state

bool machine_is_init_st(Machine *m, State s);
bool machine_is_final_st(Machine *m, State s);
States machine_next_state(Machine *m, State present, Symbol input);

// Managing machine states
// (These functions will only work correctly on a DFA)

void machine_init(Machine *m);
void machine_input(Machine *m, Symbol input);
State machine_present_state(Machine *m);

void machine_free(Machine *m);

#endif
