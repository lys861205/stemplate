CXX=g++
CXXFLAG=-std=c++11

all: test_template test_ctemplate

test_template: test_template.o stemplate.o
	g++ -std=c++11 -g test_template.o stemplate.o -o test_template

test_ctemplate: test_ctemplate.o
	g++ -std=c++11 -g test_ctemplate.o -o test_ctemplate -lctemplate -lpthread

test_template.o: test_template.cc
	g++ -c -g -std=c++11 test_template.cc      

stemplate.o: stemplate.cc      
	g++ -c -g -std=c++11 stemplate.cc

test_ctemplate.o: test_ctemplate.cc
	g++ -c -g -std=c++11 test_ctemplate.cc -lctemplate -lpthread

clean:
	rm -f *.o test_template test_ctemplate
