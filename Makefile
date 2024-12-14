CC = gcc
CFLAGS = -Wall -O2
TARGET = scan
INSTALL_DIR = $(HOME)/.local/bin

.PHONY: all clean install

all: $(TARGET)

$(TARGET): scan.c
	$(CC) $(CFLAGS) -o $@ $^

install: $(TARGET)
	mkdir -p $(INSTALL_DIR)
	install -m 755 $(TARGET) $(INSTALL_DIR)

clean:
	rm -f $(TARGET)
