.PHONY: run clean

main: main.cpp heap.hpp
	g++ -Wall -Wextra -std=c++20 main.cpp -o main

debug: main.cpp heap.hpp
	g++ -g -Wall -Wextra -std=c++20 main.cpp -o main
	gdb main

run: main
	./main

clean:
	rm main
