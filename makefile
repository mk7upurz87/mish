CC = gcc
CFLAGS = -ggdb -Wall -std=c99
TARGET = queueADT

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET)

