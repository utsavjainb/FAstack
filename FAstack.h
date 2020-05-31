#include <atomic>

#define N 4
#define MAX_FAILURES 10 

struct Element {
    int e;
};


struct State {
    int pending{1};
    int id{63};
};
    
// Pending represents whether the push or pop is uptack or tack. 1 means uptack, 0 means tack.

/*
struct {
    int pending{1};
    int id{63};
} state;
*/

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

    int offset{64};
};

struct Segment {
    int id{64};
    int counter{64};
    int time_stamp{64};
    bool retired;
	std::atomic<Segment*> prev;
    std::atomic<Segment*> next;
    std::atomic<Segment*> real_next; 
    std::atomic<Segment*> free_next;
    Cell cells[N];
};



struct Handle {
    Segment* top;
    Segment* sp; 
    Segment* free_list;
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

struct Stack {
	std::atomic<Segment*> top;   
    //int T{64};
    //std::atomic<int> T{64};
    std::atomic<int> T{ATOMIC_VAR_INIT(64)};
};
