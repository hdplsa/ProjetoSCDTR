CC=g++
CFLAGS=-std=c++11 -c -Wall
LFLAGS=-lboost_system -lboost_thread -lpthread

all: main.o serial.o tcpServer.o app

app: main.o Serial.o tcpServer.o
	$(CC) main.o Serial.o tcpServer.o -o app $(LFLAGS)

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

serial.o: Serial.cpp  
	$(CC) $(CFLAGS) Serial.cpp

tcpServer.o: tcpServer.cpp
	$(CC) $(CFLAGS) tcpServer.cpp

clean:
	rm *o app
