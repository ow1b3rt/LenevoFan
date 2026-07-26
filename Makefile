CC      := gcc
CFLAGS  := -Wall -Wextra -O2 $(shell pkg-config --cflags sdl2)
LDFLAGS := $(shell pkg-config --libs sdl2)

TARGET  := fanctl
SRC     := fanctl.c

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

run: $(TARGET)
	sudo ./$(TARGET)

clean:
	rm -f $(TARGET)