Instructions to run benchmarks:

Push:
    make pushtest
    ./pushtest.out

Pop:
    make poptest 
    ./poptest.out

PushPop:
    make pushpoptest 
    ./pushpoptest.out

Cleanup to remove executables:
    make clean


Parameters:
All parameter are set for have the highest throughput for  FA-stack.
Threads = 16 
MAX_FAILURES=100 
N (# of Cells per Segment) = 120
Elements = 1000
