/*
 * Executor.h
 *
 *  Created on: 2016-05-08
 *      Author: Anton Kochnev
 */

#ifndef EXECUTOR_H_
#define EXECUTOR_H_

#include <atomic>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace cppcore
{
namespace thread
{
  class Executor
  {
  public:
    typedef std::function<void()> Task;

  public:
    Executor(std::size_t thread_count = 1)
    :
      terminated_(false)
    {
      for (std::size_t i = 0; i < thread_count; ++i)
      {
        pool_.emplace_back(std::bind(&Executor::executor_thread, this));
      }
    }

    Executor(const Executor&) = delete;
    Executor& operator= (const Executor&) = delete;

    ~Executor()
    {
      finish();
    }

    void finish()
    {
      terminate();
      join();
    }

    void join()
    {
      for (std::thread& t : pool_)
      {
        t.join();
      }
    }

    void submit(const Task& task)
    {
      std::unique_lock<std::mutex> lk(mutex_);

      if (!terminated_)
      {
        ++task_count_;
        task_queue_.push(task);
        cond_.notify_one();
      }
    }

    template<typename F, typename... Args>
    auto submit(F&& fn, Args&&... args) -> std::future<decltype(fn(args...))>
    {
      typedef std::packaged_task<decltype(fn(args...))()> PackagedTask;
      auto packaged_task = std::make_shared< PackagedTask >(
          std::bind(std::forward<F>(fn), std::forward<Args>(args)...) );
      const Task task = std::bind(&PackagedTask::operator(), packaged_task);
      submit( task );
      return packaged_task->get_future();
    }

    bool wait_for_empty()
    {
      std::unique_lock<std::mutex> lk(mutex_);

      while (task_count_ && !terminated_)
      {
        cond_.wait(lk);
      }

      return (task_count_ == 0);
    }

    void terminate()
    {
      std::lock_guard<std::mutex> lk(mutex_);
      terminated_ = true;
      cond_.notify_all();
    }

  private:
    std::atomic<bool> terminated_;
    std::vector<std::thread> pool_;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::queue<Task> task_queue_;
    std::size_t task_count_ = 0;

  private:
    void executor_thread() noexcept
    {
      while (!terminated_)
      {
        Task task;

        {
          std::unique_lock<std::mutex> lk(mutex_);

          while (task_queue_.empty() && !terminated_)
          {
            cond_.wait(lk);
          }

          if (terminated_)
          {
            return;
          }
          else
          {
            task = task_queue_.front();
            task_queue_.pop();
          }
        }

        try
        {
          task();
        }
        catch (...)
        {}

        std::unique_lock<std::mutex> lk(mutex_);
        --task_count_;

        if (!task_count_)
        {
          cond_.notify_all();
        }
      }
    }
  };
}
}

#endif /* EXECUTOR_H_ */
