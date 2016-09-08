vpath %.h ./include
vpath %.c ./src
vpath %.cpp ./src

CPPFLAGS=-std=c++0x -I ../include

test1: ../scan_dir.o test1.o
	g++ -o $@ $^ $(CPPFLAGS)
%.o : %.c
	g++ $@ $< $(CPPFLAGS)

.PHONY: clean
clean:
	rm test1.o test1
