CXX = g++
CXXFLAGS = -g -Wall -std=c++11 -pthread

OBJ = ./main
LIB = lib/libredis++.a /usr/local/lib/libhiredis.a -lprometheus-cpp-pull -lprometheus-cpp-core -lz

DAG_FILES = dag_scheduler/task.cpp dag_scheduler/executor.cpp dag_scheduler/scheduler.cpp
LOG_FILES = log_service/controler.cpp
THREAD_POOL_FILES = thread_pool/thread_pool.cpp

main:
	$(CXX) $(CXXFLAGS) -I ./ -o $(OBJ) main.cpp $(DAG_FILES) $(LOG_FILES) $(THREAD_POOL_FILES) $(LIB)
   