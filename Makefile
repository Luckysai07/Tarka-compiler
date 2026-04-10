CXX = g++
CXXFLAGS = -std=c++17 -Wall

OBJS = main.o lexer.o parser.o semantic.o codegen.o optimizer.o interpreter.o
TARGET = compiler

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).exe output.tac

.PHONY: all clean
