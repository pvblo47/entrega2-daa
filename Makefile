CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic
INCLUDES := -Iinclude -Ilinea_base

SRCDIR   := src
SRCDIR := src
SRCS   := $(wildcard $(SRCDIR)/*.cpp) linea_base/ModeloBloques.cpp linea_base/ExtractorConos.cpp
OBJS   := build/main.o build/GrafoDAG.o build/OperacionesCono.o build/Extraccion.o build/Instancia.o build/ModeloBloques.o build/ExtractorConos.o
TARGET := build/extraccion_dag

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

build/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

build/%.o: linea_base/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -f build/*.o build/extraccion_dag