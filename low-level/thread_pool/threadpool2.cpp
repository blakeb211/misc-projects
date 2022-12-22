/* My implementation and demo of a threadpool.
 * Features: Create pool with arbitrary number of threads.
 *           Submit tasks with arbitrary return type.
 *           Submit tasks with arbitrary parameters using std::bind.
 * Limitations:
 *           No way to stop in-progress tasks.
 */
#include <cmath>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <queue>
#include <thread>

/* Mutex protects:
    - stop flag
    - task queue
*/

class TPool {
public:
  using Task = std::function<void()>;
  explicit TPool(size_t _nthread) { start(_nthread); }
  ~TPool() { stop(); }
  // packaged_task has two features: a call operator, and a future for the the
  // result
  template <typename T>
  auto submitTask(T _task) -> std::future<decltype(_task())> {
    // decltype(_task())()
    //        ^          ^
    // ---------------   -------
    // _task return type  call operator
    //  e.g. if task is a void() this produces void()
    auto pkgTaskPtr =
        std::make_shared<std::packaged_task<decltype(_task())()>>(_task);
    {
      std::unique_lock<std::mutex> _locked{mLock};
      // Push a void() onto the task queue.
      mTasks.push([=] { (*pkgTaskPtr)(); });
    }

    // Notify one thread to check for work on the task queue.
    mCond.notify_one();
    return pkgTaskPtr->get_future();
  };

private:
  std::vector<std::thread> mThreads;
  std::queue<Task> mTasks;
  std::mutex mLock;
  std::condition_variable mCond;
  bool mStopFlag{false};
  auto start(size_t _nthread) -> void {
    while (_nthread > 0) {
      _nthread--;
      mThreads.emplace_back(std::thread{[=] {
        while (true) {
          Task _task;
          bool taskWasGrabbed{false};
          {
            std::unique_lock<std::mutex> _locked{mLock};
            mCond.wait(_locked, [=] { return mStopFlag || !mTasks.empty(); });
            
            if (!mTasks.empty()) {
              _task = mTasks.front();
              mTasks.pop();
              taskWasGrabbed = true;
            }
            if (mStopFlag && mTasks.empty() && !taskWasGrabbed) {
              break;
            }
          }
          if (taskWasGrabbed)
            _task();
        }
      }});
    }
  };
  auto stop() -> void {
    {
      std::unique_lock<std::mutex> _locked{mLock};
      mStopFlag = true;
    }

    // Notify all threads to check the Stop flag
    mCond.notify_all();

    // Must release lock to join the threads or else there's a deadlock when the
    // workerLambda trys to get the mutex.
    for (auto &t : mThreads) {
      t.join();
    }
  };
};

auto main() -> int {
  auto pool = TPool{10};

  auto lam = [](int i) {
    std::vector<int> v(100);
    // Fake work
    for (int i = 0u; i < 1e8; i++)
      auto res = std::sqrt(std::pow(0.2, 2e7));
    // Return string result
    auto threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
    return std::string("Task completed by thread " +
                       std::to_string(threadId).substr(3, 8));
  };

  // Use std::bind to create a functor that takes no parameters, since
  // threadpool only works on tasks that take zero parameters.
  auto functor = std::bind(lam, 1);

  // Save task results in a vector of futures
  auto fuVec = std::vector<std::future<decltype(functor())>>();
  for (int i = 0; i < 10; i++)
    fuVec.push_back(pool.submitTask(functor));

  // Print out return values of tasks
  for (auto &f : fuVec)
    std::cout << f.get() << std::endl;
  return 0;
}
