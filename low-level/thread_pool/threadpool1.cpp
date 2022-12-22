// Annotated and modified reference code from Code Blacksmith youtube
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <queue>
#include <thread>
#include <vector>

/* Fast Random Number Generator
   Used so that our threadpool can return some varied results.
*/
unsigned long xorshf96(void) { // period 2^96-1
  static unsigned long x = 123456789, y = 362436069, z = 521288629;
  unsigned long t;
  x ^= x << 16;
  x ^= x >> 5;
  x ^= x << 1;

  t = x;
  x = y;
  y = z;
  z = t ^ x ^ y;
  return z;
}

class ThreadPool {
public:
  using Task = std::function<void()>;

  explicit ThreadPool(std::size_t numThreads) { start(numThreads); }

  ~ThreadPool() { stop(); }

  template <class T> auto enqueue(T task) -> std::future<decltype(task())> {
    auto wrapper = std::make_shared<std::packaged_task<decltype(task())()>>(
        std::move(task));

    {
      std::unique_lock<std::mutex> lock{mEventMutex};
      mTasks.emplace(
          [=] { (*wrapper)(); }); // Add a void() functor, which calls the
                                  // packaged_task, to the task queue
    }

    mEventVar.notify_one();
    return wrapper
        ->get_future(); // Return a future to the task's return value to the
                        // caller. Note that the task has not been called yet,
                        // but the caller is holding the future ahead of time.
  }

private:
  std::vector<std::thread> mThreads;

  std::condition_variable mEventVar;

  std::mutex mEventMutex;
  bool mStopping = false;

  std::queue<Task> mTasks;

  void start(std::size_t numThreads) {
    for (auto i = 0u; i < numThreads; ++i) {
      mThreads.emplace_back([=] {
        while (true) {
          Task task;

          {
            std::unique_lock<std::mutex> lock{mEventMutex};

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
      thread.join();
  }
};

int main() {
  {
    ThreadPool pool{36};

    std::vector<std::future<float>> vf;

    for (auto i = 0; i < 36; ++i) {
      auto fut = pool.enqueue([]() -> float {
        auto f = 1000000000;
        while (f > 1)
          f /= 1.00000001;
        return float(xorshf96());
      });
      vf.push_back(std::move(fut));
    }

    for (auto &f : vf) {
      std::cout << f.get() / 1e15 << std::endl;
    }
  }

  return 0;
}
