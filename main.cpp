#include <iostream>
#include <atomic>
#include <vector>
#include <stdio.h>
#include "FAstack.h"
#include <ctime>                                                                                                                    

int main() {

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
    Element t1 = {1};
    Element t2 = {2};
    Element t3 = {3};
    Element t4 = {4};
    Element t5 = {5};

    stack_init();    
    Handle* hr = new Handle();
    alloc_peers(hr);
     
    push(hr, t1);
    push(hr, t2);
    push(hr, t3);
    push(hr, t4);
    push(hr, t5);
    Element t6 = pop(hr);
    Element t7 = pop(hr);
    Element t8 = pop(hr);
    Element t9 = pop(hr);
    Element t10 = pop(hr);
    Element t11 = pop(hr);

}
