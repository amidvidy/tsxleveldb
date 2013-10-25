#include <iostream>
#include <string>
#include <thread>
#include "db.hpp"

using namespace std;

 void log(std::string msg) {
  cout << msg << endl;
}

void hammerDB(DB *db, int nthreads, int nkeys) {
  thread threads[nthreads];
  
  cout << "Starting Benchmark!" << endl;
  auto start_time = chrono::high_resolution_clock::now();

  for (int i = 0; i < nthreads; ++i) {
    threads[i] = thread([=]() {
	for (int times = 0; times < nkeys; ++times) {
	  db->put(to_string(i), to_string(times));
	}
    });
  }

  for (int i = 0; i < nthreads; ++i) {
    threads[i].join();
  }
  
  auto end_time = chrono::high_resolution_clock::now();

  auto timeMicros = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();

  cout << "Finishing Benchmark!" << endl;
  cout << "Total Duration: " << timeMicros << " us" << endl;
  
}

int main(void) {
  DB *db = new CoarseGrainedDB;
  hammerDB(db, 20, 1000);
  cout << db->get(string("foo")) <<  endl;
  return 0;
}



