CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic
INCLUDES := -Iinclude -Ilinea_base

SRCDIR   := src
LBDIR    := linea_base

SRCS     := $(wildcard $(SRCDIR)/*.cpp)
LBSRCS   := $(LBDIR)/ModeloBloques.cpp \
             $(LBDIR)/ExtractorConos.cpp \
             $(LBDIR)/GeneradorInstancias.cpp \
             $(LBDIR)/Reporte.cpp

OBJS     := $(patsubst $(SRCDIR)/%.cpp, build/%.o, $(SRCS))
LBOBJS   := $(patsubst $(LBDIR)/%.cpp, build/lb_%.o, $(LBSRCS))

TARGET   := build/extraccion_dag

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS) $(LBOBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

build/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

build/lb_%.o: $(LBDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -f build/*.o build/extraccion_dag