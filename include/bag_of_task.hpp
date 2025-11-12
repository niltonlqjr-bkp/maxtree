#include <cinttypes>
#include <condition_variable>
#include <deque>

#ifndef __BAG_OF_TASK_HPP__
#define __BAG_OF_TASK_HPP__

#include "heap.hpp"

template <class Task>
class bag_of_tasks{
    private:
        std::deque<Task> *tasks;
        std::mutex lock;
        std::condition_variable has_task, no_task;
        int num_task;
        bool running;
        int waiting;
    public:
        bag_of_tasks(bool start_running = false);
        ~bag_of_tasks();
        void start();
        void insert_task(Task t);
        int position_of(int priority);
        bool is_running();
        bool get_task(Task &ret);
        // bool get_task_by_position(Task &ret, int position);
        template <class T> bool get_task_by_field(Task &ret, T value, T getter(Task));
        Task at(int pos);
        void wait_empty();
        void wakeup_workers(bool lock = true);
        void notify_end();
        int num_waiting();
        int get_num_task();
        bool empty();
        template <class T> uint64_t search_by_field(T value, T getter(Task), bool lock=false);

};


template <class Task>
class prio_bag_of_tasks{
    private:
        min_heap<Task> *tasks;
        std::mutex lock;
        std::condition_variable has_task, no_task;
        int num_task;
        bool running;
        int waiting;
    public:
        prio_bag_of_tasks(bool start_running = false);
        ~prio_bag_of_tasks();
        void start();
        void insert_task(Task t);
        int position_of(int priority);
        bool is_running();
        bool get_task(Task &ret, int priority = -1);
        bool get_task_by_position(Task &ret, int position);
        template <class T> bool get_task_by_field(Task &ret, T value, T getter(Task));
        Task at(int pos);
        void wait_empty();
        void wakeup_workers(bool lock = true);
        void notify_end();
        int num_waiting();
        int get_num_task();
        void print();
        bool empty();
        template <class T> uint64_t search_by_field(T value, T getter(Task), bool lock=false);

};

#include "bag_of_task.tpp"

#endif