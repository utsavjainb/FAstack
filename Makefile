pushtest:
	g++ -std=c++14 pushtest.cpp  -g -o pushtest.out -l pthread
poptest:
	g++ -std=c++14 poptest.cpp  -g -o poptest.out -l pthread
pushpoptest:
	g++ -std=c++14 pushpoptest.cpp  -g -o pushpoptest.out -l pthread
pairwise:
	g++ -std=c++14 pairwise.cpp  -g -o pairwise.out -l pthread
clean:
	rm *.out 
