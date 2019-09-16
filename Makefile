BUILD   = linux-build/
TARGET  = $(BUILD)dotest

CPP     = g++
CFLAGS  = -Wall -O0 -g -c -gdwarf-2 -std=c++0x -Wno-deprecated -D _SYSCALL -D _TEST_MEM_

LIB	= -lprofiler -ltcmalloc

SRC     = $(wildcard *.cpp)
OBJ     = $(SRC:.cpp=.o)
OBJP    = $(join $(addsuffix $(BUILD), $(dir $(SRC))), $(notdir $(SRC:.cpp=.o)))

.cpp.o:
	$(CPP) $(CFLAGS) -o $(BUILD)$*.o $*.cpp

$(TARGET):$(OBJ)
	$(CPP) -o $(TARGET) $(OBJP) $(LIB) $(MLIB)

all:$(TARGET)

clean:
	rm -rf $(BUILD)*.o
	rm -rf $(TARGET)
