CC=g++
CFLAGS=-std=c++11 -c -g -Wall
LFLAGS=-lboost_system -lboost_thread -lpthread

all: main.o mainController.o Arduino.o tcpServer.o Serial.o app

app: main.o mainController.o Arduino.o tcpServer.o Serial.o
	$(CC) main.o mainController.o Arduino.o tcpServer.o Serial.o -o app $(LFLAGS)

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

mainController.o: mainController.cpp
	$(CC) $(CFLAGS) mainController.cpp

Arduino.o: Arduino.cpp
	$(CC) $(CFLAGS) Arduino.cpp

Serial.o: Serial.cpp  
	$(CC) $(CFLAGS) Serial.cpp

tcpServer.o: tcpServer.cpp
	$(CC) $(CFLAGS) tcpServer.cpp

clean:
	rm *o app
