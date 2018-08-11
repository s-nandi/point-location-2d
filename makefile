CC = g++
CXXFLAGS = -std=c++14 -Iinclude

SRC = src/*.cpp

tester: tester.cpp
	$(CC) $(SRC) tester.cpp -o tester $(CXXFLAGS)

.PHONY : clean
clean:
	-rm tester.o tester