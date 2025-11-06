# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -g
LDFLAGS = -pthread
LDLIBS = -lboost_system -lboost_thread

# Source files
SERVER_SRC = chatRoom.cpp
CLIENT_SRC = client.cpp

# Object files (automatically generated from source files)
SERVER_OBJ = $(SERVER_SRC:.cpp=.o)
CLIENT_OBJ = $(CLIENT_SRC:.cpp=.o)

# Default target: build both server and client
all: chatApp clientApp

# Build the chat server application
chatApp: $(SERVER_OBJ)
	$(CXX) $(LDFLAGS) $(SERVER_OBJ) $(LDLIBS) -o chatApp

# Build the client application
clientApp: $(CLIENT_OBJ)
	$(CXX) $(LDFLAGS) $(CLIENT_OBJ) $(LDLIBS) -o clientApp

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f *.o chatApp clientApp

