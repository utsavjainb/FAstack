#include <atomic>

#define N 10 
//#define MAX_FAILURES 0 
//#define MAX_FAILURES 10 
//int N = 10;

struct Element {
    int e;
};


struct State {
    int pending{1};
    int id{63};
};
    

struct PushReq {
    Element elem;
	std::atomic<State> state;
};

struct PopReq {
    int idx{64};
	std::atomic<State> state;
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

/*
class Cell {
  public:
    std::atomic<Element> elem;
    std::atomic<PushReq*> push;
    std::atomic<PopReq*> pop;
	std::atomic<int> offset;
    Cell() {
        offset = 64;
    }
}
*/

struct Segment {
    int id{64};
    //int counter{64};
    std::atomic<int> counter{64};
    int time_stamp{64};
    bool retired;
	std::atomic<Segment*> prev;
    std::atomic<Segment*> next;
    std::atomic<Segment*> real_next; 
    std::atomic<Segment*> free_next;
    //Cell* cells[4];
    Cell* cells[N];
};

/*
class Segment {
  public:
    int id;
    std::atomic<int> counter;
    int time_stamp;
    bool retired;
	std::atomic<Segment*> prev;
    std::atomic<Segment*> next;
    std::atomic<Segment*> real_next; 
    std::atomic<Segment*> free_next;
    Cell* cells[4];
    Segment() {
        id = 64;
        counter = 64;
        time_stamp = 64;
    }
}
*/

/*
struct Handle {
    Segment* top;
    Segment* sp; 
    std::atomic<Segment*> free_list;
    Handle* next;
    struct {
        int help_id{64};
        PushReq req;
        Handle *peer;
    } push;

    struct {
        PopReq req;
        Handle *peer;
    } pop;
    
    int time_stamp{64}; 
    
};
*/

Segment* init_segment(int id){
	Segment* sg = (Segment*) malloc(sizeof(Segment));
	sg->id = id;
	sg->counter = 0;
	//sg->time_stamp = get_timestamp();
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
        //free_list.store(new_segment(3), std::memory_order_relaxed);
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
