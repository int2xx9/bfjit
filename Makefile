CC=gcc
CFLAGS=-g -Wall
TARGET=bf
SOURCES=bf.c codegen.c code.c bf_functions.c icodegen.c icode.c

default: $(TARGET)

clean:
	rm -f $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) -o $(TARGET) $(CFLAGS) -m32 -masm=intel $(SOURCES)

