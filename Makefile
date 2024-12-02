CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -g

# List of object files
OBJS_MAIN = main.o
OBJS_TESTER = tester.o

# Output executables
MAIN_EXEC = main
TESTER_EXEC = tester

# Default target (build both executables)
all: $(MAIN_EXEC) $(TESTER_EXEC)

# Rule for main executable
$(MAIN_EXEC): $(OBJS_MAIN)
	$(CXX) $(OBJS_MAIN) -o $(MAIN_EXEC)

# Rule for tester executable
$(TESTER_EXEC): $(OBJS_TESTER)
	$(CXX) $(OBJS_TESTER) -o $(TESTER_EXEC)

# Rule for compiling main.o
main.o: main.cpp Wordle.h
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

# Rule for compiling tester.o
tester.o: tester.cpp Wordle.h
	$(CXX) $(CXXFLAGS) -c tester.cpp -o tester.o

# Clean up build artifac
clean:
	rm -f $(OBJS_MAIN) $(OBJS_TESTER) $(MAIN_EXEC) $(TESTER_EXEC)