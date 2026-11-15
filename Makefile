# Makefile for Mini Git (C++)
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra

SRCS = main.cpp MiniGit.cpp FileManager.cpp Version.cpp VersionHistory.cpp UndoRedo.cpp Compare.cpp Search.cpp
TARGET = minigit

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)
