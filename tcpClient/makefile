CC=g++
CFLAGS=-std=c++11 -c -g -Wall
LFLAGS=-lboost_system -lboost_thread -lpthread

all: main.o tcpClient.o app

app: main.o tcpClient.o
	$(CC) main.o tcpClient.o -o app $(LFLAGS)

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

tcpClient.o: tcpClient.cpp
	$(CC) $(CFLAGS) tcpClient.cpp

clean:
	rm *o app
