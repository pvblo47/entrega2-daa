CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Wpedantic

SRCS_TEST      := linea_base.cpp ExtractorConos.cpp ModeloBloques.cpp Reporte.cpp GeneradorInstancias.cpp
SRCS_BENCHMARK := benchmark.cpp ExtractorConos.cpp ModeloBloques.cpp Reporte.cpp GeneradorInstancias.cpp

TARGET_TEST      := linea_base.exe
TARGET_BENCHMARK := benchmark.exe

.PHONY: all clean run run-benchmark

all: $(TARGET_TEST) $(TARGET_BENCHMARK)

$(TARGET_TEST): $(SRCS_TEST)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TARGET_BENCHMARK): $(SRCS_BENCHMARK)
	$(CXX) $(CXXFLAGS) -o $@ $^

run: $(TARGET_TEST)
	./$(TARGET_TEST)

run-benchmark: $(TARGET_BENCHMARK)
	./$(TARGET_BENCHMARK)

clean:
	rm -f $(TARGET_TEST) $(TARGET_BENCHMARK)
