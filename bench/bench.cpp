#include <iostream>
#include <string>
#include <thread>
#include "db.hpp"

void hammerDB(CoarseGrainedDB *db, int nthreads, int nkeys) {
  std::thread threads[nthreads];

  
  
  std::cout << "Starting Benchmark!" << std::endl;
  auto start_time = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < nthreads; ++i) {
    threads[i] = std::thread([=]() {
	std::string thread_name = "foo";
	for (int times = 0; times < nkeys; ++times) {
	  db->put("foo", thread_name);
	}
    });
  }
  
  for (int i = 0; i < nthreads; ++i) {
    threads[i].join();
  }
  auto end_time = std::chrono::high_resolution_clock::now();

  
  std::cout << "Finishing Benchmark!" << std::endl;
  std::cout << "Total Duration: " << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() << " us" << std::endl;
  
}

int main(void) {
  CoarseGrainedDB *db = new CoarseGrainedDB;
  hammerDB(db, 100, 100000);
  std::cout << db->get(std::string("foo")) <<  std::endl;
  return 0;
}



