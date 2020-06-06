#include <iostream>
#include <atomic>
#include <vector>
#include <stdio.h>
#include "FAstack.h"
#include <pthread.h>
#include <unistd.h>
#include <ctime>                                                                                                                    
#include <chrono>


#define NUMELEMS 100
#define NUMTHREADS 4 

void* push_thread(void* arg) {
    int thID = (long) arg;
    Handle* hr = new Handle();
    alloc_peers(hr);

    std::vector<Element> popElems;
    //std::cout << "pushing.. " << std::endl;
    for(int i = 1; i <= NUMELEMS; i++) {
        push(hr, Element{i}); 
        std::cout << "Thread " << thID << " pushed " << i << std::endl;
        //sleep(0.3);
    }
}

void* pop_thread(void* arg) {
    int thID = (long) arg;
    Handle* hr = new Handle();
    alloc_peers(hr);

    Element popped; 
    //std::cout << "popping.. " << std::endl;
    for(int i = 1; i <= NUMELEMS; i++) {
        popped = pop(hr);
        //std::cout << "Thread " << thID << " popped " << i << std::endl;
    }
    //std::cout << "done!" << std::endl;
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
    int rc;
    int rc2;
    pthread_t  pushThreads[NUMTHREADS];
    pthread_t  popThreads[NUMTHREADS];
    
    for (int i = 0; i < NUMTHREADS; i++) {
        rc = pthread_create(&pushThreads[i], NULL, push_thread, (void*)i);
        if (rc) {
            printf("ERROR,  unable to create thread ");
            std::cout << rc << std::endl;
            exit(-1);
        }   
    }   


    
    for (int i = 0; i < NUMTHREADS; i++) {
        rc = pthread_join(pushThreads[i], NULL);
        if (rc) {
            printf("ERROR,  unable to create thread ");
            std::cout << rc << std::endl;
            exit(-1);
        }   
    }   


    std::cout << "DONE PUSH" << std::endl;

    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUMTHREADS; i++) {
        rc = pthread_create(&popThreads[i], NULL, pop_thread, (void*)i);
        if (rc) {
            printf("ERROR,  unable to create thread ");
            std::cout << rc << std::endl;
            exit(-1);
        }   
    }   

    for (int i = 0; i < NUMTHREADS; i++) {
        rc = pthread_join(popThreads[i], NULL);
        if (rc) {
            printf("ERROR,  unable to create thread ");
            std::cout << rc << std::endl;
            exit(-1);
        }   
    }   
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time-start_time);
    double seconds = duration.count()/1000000.0;
    printf("Time: %e s\n", seconds);


    return 0;

}
