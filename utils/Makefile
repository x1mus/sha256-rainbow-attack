CC=g++
all: sha256-main check-passwd gen-passwd

sha256-main: sha256-main.o sha256.o
	$(CC) -o $@ $^

check-passwd: check-passwd.o sha256.o
	$(CC) -o $@ $^

gen-passwd: gen-passwd.o sha256.o
	$(CC) -o $@ $^

sha256-main.o: sha256-main.cpp sha256.h
	$(CC) -o $@ -c $<

sha256.o: sha256.cpp sha256.h
	$(CC) -o $@ -c $<

check-passwd.o: check-passwd.cpp passwd-utils.hpp random.hpp sha256.h
	$(CC) -o $@ -c $<

gen-passwd.o: gen-passwd.cpp passwd-utils.hpp random.hpp sha256.h
	$(CC) -o $@ -c $<
clean:
	rm -rf *.o