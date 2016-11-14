CC=g++
CFLAGS=-c -Wall -std=c++11

all: app 

app: main.o Serial.o
	$(CC) main.o Serial.o -o app

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

serial.o: Serial.cpp  
	$(CC) $(CFLAGS) Serial.cpp

clean:
	rm *o app
