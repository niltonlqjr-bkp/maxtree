#include <cinttypes>

#ifndef __BAG_OF_TASK_HPP__
#define __BAG_OF_TASK_HPP__

#include "heap.hpp"


template <class Task>
class bag_of_tasks{
    private:
        max_heap<Task> *tasks;
        std::mutex lock;
        std::condition_variable has_task, no_task;
        int num_task;
        bool running;
        int waiting;
    public:
        bag_of_tasks();
        ~bag_of_tasks();
        void insert_task(Task t);
        int position_of(int priority);
        bool is_running();
        bool get_task(Task &ret, int priority = -1);
        void wait_empty();
        void wakeup_workers(bool lock = true);
        void notify_end();
        int num_waiting();
        void print();
        bool empty();
};

#include "bag_of_task.tpp"

#endif