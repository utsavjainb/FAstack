#include <iostream>
#include <atomic>
#include <vector>
#include <stdio.h>
#include "FAstack.h"
#include <pthread.h>
#include <unistd.h>
#include <ctime>                                                                                                                    


void* thread_func(void* arg) {
    int thID = (long) arg;
    Handle* hr = new Handle();
    alloc_peers(hr);

    std::vector<Element> popElems;
    std::cout << "pushing.. " << std::endl;
    for(int i = 1; i <= 100; i++) {
        push(hr, Element{i}); 
        std::cout << "Thread " << thID << " pushed " << i << std::endl;
        sleep(0.1);
    }

    Element popped; 

    std::cout << "popping.. " << std::endl;
    for(int i = 1; i <= 100; i++) {
        popped = pop(hr);
        std::cout << "Thread " << thID << " popped " << i << std::endl;
    }
    std::cout << "done!" << std::endl;
}


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
    int nt = 3;
    int rc;
    pthread_t  stackThreads[nt];
    for (int i = 0; i < nt; i++) {
        rc = pthread_create(&stackThreads[i], NULL, thread_func, (void*)i);
        if (rc) {
            printf("ERROR,  unable to create thread ");
            std::cout << rc << std::endl;
            exit(-1);
        }   
    }   

    for (int i = 0; i < nt; i++) {
        rc = pthread_join(stackThreads[i], NULL);
        if (rc) {
            printf("ERROR,  unable to create thread ");
            std::cout << rc << std::endl;
            exit(-1);
        }   
    }   
    return 0;

}
