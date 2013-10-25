#include <iostream>
#include <string>
#include <thread>
#include "db.hpp"

void hammerDB(CoarseGrainedDB *db, int nthreads, int nkeys) {
  std::thread threads[nthreads];
  
  std::cout << "Starting Benchmark!" << std::endl;

  for (int i = 0; i < nthreads; ++i) {
    threads[i] = std::thread([=]() {
	for (int times = 0; times < nkeys; ++times) {
	  db->put("foo", "bar");
	}
    });
  }
  
  for (int i = 0; i < nthreads; ++i) {
    threads[i].join();
    std::cout << "joined a thread!" << std::endl;
  }
  
  std::cout << "Finishing Benchmark!" << std::endl;
}

int main(void) {
  CoarseGrainedDB *db = new CoarseGrainedDB;
  db->put(std::string("adam"), std::string("midvidy"));
  hammerDB(db, 400, 10000);
  std::cout << "fooooooo!!!" << std::endl;
  std::cout << db->get(std::string("foo")) << std::endl;
  return 0;
}



