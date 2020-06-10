#include <iostream>
#include <atomic>
#include <vector>
#include <stdio.h>
#include "FAstack.h"
#include <pthread.h>
#include <unistd.h>
#include <ctime>                                                                                                                    
#include <chrono>


void* push_thread(void* arg) {
    int thID = (long) arg;

    for(int i = 1; i <= NUMELEMS; i++) {
        push(handles[thID], Element{i*thID}); 
        //std::cout << "Thread " << thID << " pushed " << i << std::endl;
    }
}

void* pop_thread(void* arg) {
    int thID = (long) arg;

    Element popped; 
    for(int i = 1; i <= NUMELEMS; i++) {
        popped = pop(handles[thID]);
        std::cout << "Thread " << thID << " popped " << i << std::endl;
    }
}


int main() {
    init_handles();

    uptickPush = new PushReq();
    tickPush = new PushReq();
    uptickPop = new PopReq();
    tickPop = new PopReq();

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
    sleep(2);

    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUMTHREADS; i++) {
        rc = pthread_create(&popThreads[i], NULL, pop_thread, (void*)i);
        usleep(100);
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
