vpath %.h ./include
vpath %.c ./src
vpath %.cpp ./src

objects = scan_dir.o
CPPFLAGS = -std=c++0x -I ./include

.PHONY: all
all: $(objects)
	
%.o: %.cpp %.c
	g++ -o $@ -c $^ $(CPPFLAGS)


.PHONY: clean
clean:
	rm *.o
