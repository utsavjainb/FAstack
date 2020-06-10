#include <atomic>

#define N 120 

struct Element {
    int e;
};

struct State {
    int pending{1};
    int id{63};
};

class PushReq {
  public:
    Element elem;
    std::atomic<State> state;
    PushReq() {}
};

class PopReq {
  public:
    int idx{64};
    std::atomic<State> state;
    PopReq() {}
};

PushReq* uptickPush;
PushReq* tickPush;

PopReq* uptickPop;
PopReq* tickPop;

Element uptickE;
Element tickE;
Element emptyE;

int uptickTime = -1;
int tickTime = -2;


struct Cell {
    std::atomic<Element> elem;
    std::atomic<PushReq*> push;
    std::atomic<PopReq*> pop;
    std::atomic<int> offset{64};
};


struct Segment {
    int id{64};
    std::atomic<int> counter{64};
    int time_stamp{64};
    bool retired;
    std::atomic<Segment*> prev;
    std::atomic<Segment*> next;
    std::atomic<Segment*> real_next; 
    std::atomic<Segment*> free_next;
    Cell* cells[N];
};

Segment* init_segment(int id){
    Segment* sg = (Segment*) malloc(sizeof(Segment));
    sg->id = id;
    sg->counter = 0;
    sg->time_stamp = uptickTime;
    sg->retired = false;
    sg->prev = NULL;
    sg->next = NULL;
    sg->real_next = NULL;
    sg->free_next = NULL;
    return sg;
}

Segment* new_segment(int id) {
    Segment* sg = init_segment(id); 
    for (int i = 0; i < N; i++){
        sg->cells[i] = (Cell*) malloc(sizeof(Cell));
        sg->cells[i]->elem.store(uptickE, std::memory_order_release);
        sg->cells[i]->push = uptickPush;
        sg->cells[i]->pop = uptickPop;
    }   
    return sg; 
}

class Handle{
  public:
    class hPush {
      public:
        int help_id;
        PushReq req;
        Handle* peer;
        hPush() { 
            help_id = 64;
        }
    };
    class hPop {
      public:
        PopReq req;
        Handle* peer;
        hPop() {
        }
    };
    Segment *top; 
    Segment *sp; 
    std::atomic<Segment*> free_list;
    int time_stamp;
    Handle* next;
    hPush push;
    hPop pop;
    Handle() {
        time_stamp = 64;    
        top = new_segment(1);
        sp = new_segment(2);
        push.peer = this; 
        pop.peer = this; 
    }
}; 

void alloc_peers(Handle *h){
    h->push.peer = new Handle();
    h->pop.peer = new Handle();
}

void alloc_pushpeer(Handle *h) {
    h->push.peer = new Handle();
}

void alloc_poppeer(Handle *h) {
    h->push.peer = new Handle();
}

struct Stack {
    std::atomic<Segment*> top;   
    std::atomic<int> T{ATOMIC_VAR_INIT(64)};
};
