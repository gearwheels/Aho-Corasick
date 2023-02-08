FLAGS = -O2 -g -pedantic -std=c++17 -Wall -Werror

all: main
	g++ $(FLAGS) main.o -o solution

main: main.cpp
	g++ $(FLAGS) -c main.cpp

clean:
	rm -rf *.o solution