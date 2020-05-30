#include <iostream>
#include <atomic>
#include <vector>
#include <stdio.h>
#include "FAstack.h"
#include <ctime>                                                                                                                    


Stack* s;
int pc = 64; 

int get_timestamp(){
    return std::time(0);    
}

Segment * init_segment(int id){
	Segment * sg;
	sg->id = id;
	sg->counter = 0;
	sg->time_stamp = get_timestamp();
	sg->retired = false;
	sg->prev = NULL;
	sg->next = NULL;
	sg->real_next = NULL;
	sg->free_next = NULL;
	return sg;
}

Cell init_cell() {
	Cell c;
	Element e;
	c.elem = e;
	c.push = new PushReq();
	c.pop = new PopReq();
	return c;
}

Segment* new_segment(int id) {
    Segment* sg = init_segment(id); 
    for (int i = 0; i < N; i++){
        sg->cells[i] = init_cell(); 
    }   
    return sg; 
}

void stack_init(){
    s = new Stack();
    Segment * top = new_segment(0);
    Segment * next = new_segment(1);
    top->next.store(next,std::memory_order_relaxed);
    next->prev.store(top,std::memory_order_relaxed);
    s->top.store(top,std::memory_order_relaxed);
    s->T = 1;
    pc = 1;
}

Cell find_cell(Segment **sp, int cell_id){
    Segment * sg = *sp;
    for (int i = sg->id; i < cell_id/N; i++){
		Segment * next;
        if (sg == s->top){
            next = sg->next.load(std::memory_order_relaxed);
            Segment * nnext = next->next;
            if (nnext == NULL){
                Segment* tmp = new_segment(i+2);
                tmp->prev = next;
                Segment* snull = NULL;
                if (!std::atomic_compare_exchange_strong_explicit(&next->next, &snull, tmp, std::memory_order_release, std::memory_order_relaxed)){
                    free(tmp);
                }   
            }   
			std::atomic_compare_exchange_strong_explicit(&s->top, &sg, next, std::memory_order_release, std::memory_order_relaxed);
        } else {
            next = sg->next.load(std::memory_order_relaxed);
        }   
        sg = next;
    }   
    *sp = sg; 
    return sg->cells[cell_id % N]; 
}

int FAA(int* sTOP, int incr) {
    //wrapper for FAA, FAA object
    //that provides ID for each pop request

}

void push(Handle* h, Element x) {
    h->time_stamp = get_timestamp();
    h->top = s->top.load(std::memory_order_relaxed);
    bool flag = false;
    for(int p = 0; p < MAX_FAILURES; p++) {
        //T might have to be atomic
        int i = FAA(&s->T, 1);
        Cell c = find_cell(&h->top.load(std::memory_order_relaxed, i);
        //wtf is uptick
        if(std::atomic_compare_exchange_strong_explicit(&c->elem, uptick, x) {
            flag = true;
            break;
        }
    }
    if(!flag)  {
        wf_push(h, x, i);
    }
    //wtf is tick timestamp
    h->time_stamp = get_timestamp(tick);
    h->top = NULL;
}

void wf_push(Handle* h, Element x, int push_id) {
    PushReq* r = &h->push.req;
    r->elem = x;
    r->state.pending = 1;
    r->state.id = push_id;
    Segment* sp = h->top; 
    do {
        int i = FAA(&s->T, 1);
        Cell c = find_cell(&sp, i);
        if(std::atomic_compare_exchange_strong_explicit(&c->push, uptickpush, r) || c->push == r) {
            State* checkstate;
            checkstate->pending = 1;
            checkstate->id = push_id;
            State* newstate;
            newstate->pending = 0;
            newstate->id = i;
            if (std::atomic_compare_exchange_strong_explicit(&r->state, &checkstate, newstate, std::memory_order_relaxed, std::memory_order_relaxed) || r->state == newstate) {
                break;
            }
        }
        if (c->push == r) {
            // pop pointer in Cell must be atomic
            if (std::atomic_compare_exchange_strong_explicit(&c->pop, uptickpop, tickpop) {
                int counter = FAA(sp->counter, 1);
                if (counter == N - 1) {
                    remove(h, sp);
                }
            }
        }
    } while(r->state.pending != 0);
    int i = r->state.id;
    find_cell(&h->top, i);  
    c->elem = x;
}



int main() {
    Stack* s = new Stack();
    std::cout << s << std::endl;
}
