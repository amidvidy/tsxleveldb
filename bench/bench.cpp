#include <iostream>
#include <cstdlib>
#include <thread>
#include <map>
#include <chrono>
#include <ctime>
#include "concurrent_counter.hpp"
#include "transactional.hpp"
#include "lock.hpp"

using namespace bench;

typedef std::chrono::high_resolution_clock timer;

long hammerArray(counter::ConcurrentCounter *arr, int nthreads, int nwrites, bool isTx) {
  std::thread threads[nthreads];

  timer::time_point start_time = timer::now();
  
  // seed prng with time
  srand((unsigned)time(nullptr));
  
  sync::ThreadState ts;
  spinlock_t ts_lock;

  for (int thread_id = 0; thread_id < nthreads; ++thread_id) {

    threads[thread_id] = std::thread([thread_id, nwrites, arr, &ts, &ts_lock]() {
	int sz = arr->size();
	for (int times = 0; times < nwrites;  ++times) {
	  int idx = rand() % sz;
	  arr->increment(idx);
	}

	 {
	   //sync::TransactionalScope xact(ts_lock, true);
	   ts += sync::TransactionalScope::getStats();
	   //sync::TransactionalScope::printStats();
	 }

      });

  }
    
  for (int i = 0; i < nthreads; ++i) {
    threads[i].join();
  }

  timer::time_point end_time = timer::now();
  if (isTx) {
    sync::TransactionalScope::printStats(ts);
  }
  return std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
  
}

int main(void) {
  std::size_t num_elements = 200;
  int nthreads = 8, nwrites = 1000000;

  // Initialize all impls for testing
  std::map<std::string, counter::ConcurrentCounter*> impls;
  //impls[std::string("nosync")] = new counter::IncorrectConcurrentCounter(num_elements);
  impls[std::string("coarse")] = new counter::CoarseConcurrentCounter(num_elements);
  impls[std::string("fine")] = new counter::CoarseConcurrentCounter(num_elements);
  impls[std::string("rtm_adaptive_coarse")] = new counter::RTMCoarseConcurrentCounter(num_elements);
  impls[std::string("rtm_adaptive_fine")] = new counter::RTMFineConcurrentCounter(num_elements);
  impls[std::string("rtm_naiive")] = new counter::RTMConcurrentCounter(num_elements);

  // run benchmarks on each array and print output
  for (auto& impl : impls) {
    auto run_time = hammerArray(impl.second, nthreads, nwrites, true);
    int total_recorded = impl.second->total();
    int total_expected = nthreads * nwrites;

    // i should really be using the std::chrono time conversions but this code is low priority
    auto avg = run_time / (double)total_recorded;

    int missing = total_recorded - total_expected;
    std::cout << "Implementation: " << impl.first << std::endl;
    std::cout << "Elapsed Time (ms): " << (double)run_time * 0.001 << std::endl;
    std::cout << "Avg (ns): " << avg * 1000 << std::endl;
    std::cout << "Total = " << total_recorded << "\tExpected = " << total_expected << std::endl;
    std::cout << ((total_recorded == total_expected) ? 
		  "All writes were recorded." : 
                     ((missing<0) ?
		      (std::string("There were ") + std::to_string(-missing) + std::string(" missing writes!")) :
		      (std::string("There were ") + std::to_string(missing) + std::string(" writes that should NOT have occured. This should never happen.")))) << std::endl;
  }


}
