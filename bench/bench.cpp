#include <iostream>
#include <string>
#include <thread>
#include <iomanip>
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
  
  log("Starting Benchmark!");

  timer::time_point start_time = timer::now();

  for (int thread_id = 0; thread_id < nthreads; ++thread_id) {
    threads[thread_id] = thread([thread_id, nkeys, &tstarts, &tends, db]() {
	tstarts[thread_id] = timer::now();
	for (int times = 0; times < nkeys; ++times) {
	  string gets =  db->get(to_string(thread_id + times));
	  db->put(to_string(thread_id + times), to_string(thread_id) + gets);
	}
        tends[thread_id] = timer::now();
    });
  }

  for (int i = 0; i < nthreads; ++i) {
    threads[i].join();
  }
  
  auto end_time = timer::now();

  auto timeMicros = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();
  
  log("Finishing Benchmark!");
  log(string("Total Duration: ")+to_string(timeMicros)+string(" us"));

  auto DELIM = setw(20);
  
  cout << "tid" << DELIM;
  cout << "start time(us)" << DELIM;
  cout << "end time(us)" << DELIM;
  cout << "duration(ms)" << DELIM;
  cout << "avg per write(us)" << endl;

  for (int i = 0; i < nthreads; ++i) {

    auto duration = chrono::duration_cast<chrono::milliseconds>(tends[i] - tstarts[i]).count();
    auto tstart = chrono::duration_cast<chrono::microseconds>(tstarts[i] - start_time).count();
    auto tend = chrono::duration_cast<chrono::microseconds>(tends[i] - start_time).count();
    auto avg = chrono::duration_cast<chrono::microseconds>(tends[i] - tstarts[i]).count() / nkeys;

    cout << i << DELIM;

    cout << tstart << DELIM;
    cout << tend << DELIM;
    cout << duration << DELIM;
    cout << avg << endl;
  }
}

void timeStats(timer::time_point *start_times, timer::time_point *end_times, int nthreads) {
}

void dbStats(DB *db, int nthreads) {
  cout << "DB size = " << db->size() << endl;
  // TODO refactor
  CoarseGrainedDB *db2 = static_cast<CoarseGrainedDB*>(db);
  long waitTime = db2->waitTime();
  long lockCount = db2->getLockCount();
  cout << "Time spent waiting to acquire lock (total) = " << waitTime << " us" << endl;
  cout << "Total lock acquisitions = " << lockCount << endl;
  cout << "Time spnt waiting to acquire lock (average) = " << waitTime / (double)lockCount << " us" << endl;
  cout << "Waiting time per thread = " << waitTime / nthreads << " us" << endl;
}

int main(void) {
  DB *db = new CoarseGrainedDB;
  int nthreads = 8;
  hammerDB(db, nthreads, 10000);
  dbStats(db, nthreads);
  return 0;
}



