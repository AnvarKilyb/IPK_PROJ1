CC = g++
CFLAGS = -g -Wall
TARGET = server

all: $(TARGET)

$(TARGET) : $(TARGET).cpp
			$(CC) $(CFLAGS) -o hinfosvc $(TARGET).cpp