CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lm

TARGET = cube

all: $(TARGET)

$(TARGET): cube.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean
