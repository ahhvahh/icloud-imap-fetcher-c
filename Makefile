CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2 -Iinclude
LDFLAGS ?= -lcurl
TARGET := bin/icloud-imap-fetcher-c
SRC := $(wildcard src/*.c)

all: $(TARGET)

$(TARGET): $(SRC)
	@mkdir -p bin
	$(CC) $(CFLAGS) $(SRC) -o $@ $(LDFLAGS)

clean:
	rm -rf bin build

.PHONY: all clean
