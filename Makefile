CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic
INCLUDES := -Iinclude -Isrc/linea_base

DAGDIR := src/extraccionDAG
LBDIR  := src/linea_base

OBJS := build/main.o build/GrafoDAG.o build/OperacionesCono.o \
        build/Extraccion.o build/Instancia.o \
        build/ModeloBloques.o build/ExtractorConos.o

TARGET := build/extraccion_dag

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

build/%.o: $(DAGDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

build/%.o: $(LBDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -f build/*.o build/extraccion_dag