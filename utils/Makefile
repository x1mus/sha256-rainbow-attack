CC=g++
EXEC=main
FLAGS = -std=c++17 -Wall #-DDEBUG # -Wpedantic -Wextra -Wconversion -Weffc++ -Wstrict-null-sentinel -Wold-style-cast -Wnoexcept -Wctor-dtor-privacy -Woverloaded-virtual -Wsign-promo -Wzero-as-null-pointer-constant -Wsuggest-final-types -Wsuggest-final-methods -Wsuggest-override


all: $(EXEC)

main: main.o sha256.o reduction.o
	$(CC) -o $@ $^ -pthread

main.o: main.cpp argparse.hpp sha256.h reduction.hpp
	$(CC) -o $@ -c $< -pthread $(FLAGS)

sha256.o: sha256.cpp sha256.h
	$(CC) -o $@ -c $< $(FLAGS)

reduction.o: reduction.cpp reduction.hpp
	$(CC) -o $@ -c $< $(FLAGS)

clean:
	rm -rf *.o