#include <atomic>

//#define N 10 
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



struct Cell {
    std::atomic<Element> elem;
    //PushReq *push;
    //PopReq *pop;
    std::atomic<PushReq*> push;
    std::atomic<PopReq*> pop;

	std::atomic<int> offset{64};
};

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
    Cell* cells[4];
};

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
    }
}; 

void alloc_peers(Handle *h){
    h->push.peer = new Handle();
    h->pop.peer = new Handle();
}

struct Stack {
	std::atomic<Segment*> top;   
    std::atomic<int> T{ATOMIC_VAR_INIT(64)};
};
