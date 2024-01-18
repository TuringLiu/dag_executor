#include "dag_scheduler/scheduler.h"
#include "dag_scheduler/task.h"

#include <iostream>
#include <thread>

#include <array>
#include <chrono>
#include <cstdlib>
#include <memory>
#include <string>
#include <thread>
#include <unistd.h>
#include <algorithm>

#include "log_service/log_sender.h"

#include <functional>

int main()
{
  LocalSender metrics_local;
  for(int i = 0; i < 101; i++)
  {
    metrics_local.send("test", {{"method", "GET"}, {"abc", "123"}}, i);
  }
  

  // auto tp = std::chrono::system_clock::now();
  // auto ttp = std::chrono::high_resolution_clock::now();
  // auto tp1 = std::chrono::steady_clock::now();
  // std::time_t tt = std::chrono::system_clock::to_time_t(tp);
  // std::cout << tt << "seconds from 1970-01-01 00:00:00 UTC" << std::endl;
  // std::cout << tp.time_since_epoch().count() << std::endl;
  // std::cout << ttp.time_since_epoch().count() << std::endl;
  // std::cout << tp1.time_since_epoch().count() << std::endl;

  return 0;
}

// int main() {
//   using namespace prometheus;

//   Exposer exposer{"127.0.0.1:8080"};
//   auto registry = std::make_shared<Registry>();

//   // add a new counter family to the registry (families combine values with the
//   // same name, but distinct label dimensions)
//   //
//   // @note please follow the metric-naming best-practices:
//   // https://prometheus.io/docs/practices/naming/
//   auto& packet_counter = BuildHistogram()
//                              .Name("Test_66")
//                              .Help("No description")
//                              .Register(*registry);
//   auto& udp_tx_counter =
//       packet_counter.Add({{"protocol", "udp"}, {"direction", "tx"}},  Histogram::BucketBoundaries{0, 1, 2});

//   exposer.RegisterCollectable(registry);

// 	ThreadPool thread_pool(4, 100);
// 	for(int i = 0; i < 10; i++)
// 	{
// 		thread_pool.add_task([i, &udp_tx_counter](){
// 			for (;;) {
// 				// std::this_thread::sleep_for(std::chrono::seconds(1));
// 				const auto random_value = std::rand();
// 				// std::cout << (random_value & 1) << std::endl;
// 				udp_tx_counter.Observe(i);
// 			}
// 		});
// 	}
// 	sleep(200);
//   	return 0;
// }

// int main() {
// 	auto start = std::chrono::high_resolution_clock::now();

// 	PrometheusControler prometheus_controler("127.0.0.1:8080");

// 	ThreadPool thread_pool(4, 100);
// 	for(int i = 0; i < 1; i++)
// 	{
// 		thread_pool.add_task([&](){
// 			while(1) {
// 				std::this_thread::sleep_for(std::chrono::seconds(1));
// 				const auto random_value = std::rand();

// 				if (random_value & 1) prometheus_controler.produce_("test", {{"method", "GET"}}, 1);
// 				if (random_value & 2) prometheus_controler.produce_("test", {{"method", "GET"}}, 2);
// 				if (random_value & 4) prometheus_controler.produce_("test", {{"method", "GET"}}, 4);
// 				if (random_value & 8) prometheus_controler.produce_("test", {{"method", "GET"}}, 8);
// 			}
// 		});
// 	}
// 	sleep(20);
// 	auto end = std::chrono::high_resolution_clock::now();    //结束时间

// 	long long duration = (end - start).count();
//     std::cout << "程序运行时间：" << duration / 1000000000.0 << "s"
//         << "；  " << duration / 1000000.0 << "ms" 
//         << "；  " << duration / 1000.0 << "us"
//         << std::endl;
    
// 	return 0;
// }


// int main()
// {
    
//     std::cout << "this is a test!" << std::endl;

//     TaskPtr task = std::make_shared<Task>([](){ std::cout << "this is a test for lamda function! 1" << std::endl;});
//     TaskPtr task_2 = std::make_shared<Task>([](){ std::cout << "this is a test for lamda function! 2" << std::endl;});
//     TaskPtr task_3 = std::make_shared<Task>([](){ std::cout << "this is a test for lamda function! 3" << std::endl;});
//     TaskPtr task_4 = std::make_shared<Task>([](){ std::cout << "this is a test for lamda function! 4" << std::endl;});

//     TaskPtr task_5 = std::make_shared<Task>([](){ std::cout << "this is a test for lamda function! 5" << std::endl;});
//     TaskPtr task_6 = std::make_shared<Task>([](){ std::cout << "this is a test for lamda function! 6" << std::endl;});


//     Scheduler dag;
//     dag.add_task(task);
//     dag.add_task(task_2);
//     dag.add_task(task_3);
//     dag.add_task(task_4);
//     dag.add_task(task_5);
//     dag.add_task(task_6);

//     task->set_precede(task_2);
//     task_2->set_precede(task_3);
//     task_3->set_precede(task_4);

//     task->set_precede(task_6);
//     task_6->set_precede(task_5);
//     task_5->set_precede(task_4);

//     dag.execute();

//     return 0;
// }