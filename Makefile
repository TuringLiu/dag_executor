main:
	g++ -g -pthread -I ./ -o main main.cpp dag_scheduler/task.cpp dag_scheduler/executor.cpp  dag_scheduler/scheduler.cpp thread_pool/thread_pool.cpp lib/libredis++.a /usr/local/lib/libhiredis.a