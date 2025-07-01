CXX = g++
CXXFLAGS = -Wall -pthread -std=c++11

# Source files
SRCS = LockManager.cpp rag.cpp Logger.cpp 2PLManager.cpp TreeLockManager.cpp treelock_test.cpp
OBJS = $(SRCS:.cpp=.o)

# Output executable
TARGET = lock_test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) lock_manager.log

