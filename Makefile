all:
	g++ -std=c++14 threadtest.cpp  -g -o threadtest -l pthread
singlethread:
	g++ -std=c++14 main.cpp  -g -o main 
clean:
	rm threadtest main
