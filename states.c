#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>

#include "states.h"

States* state_newcol(){
    States *s = (States *)malloc(sizeof(States));
    s->numStates = 0;
    s->states = NULL;
    return s;
}

void state_add(States *collection, State state){
    if(state_contains(collection, state))
        return;

    collection->numStates++;
    collection->states = (State *)realloc(collection->states, sizeof(State) * collection->numStates);
    collection->states[collection->numStates - 1] = state;
}

bool state_contains(States *collection, State state){
    uint64_t i = 0;
    while(i < collection->numStates){
        if(state_iseq(collection->states[i], state))
            return true;
        i++;
    }
    return false;
}

void state_free(States *s){
    free(s->states);
    free(s);
}

void state_print(States *collection){
    if(collection->numStates == 0){
        printf("{ Phi }");
        return;
    }
    printf("{ ");
    state_print_single(collection->states[0]);
    uint64_t i = 1;
    while(i < collection->numStates){
        printf(", ");
        state_print_single(collection->states[i]);
        i++;
    }
    printf(" }");
}
