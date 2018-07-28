CC = g++
CXXFLAGS = -std=c++11 -Iinclude

tester: tester.o
	$(CC) tester.o -o tester $(CXXFLAGS)

.PHONY : clean
clean:
	-rm tester.o tester makefile~ tester.cpp~