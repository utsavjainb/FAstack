#include <iostream>
#include <atomic>
#include <vector>
#include <stdio.h>
#include "FAstack.h"
#include <ctime>                                                                                                                    



int main() {
    //inits
    uptickPush = (PushReq*) malloc(sizeof *uptickPush);
    tickPush = (PushReq*) malloc(sizeof *tickPush);
    uptickPop = (PopReq*) malloc(sizeof *uptickPop);
    tickPop = (PopReq*) malloc(sizeof *tickPop);

    State utick = {1, -1};   
    State tick = {1, -1};   
    uptickPush->state.store(utick);
    tickPush->state.store(tick);
    uptickPop->state.store(utick);
    tickPop->state.store(tick);
    uptickE.e = -1;
    tickE.e = -2;
 	emptyE.e = -3;


    stack_init();    
    Handle* hr = new Handle();
    alloc_peers(hr);

    std::vector<Element> popElems;
    for(int i = 1; i <= 1000; i++) {
        push(hr, Element{i}); 
    }

    Element popped; 

    for(int i = 1; i <= 1000; i++) {
        popped = pop(hr);
    }

}
