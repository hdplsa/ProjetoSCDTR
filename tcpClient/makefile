CC=g++
CFLAGS=-std=c++11 -c -g -Wall
LFLAGS=-lboost_system -lboost_thread -lboost_chrono -lpthread

all: main.o client.o app

app: main.o client.o
	$(CC) main.o client.o -o app $(LFLAGS)

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

client.o: client.cpp
	$(CC) $(CFLAGS) client.cpp

clean:
	rm *o app
