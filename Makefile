CC = g++

TARGET = rtree
OBJECTS = $(patsubst %.cpp,%.o,$(wildcard *.cpp))

# CFLAGS = -std=c++11 -Wall -Ofast
CFLAGS = -Wall -g -Wextra -pedantic -std=c++11  -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -Wshift-overflow=2 -Wduplicated-cond -Wcast-qual -Wcast-align

$(TARGET): $(OBJECTS)
	$(CC) $^ -o $(TARGET)

$(OBJECTS): %.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

clean: 
	rm -f *.o
	rm -f rtree

.PHONY: clean