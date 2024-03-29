### Test MakeFile for experimenting ###
CC = gcc
TARGET = tracer

CFLAGS = -g -Wall

.PHONY: default all clean

default: $(TARGET)
all: default

LIBS = -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS) tracer.c
	$(CC) tracer.c -Wall $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)
