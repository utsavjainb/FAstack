#include <iostream>
#include <atomic>
#include <vector>
#include <stdio.h>
#include "FAstack_structs.h"
#include <ctime>                                                                                                                    

#define MAX_FAILURES 100 
#define NUMELEMS 1000
#define NUMTHREADS 24 

Stack* s;
std::atomic<int> pc; 

int get_timestamp(){
    return std::time(0);    
}

bool equal_states(State s1, State s2){
	return s1.id == s2.id && s1.pending == s2.pending;
}

bool equal_elements(Element e1, Element e2){
	return e1.e == e2.e;
}

void stack_init(){
    s = (Stack*) malloc(sizeof *s);
    Segment* top = new_segment(0);
    Segment* next = new_segment(1);
    top->next.store(next,std::memory_order_relaxed);
    next->prev.store(top,std::memory_order_relaxed);
    s->top.store(top,std::memory_order_relaxed);
    s->T = 1;
    pc = 1;
}


Cell* find_cell(Segment **sp, int cell_id){
    Segment* sg = *sp;
    for (int i = sg->id; i < cell_id/N; i++){
		Segment* next;
        if (sg == s->top){
            next = sg->next.load(std::memory_order_relaxed);
            Segment* nnext = next->next;
            if (nnext == NULL){
                Segment* tmp = new_segment(i+2);
                tmp->prev = next;
                Segment* snull = NULL;
                //if (!std::atomic_compare_exchange_strong_explicit(&next->next, &snull, tmp, std::memory_order_release, std::memory_order_relaxed)){
                if (!next->next.compare_exchange_strong(snull, tmp, std::memory_order_release, std::memory_order_relaxed)){
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

void cleanup(Handle *h) {
    Segment *prev = NULL;
    //for(Segment *cur = h->free_list.load(); cur != NULL; cur = cur->next.load()) {
    Segment *cur = h->free_list.load();
    while(cur != NULL) { 
        bool freemark = true;
        for(Handle* ph = h->push.peer; ph != NULL; ph = ph->next) {
            if (cur == s->top || ph->time_stamp <= cur->time_stamp || ph->top == cur ) {
                freemark = false;
                break;
            }
        }
        if (freemark) {
            if (prev != NULL) {
                prev->next.store(cur->next);
                cur->next.load()->prev = prev;
                //need to fix this line, cannot free and then iterate to cur next
                Segment* ncur = cur->next.load();
                prev = cur;
                free(cur);
                cur = ncur;
                continue;
            } else {
                h->free_list.store(cur->next); 
                cur->next = NULL;
            }
        }
        prev = cur;
        cur = cur->next.load();
    }

}

void remove(Handle *h , Segment *sp) {
    sp->retired = true;
    int i = 1;
    Segment* next = sp->real_next.load(std::memory_order_acquire);
    if(next == NULL) {
        next = sp->next;
    }
    Segment* rnext;
    while(i++ < NUMTHREADS && next->retired) {
        rnext = next->real_next.load(std::memory_order_acquire);
        if( rnext == NULL) {
            next = next->next;
        } else {
            next = rnext; 
        }
    }
    i = 1;
    Segment* prev = sp->prev.load(std::memory_order_acquire);
    Segment* nprev;
    Segment* pnext; 
    while(i++ < NUMTHREADS && prev != NULL && prev->retired) {
        prev = prev->prev;
    }
    nprev = next->prev;
    
    while(nprev != NULL && (prev == NULL || nprev->id > prev->id) && !next->prev.compare_exchange_strong(nprev, prev) ) {
        nprev = next->prev; 
    }
    if (prev == NULL) {
        goto RFLAG;
    }
    pnext = prev->real_next.load(std::memory_order_acquire);
    while((pnext == NULL || pnext->id < next->id) && (!prev->real_next.compare_exchange_strong(pnext, next))) {
        pnext = prev->real_next;
    }

    RFLAG:sp->time_stamp = get_timestamp();
    //std::atomic<Segment*> rover = h->free_list;  
    Segment* rover = h->free_list.load(std::memory_order_acquire);  
    while(rover != NULL) {
        Segment* nrover =  rover->next.load(std::memory_order_acquire);
        if (nrover == NULL) {
            rover->next.store(sp, std::memory_order_release);
            sp->prev = rover;
            break;
        }
        rover = nrover;
    }
    cleanup(h); 

}


Element help_push(Handle *h, Cell* c, int i) {
    if (!c->elem.compare_exchange_strong(uptickE, tickE) && !equal_elements(c->elem, tickE)) {
        return c->elem;
    }
    Handle* p;
    PushReq* r;
    State s;
    if (c->push == uptickPush) {
        p = h->push.peer;
        r = &p->push.req; 
        s = r->state.load(std::memory_order_acquire);
        if ( h->push.help_id != 0 and h->push.help_id != s.id) {
            h->push.help_id = 0;
            h->push.peer = p->next;
            p = h->push.peer;
            r = &p->push.req;
            s = r->state.load(std::memory_order_acquire);
        }
        
        if (s.pending && s.id <= i) {
            c->push.compare_exchange_strong(uptickPush, r, std::memory_order_release, std::memory_order_relaxed);
        }
        if (s.pending and c->push != r) {
            h->push.help_id = s.id;
        } else {
            h->push.help_id = 0;
            h->push.peer = p->next;
        }
        c->push.compare_exchange_strong(uptickPush, tickPush);
    }
    if(c->push = tickPush) {
        return tickE;
    }
    r = c->push;
    s = r->state.load(std::memory_order_acquire);
    Element v = r->elem;
    State wr = {1, s.id};
    State qr = {0, i};
    if (s.id <= i and r->state.compare_exchange_strong(wr, qr) || equal_states(r->state, qr)) {
        c->elem = v;
    }
    return c->elem;
}
    


void wf_push(Handle* h, Element x, int push_id) {
    PushReq* r = &h->push.req;
    r->elem = x;
    //not sure if this is correct way, have to construct a State and then store atomically
    State tempstate = {1, push_id};
    //State tempstate{.id = push_id};
    r->state.store(tempstate, std::memory_order_relaxed);
    //r->state.pending = 1;
    //r->state.id = push_id;
    Segment* sp = h->top; 
    Cell* c;
    do {
        int i = std::atomic_fetch_add(&s->T, 1);
        c = find_cell(&sp, i);
        if(c->push.compare_exchange_strong(uptickPush, r) || c->push == r) {
            /*
            State* checkstate;
            checkstate->pending = 1;
            checkstate->id = push_id;
            State* newstate;
            newstate->pending = 0;
            newstate->id = i;
            */
            State checkstate = {1, push_id};
            State newstate = {0, i};
            //if (r->state.compare_exchange_strong(*checkstate, *newstate, std::memory_order_relaxed, std::memory_order_relaxed) || equal_states(r->state,*newstate)) {
            if (r->state.compare_exchange_strong(checkstate, newstate, std::memory_order_relaxed, std::memory_order_relaxed) || equal_states(r->state, newstate)) {
                break;
            }
        }
        if (c->push == r) {
            // pop pointer in Cell must be atomic
            //if (std::atomic_compare_exchange_strong_explicit(&c->pop, &uptickPop, tickPop)){
            if (c->pop.compare_exchange_strong(uptickPop, tickPop)){
                //int counter = std::atomic_fetch_add(sp->counter, 1);
                int counter = sp->counter.fetch_add(1, std::memory_order_relaxed);
                if (counter == N - 1) {
                    remove(h, sp);
                }
            }
        }
    //} while(r->state.pending != 0);
    //UNSURE: does this have to be atomic?
    } while(r->state.load().pending != 0);
    int i = r->state.load().id;
    find_cell(&h->top, i);  
    c->elem = x;
}

//PUSH
void push(Handle* h, Element x) {
    h->time_stamp = get_timestamp();
    h->top = s->top.load(std::memory_order_relaxed);
    bool flag = false;
    int i;
    for(int p = 0; p < MAX_FAILURES; p++) {
        i = std::atomic_fetch_add(&s->T, 1);
        Cell* c = find_cell(&h->top, i);

        if(c->elem.compare_exchange_strong(uptickE, x, std::memory_order_release, std::memory_order_relaxed)){
            flag = true;
            break;
        }
    }
    if(!flag) {
        wf_push(h, x, i);
    }
    h->time_stamp = tickTime;
    h->top = NULL;
}

void help_pop(Handle* h, Handle* helpee){
    helpee->pop.req;
	PopReq* r = &helpee->pop.req;
	State s = r->state.load(std::memory_order_acquire);
	if (!s.pending) {
		return;
	}
	int idx = r->idx;
	Segment *sp = helpee->sp;
	h->time_stamp = helpee->time_stamp;
	//TODO: Look into this for loop since it was worded incorrectly.
	for (int i = idx % N; sp != NULL && i != N-1; sp = sp->prev){
		if (!equal_states(r->state,s)){
			return;
		}
		while (i >= 0){
			Cell* c = sp->cells[i];
			int cid = sp->id*N + i;
			Element v = help_push(h, c, cid);
			if (!equal_elements(v,tickE) && (c->pop.compare_exchange_strong(uptickPop,r,std::memory_order_release, std::memory_order_relaxed) || c->pop == r)){
				State wr = {0,cid};
				r->state.compare_exchange_strong(s, wr, std::memory_order_release, std::memory_order_relaxed);
				return;
			} else {
				if (equal_elements(v, tickE) && c->pop.compare_exchange_strong(uptickPop, tickPop, std::memory_order_release, std::memory_order_relaxed)){
					int counter = sp->counter.fetch_add(1, std::memory_order_acquire);
					if (counter == N-1){
						remove(h,sp);
					}
				}
			}
			i--;
		}
	}
	if (equal_states(r->state,s)){
		State w = {0,0};
		r->state.compare_exchange_strong(s,w, std::memory_order_release, std::memory_order_relaxed);
	}
}

Element wf_pop(Handle* h, int cid){
	PopReq* r = &h->pop.req;
	r->idx = cid;
	r ->state = {1, pc.fetch_add(1, std::memory_order_relaxed)};
	help_pop(h,h);
	int i = r->state.load(std::memory_order_acquire).id;
	if (i == 0){
		// Empty means we return the "empty element"
		return emptyE;
	}
	Cell * c = find_cell(&h->sp, i);
	Element v = c->elem;
	c->pop.store(tickPop, std::memory_order_acquire);
	int counter = h->sp->counter.fetch_add(1, std::memory_order_relaxed);
	if (counter == N-1){
		remove(h,h->sp);
	}
	return v;
}

//POP
Element pop(Handle * h){
	h->time_stamp = get_timestamp();
	h->top = s->top.load(std::memory_order_acquire);
	int t = s->T.load(std::memory_order_acquire);
	find_cell(&h->top,t);
	Segment* sp = h->top;
	Element v;
	int idx = 0;
	if (t % N != 0){
		idx = t-1;
	} else {
		sp = sp->prev;
		if (sp != NULL){
			idx = sp->id * N + N-1;
		}
	}
	int p = 0;
    
	while (sp != NULL) {
		if (sp->id <= idx/N){
			if (sp->id < idx/N){
				idx = sp->id*N +N-1;
			}
			if (p == MAX_FAILURES){
				goto FLAG1;
			}
			int i = idx % N;
			Cell* c = sp->cells[i];
			int offset = c->offset.fetch_add(1, std::memory_order_acquire);
			idx -= offset;
			if (idx < 1){
				v = emptyE;
				goto FLAG3;
			}
			p++;
			if (offset != 0 ){
				continue;
			}
			v = help_push(h, c, idx);
			if (c->pop.compare_exchange_strong(uptickPop,tickPop)){
				int counter = sp->counter.fetch_add(1, std::memory_order_acquire);				
				if (counter == N-1){
					remove(h,sp);
				}
				if (!equal_elements(v,tickE)) {
					goto FLAG2;
				}
			}
			idx -= c->offset.fetch_add(1, std::memory_order_acquire);
		} else {
			sp = sp->prev;
		}
	}
	if (sp == NULL){
		v = emptyE;
		goto FLAG3;
	}

	FLAG1:h->sp = sp;
		v = wf_pop(h,idx);
	FLAG2:if (!equal_elements(v,emptyE)){
			help_pop(h, h->pop.peer);
			h->pop.peer = h->pop.peer->next;
		  }
	FLAG3:h->time_stamp = tickTime;
		  h->top = NULL;
          /* 
          if (v.e != -3) {
              //std::cout << "Popped:  " << v.e << std::endl;
          } else {
              //std::cout << "Stack is empty: nothing to pop" << std::endl;

          }          
          */ 
          //alloc_peers(h);
          //alloc_poppeer(h);
		  return v;
}





