#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <queue>
#include <functional>
#include <thread>
#include <vector>
#include <mutex>
#include <future>

#include <iostream>

//TODO : write ThreadPool::join()

/**
 * Simple alias type for unique locks
 */
using GuardLock = std::unique_lock<std::mutex>;

/**
 * This class models a thread pool with a specified number of workers.
 * <p>
 * It is possible to enqueue tasks (that is, functions taking an arbitrary number of arguments and potentially
 * returning a value) that will be runned when a worker is free.
 * <p>
 * It is also possile to stop every worker. In that case, any running tasks finish their work, and emaining tasks
 * are not runned. Note that after calling this function, calling std::future::get on a tasks that has not been
 * runned will block with no possibility of unblocking.
 * <p>
 * Also note that it is possible to specify the behaviour of the destructor of this class, by telling wether or not the
 * worker threads need to be stopped when a threadpool object is deallocated. By default, this parameter is set as true.
 * Note that when set true, it means that on deallocation, the current tasks that are running are finished (and are not
 * brutally stopped), and no other tasks are ran. It is then possible that when stopped, there are still some remaining
 * enqueued tasks that have not been ran.
 * <p>
 * <h3>Code example</h3>
 * <p>
 * <pre><code>
 int make_addition(int i, int j)
 {
     return i + j;
 }

 int main()
 {
    dg::utils::ThreadPool t(5); //a pool of five threads

    std::vector<std::future<int>> v; //int because make_addition returns ints

    for(int i = 0; i < 500; i++)
        for(int j = 0; j < 500; j++)
            v.push_back(t.enqueue(make_addition, i, j)); //I could push any function here, not only (int)(*f)(int,int)

    for(unsigned i = 0; i < v.size(); i++)
        cout << v[i].get() << endl; //get blocks until the result has been computed
  }
  </code></pre>
 * @brief This class models a thread pool with a specified number of workers.
 */
class ThreadPool //FIXME : I suspect I'm vulnerable to race / spurious wakeups
{
    class ThreadWorker
    {
        static int id_count;

        const int _id; //incremented automaticaly from id_count
        ThreadPool * pool; //it's just an adress : nothing is allocated

        public:
            explicit ThreadWorker(ThreadPool * pool);

            ThreadWorker(ThreadWorker&&) = default;

            ThreadWorker(const ThreadWorker& w) = delete;
            ThreadWorker& operator =(const ThreadWorker &) = delete;
            ThreadWorker& operator =(ThreadWorker &&) = delete;

            int id() const; //for debugging purposes

            void operator()(); //waits until notified, then it takes the first task and runs it
    };

    bool _stop_on_deallocation;
    bool _stopped; //did we ask to stop it ?
    std::queue<std::function<void()>> tasks; //list of tasks yet to perform
    //std::array<std::thread, N> threads; //running threads
    std::vector<std::thread> threads;
    std::mutex mutex; //mutex
    std::condition_variable condition; //for notifying

    public:
        /**
         * Builds up a thread pool
         * <p>
         * Note that it is possible to specify the behaviour of the destructor of this class, by telling wether
         * or not the worker threads need to be stopped when a threadpool object is deallocated. By default,
         * this parameter is set as true. Note that when set true, it means that on deallocation, the current
         * tasks that are running are finished (and are not brutally stopped), and no other tasks are ran. It
         * is then possible that when stopped, there are still some remaining enqueued tasks that have not been
         * ran.
         * @brief Builds up a thread pool
         * @param n the number of worker threads to run
         * @param stop_on_deallocation true if you want to stop the workers when deallocating the pool
         */
        explicit ThreadPool(unsigned n = std::thread::hardware_concurrency(), bool stop_on_deallocation = true);

        /**
          * Destructor : stops the pool if <code>stop_on_deallocation</code> is true.
          * @brief Destructor : stops the pool if <code>stop_on_deallocation</code> is true.
          */
        ~ThreadPool();

        /**
         * Deleted copy constructor
         * @brief Deleted copy constructor
         */
        ThreadPool(const ThreadPool &) = delete; //THOU SHALL NOT COPY

