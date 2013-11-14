#include <mutex>

class TransactionalScope {
public:
  TransactionalScope(std::mutex *fallback);
  ~TransactionalScope();
private:
  std::mutex *mutex;
  int lockstate;
};
