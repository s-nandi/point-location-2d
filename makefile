CC = g++
CXXFLAGS = -std=c++11 -Iinclude

SRC = src/*.cpp

main: main.cpp
	$(CC) main.cpp $(SRC) -o main $(CXXFLAGS)

tester: tester.cpp
	$(CC) $(SRC) tester.cpp -o tester $(CXXFLAGS)

.PHONY : clean
clean:
	-rm main.o main tester.o tester