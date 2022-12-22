// Threadpool0 - simple threadpool, only handles void() tasks,
// cannot be stopped before the tasks are completed.
#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
public:
  using Task = std::function<void()>;
  explicit ThreadPool(std::size_t numThreads) { start(numThreads); }

  ~ThreadPool() {
    std::cout << "ThreadPool destructor called" << std::endl;
    stop();
  }

  void enqueue(Task task) {
    {
      std::unique_lock<std::mutex> lock{mEventMutex};
      mTasks.emplace(std::move(task));
    }
    mEventVar.notify_one();
  }

private:
  std::vector<std::thread> mThreads;

  std::condition_variable mEventVar;

  std::mutex mEventMutex;
  bool mStopping = false;
  std::queue<Task> mTasks;

  void start(std::size_t numThreads) {
    for (auto i = 0u; i < numThreads; i++) {
      mThreads.emplace_back([=] {
        while (true) {
          Task task;
          {
            std::unique_lock<std::mutex> lock{mEventMutex};

            // wait, without burning up CPU, periodically checking if the
            // predicate is true
            mEventVar.wait(lock, [=] { return mStopping || !mTasks.empty(); });

            if (mStopping && mTasks.empty())
              break;

            task = std::move(mTasks.front());
            mTasks.pop();
          }
          task();
        }
      });
    }
  }
  void stop() noexcept {
    {
      std::unique_lock<std::mutex> lock{mEventMutex};
      mStopping = true;
    }
    mEventVar.notify_all();
    for (auto &thread : mThreads)
      // join() waits for all threads to finish
      thread.join();
  }
};

int main(int argc, char **argv) {
  {
    using namespace std::chrono_literals;
    ThreadPool pool{(size_t)std::atoi(argv[1])};
  auto taskCount = 5;
    while (taskCount > 0) {
      taskCount--;
      pool.enqueue([] {
        std::cout << std::this_thread::get_id() << " running\n";
        auto f = 1000'000'000;
        while (f > 1) {
          f = f / 1.000000001;
        }
        std::cout << std::this_thread::get_id() << " done with work\n";
      });
    }
  }
  return 0;
}
