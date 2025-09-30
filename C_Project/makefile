CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -g

TARGET := tokenizer

SRCS := main.cpp CommentRemover.cpp Tokenizer.cpp ErrorHandler.cpp TokenStream.cpp Parser.cpp CSTNode.cpp SymbolTable.cpp
OBJS := $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) errors.txt

run: $(TARGET)
	: > errors.txt
	./$(TARGET)

debug: CXXFLAGS += -DDEBUG
debug: clean all

.PHONY: all clean run debug
