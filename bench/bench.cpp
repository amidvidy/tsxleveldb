#include <iostream>
#include <string>
#include <thread>
#include <ctime>
#include "db.hpp"

using namespace std;

 void log(std::string msg) {
  cout << msg << endl;
}

typedef chrono::high_resolution_clock timer;

void hammerDB(DB *db, int nthreads, int nkeys) {
  thread threads[nthreads];
  
  timer::time_point tstarts[nthreads];
  timer::time_point tends[nthreads];
  
  cout << "Starting Benchmark!" << endl;
  timer::time_point start_time = timer::now();

  for (int thread_id = 0; thread_id < nthreads; ++thread_id) {
    threads[thread_id] = thread([thread_id, nkeys, &tstarts, &tends, db]() {
	tstarts[thread_id] = timer::now();
	for (int times = 0; times < nkeys; ++times) {
	  db->put(to_string(thread_id + times), to_string(thread_id));
	}
        tends[thread_id] = timer::now();
    });
  }

  for (int i = 0; i < nthreads; ++i) {
    threads[i].join();
  }
  
  auto end_time = timer::now();

  auto timeMicros = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();
  
  cout << "Finishing Benchmark!" << endl;
  cout << "Total Duration: " << timeMicros << " us" << endl;

  string DELIM = "\t";
  
  cout << "thread_id" << DELIM << "start_time(us)" << DELIM << "end_time(us)" << DELIM << "duration(ms)" << endl;

  for (int i = 0; i < nthreads; ++i) {

    auto duration = chrono::duration_cast<chrono::milliseconds>(tends[i] - tstarts[i]).count();
    auto tstart = chrono::duration_cast<chrono::microseconds>(tstarts[i] - start_time).count();
    auto tend = chrono::duration_cast<chrono::microseconds>(tends[i] - start_time).count();

    cout << i << DELIM << DELIM;
    cout << tstart << DELIM << DELIM;
    cout << tend << DELIM << DELIM;
    cout << duration << endl;
  }
}

void timeStats(timer::time_point *start_times, timer::time_point *end_times, int nthreads) {
}

void dbStats(DB *db) {
  cout << "DB size = " << db->size() << endl;
}

int main(void) {
  DB *db = new CoarseGrainedDB;
  hammerDB(db, 8, 100000);
  dbStats(db);
  return 0;
}



