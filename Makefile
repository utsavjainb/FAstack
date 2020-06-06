all:
	g++ -std=c++14 threadtest.cpp  -g -o threadtest.out -l pthread
poppush:
	g++ -std=c++14 pop_push.cpp  -g -o pop_push.out -l pthread
pushtest:
	g++ -std=c++14 pushtest.cpp  -g -o pushtest.out -l pthread
poptest:
	g++ -std=c++14 poptest.cpp  -g -o poptest.out -l pthread
singlethread:
	g++ -std=c++14 main.cpp  -g -o main.out 
clean:
	rm *.out 
