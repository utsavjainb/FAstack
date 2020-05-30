#include <pthread.h>
#include <iostream>
#include <cstddef>
#include <atomic>
#include <vector>
#include <atomic>
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <math.h>

enum state_status {OPEN, CLOSED};                                                                      

/*
struct SnziRootNode {
    int count;
    int state;
    //state_status state;
};
*/
enum tick {cheap, costly};
struct Ticket {
    tick ticker;
    Ticket(tick t) {
        ticker = t;
    }  
};

class SnziRootNode  {
  public:
    int count;
    state_status state;

    SnziRootNode(int c, state_status s) {
        count = c;
        state = s;
    }

};

Ticket getTicket() {

    return Ticket(cheap);    

}


int main () {
	std::atomic<SnziRootNode*> root;
    root.store(new SnziRootNode(3, OPEN));
    SnziRootNode* old = root.load(std::memory_order_relaxed);
    //SnziRootNode* newnode = new SnziRootNode(4,CLOSED);
    SnziRootNode* tail = new SnziRootNode(10,CLOSED);
    SnziRootNode* newnode = old;
    newnode->count++;
    std::cout << old->count << std::endl; 
    std::cout << newnode->count << std::endl; 
    std::cout << root.load(std::memory_order_relaxed)->count << std::endl; 
    
	if (atomic_compare_exchange_strong_explicit(&root, &old, tail, std::memory_order_release, std::memory_order_relaxed)){
        std::cout << " tail true"<< std::endl;
    }

    std::cout << old->count << std::endl; 
    std::cout << newnode->count << std::endl; 
    std::cout << root.load(std::memory_order_relaxed)->count << std::endl; 

    if ( newnode == new SnziRootNode(4, OPEN) ) {
        std::cout << "true"<< std::endl;
    }

    root.exchange(tail);
    std::atomic<int> x;
    x.store(5);
    int y = 5; 
	if (atomic_compare_exchange_strong_explicit(&x, &y, y+1, std::memory_order_release, std::memory_order_relaxed)){
        std::cout << "true intswap"<< std::endl;
    }
    std::cout << x.load() << std::endl; 
    
    return 0;
}
