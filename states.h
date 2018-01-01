#ifndef States_H
#define States_H

#include <stdint.h>
#include <stdbool.h>

// State
// =============

typedef uint64_t State;

#define state_new(id) (id)
#define state_id(id) (id)
#define state_iseq(state1, state2) (state1 == state2)
#define state_print_single(id) printf("q%" PRIu64, id)
// ==============


// States
// =============

typedef struct{
    uint64_t numStates;
    State *states;
} States;

States* state_newcol();
void state_add(States *collection, State state);
void state_free(States *collection);
bool state_contains(States *collection, State state);
void state_print(States *collection);
// ==============

#endif
