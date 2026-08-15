
UNAME_S := $(shell uname -s)

CXX = g++

CXXFLAGS = `sdl2-config --cflags` -Wall -ggdb3 -O0
LDFLAGS  = `sdl2-config --libs` -lSDL2_image -lSDL2_gfx -lm

ifeq ($(UNAME_S), Darwin)

    BREW_PREFIX := $(shell brew --prefix 2>/dev/null || echo /opt/homebrew)
    CXXFLAGS += -Xpreprocessor -fopenmp -I$(BREW_PREFIX)/opt/libomp/include
    LDFLAGS  += -L$(BREW_PREFIX)/opt/libomp/lib -lomp
else ifeq ($(UNAME_S), Linux)

    CXXFLAGS += -fopenmp
    LDFLAGS  += -fopenmp
endif

TARGET = main
OBJS   = main.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean