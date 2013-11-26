BASE_LEVELDB=/home/jtsui/leveldb
BASE_DAEMON=/home/jtsui/leveldb-daemon

INPUT_WORKLOAD=tsxa

cp $BASE_LEVELDB/libleveldb.a /usr/local/lib/
cp -r $BASE_LEVELDB/include/leveldb /usr/local/include/
cd $BASE_DAEMON/simpleleveldb
make && sudo make install
cd $BASE_DAEMON/simpleleveldb
rm -rf tmp
./simpleleveldb --db-file=tmp

./ycsb load leveldb -P workloads/$INPUT_WORKLOAD -s -p measurementtype=timeseries -p timeseries.granularity=10000 > load.log
./ycsb run leveldb -P workloads/$INPUT_WORKLOAD -s -p measurementtype=timeseries -p timeseries.granularity=10000 > run.log
