CC=g++
CFLAGS= -std=c++11 -c -Wall
LFLAGS= -I /usr/include/boost/ -L /usr/include/boost/ –lboost_system

all: main.o serial.o app

app: main.o Serial.o
	$(CC) main.o Serial.o -o app $(LFLAGS)

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

serial.o: Serial.cpp  
	$(CC) $(CFLAGS) Serial.cpp

clean:
	rm *o app
