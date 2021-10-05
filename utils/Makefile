CC=g++
EXEC=sha256-main check-passwd gen-passwd main
FLAGS = -std=c++17 -Wall # -Wpedantic -Wextra -Wconversion -Weffc++ -Wstrict-null-sentinel -Wold-style-cast -Wnoexcept -Wctor-dtor-privacy -Woverloaded-virtual -Wsign-promo -Wzero-as-null-pointer-constant -Wsuggest-final-types -Wsuggest-final-methods -Wsuggest-override


all: $(EXEC)

main: main.o
	$(CC) -o $@ $^ 

sha256-main: sha256-main.o sha256.o
	$(CC) -o $@ $^

check-passwd: check-passwd.o sha256.o
	$(CC) -o $@ $^

gen-passwd: gen-passwd.o sha256.o reduction.o
	$(CC) -o $@ $^

main.o: main.cpp argparse.hpp
	$(CC) -o $@ -c $< $(FLAGS)

sha256-main.o: sha256-main.cpp sha256.h
	$(CC) -o $@ -c $< $(FLAGS)

sha256.o: sha256.cpp sha256.h
	$(CC) -o $@ -c $< $(FLAGS)

check-passwd.o: check-passwd.cpp passwd-utils.hpp random.hpp sha256.h
	$(CC) -o $@ -c $< $(FLAGS)

gen-passwd.o: gen-passwd.cpp passwd-utils.hpp random.hpp sha256.h reduction.hpp
	$(CC) -o $@ -c $<

reduction.o: reduction.cpp reduction.hpp
	$(CC) -o $@ -c $<

clean:
	rm -rf *.o