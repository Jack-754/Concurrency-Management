CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
OBJECTS = main.o Logger.o LockManager.o TransactionManager.o RAG.o
TREE_OBJECT = TreeProtocol.o

TARGET = concurrency_manager

# If tree protocol is used, uncomment the line below
# OBJECTS += $(TREE_OBJECT)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

Logger.o: Logger.cpp Logger.h
	$(CXX) $(CXXFLAGS) -c Logger.cpp

LockManager.o: LockManager.cpp LockManager.h
	$(CXX) $(CXXFLAGS) -c LockManager.cpp

TransactionManager.o: TransactionManager.cpp TransactionManager.h
	$(CXX) $(CXXFLAGS) -c TransactionManager.cpp

RAG.o: RAG.cpp RAG.h
	$(CXX) $(CXXFLAGS) -c RAG.cpp

TreeProtocol.o: TreeProtocol.cpp TreeProtocol.h
	$(CXX) $(CXXFLAGS) -c TreeProtocol.cpp

clean:
	rm -f *.o $(TARGET)
	rm -f logs/operations.log
