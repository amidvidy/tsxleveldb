#include <iostream>
#include <cstdlib>
#include <thread>
#include <map>
#include <chrono>
#include <ctime>
#include "concurrent_counter.hpp"

typedef std::chrono::high_resolution_clock timer;

long hammerArray(ConcurrentCounter *arr, int nthreads, int nwrites) {
  std::thread threads[nthreads];

  timer::time_point start_time = timer::now();
  
  // seed prng with time
  srand((unsigned)time(nullptr));

  for (int thread_id = 0; thread_id < nthreads; ++thread_id) {

    threads[thread_id] = std::thread([thread_id, nwrites, arr]() {
	int sz = arr->size();
	for (int times = 0; times < nwrites;  ++times) {
	  int idx = rand() % sz;
	  arr->increment(idx);
	}

    });
    
  }

  for (int i = 0; i < nthreads; ++i) {
    threads[i].join();
  }

  timer::time_point end_time = timer::now();
  return std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
  
}

int main(void) {
  std::size_t num_elements = 100;
  int nthreads = 30, nwrites = 1000000;

  // Initialize all impls for testing
  std::map<std::string, ConcurrentCounter*> impls;
  impls[std::string("incorrect")] = new IncorrectConcurrentCounter(num_elements);
  impls[std::string("coarse")] = new CoarseConcurrentCounter(num_elements);
  impls[std::string("fine")] = new CoarseConcurrentCounter(num_elements);

  // run benchmarks on each array and print output
  for (auto& impl : impls) {
    auto run_time = hammerArray(impl.second, nthreads, nwrites);
    int total_recorded = impl.second->total();
    int total_expected = nthreads * nwrites;
    std::cout << "Implementation: " << impl.first << std::endl;
    std::cout << "Elapsed Time (micros): " << run_time << std::endl;
    std::cout << "Total = " << total_recorded << "\tExpected = " << total_expected << std::endl;
    if (total_recorded == total_expected) {
      std::cout << "All Writes Recorded" << std::endl;
    } else {
      std::cout << "Missing Writes!" << std::endl;
    }

  }

}
