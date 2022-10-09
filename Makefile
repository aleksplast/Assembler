CC := g++ -Wno-pointer-arith
SRC := main.cpp assembler.cpp stack.cpp text-sort.cpp
DIR := C:\Users\USER\Documents\assembler

.PHONY: all clean

main: $(SRC)
	$(CC) $^ -o $(DIR)\$@

test: $(SRC_TEST)
	$(CC) $^ -o $(DIR)\$@

run: $(DIR)\main.exe
	$(DIR)\main.exe