        /**
         * Deleted move constructor
         * @brief Deleted move constructor
         */
        ThreadPool(ThreadPool &&) = delete; //THOU SHALL NOT MOVE

        /**
         * Deleted assignment operator
         * @brief Deleted assignment operator
         */
        ThreadPool & operator=(const ThreadPool &) = delete; //THOU SHALL NOT AFFECT

        /**
         * Deleted move-assignment operator
         * @brief Deleted move-assignment operator
         */
        ThreadPool & operator=(ThreadPool &&) = delete; //THAT SHALL NOT MOVE-AFFECT

        /**
         * Stops the thread pool.
         * <p>
         * Basically, any running tasks finish their work. Remaining tasks are not runned.
         * <p>
         * Note that after calling this function, calling std::future::get on a tasks that has
         * not been runned will block with no possibility of unblocking.
         * @brief Stops the pool
         */
        void stop();

        //TODO
        //void join();

        /**
         * Returns true if the pool is stopped, fasle otherwise.
         * @brief Returns true if the pool is stopped, fasle otherwise.
         * @return true if the pool is stopped, fasle otherwise.
         */
        inline bool stopped() const;

        /**
         * Enqueues a task (a function with its arguments) on the pool
         * @param f the function to call
         * @param args the arguments of the function to run
         * @tparam Function the type of the function to run
         * @tparam Args the type of the arguments of the function to run
         */
        template<class Function, class ... Args>
        auto enqueue(Function&& f, Args&& ... args) -> std::future<decltype(f(args...))>;
};

int ThreadPool::ThreadWorker::id_count = 1;

ThreadPool::ThreadWorker::ThreadWorker(ThreadPool * pool) : _id(id_count), pool(pool)
{
    id_count++;
}

int ThreadPool::ThreadWorker::id() const
{
    return _id;
}

void ThreadPool::ThreadWorker::operator()()
{
    while(! pool->stopped()) //I want to join if the pool is stopped
    {
        GuardLock lock(pool->mutex);
        if(pool->tasks.empty()) //wait if there is nothing to do
            pool->condition.wait(lock);

        if(! pool->tasks.empty()) //if there is work to do, do it
        {
            std::function<void()> f = pool->tasks.front();
            pool->tasks.pop();

            lock.unlock(); //looks suspicious, but isn't

            f();
        }
    }
}

ThreadPool::ThreadPool(unsigned n, bool stop_on_deallocation)
    : _stop_on_deallocation(stop_on_deallocation),
      _stopped(false), //as soon as I enqueue a function, it will start running
      tasks(std::queue<std::function<void()>>()), //stuff to do
      //threads(std::array<std::thread, N>()), //pool with N threads
      threads(std::vector<std::thread>(n)),
      mutex(std::mutex()),
      condition(std::condition_variable()) //mutex
{
    for (unsigned i = 0; i < n; ++i)
          threads[i] = std::thread { ThreadWorker(this) };
}

ThreadPool::~ThreadPool()
{
    if(_stop_on_deallocation)
        stop();
}

void ThreadPool::stop()
{
    if(_stopped)
        return;

    _stopped = true;
    condition.notify_all();

    for(unsigned i = 0; i < threads.size(); i++)
        if(threads[i].joinable())
            threads[i].join();
}

//void ThreadPool::join() //I suspect foulness
//{
//    GuardLock lock(mutex);
//    while(! tasks.empty())
//        condition.wait(lock);
//}

bool ThreadPool::stopped() const
{
    return _stopped;
}

template<class Function, class ... Args>
auto ThreadPool::enqueue(Function&& f, Args&& ... args) -> std::future<decltype(f(args...))>
{
    std::function<decltype(f(args...))()> func = std::bind(std::forward<Function>(f), std::forward<Args>(args)...);
    auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

    std::function<void()> wrapper = [task_ptr]() //wrapper to match types of 'tasks'... ugly
    {
        (*task_ptr)();
    };

    { //lock to push
        GuardLock lock(mutex);
        tasks.push(wrapper);
    } //lock released on scope exit

    condition.notify_one(); //wake up a worker

    return task_ptr->get_future();
}

#endif // THREADPOOL_HPP
