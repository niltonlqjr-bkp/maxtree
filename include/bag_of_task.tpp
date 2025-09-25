

template <class Task>
bag_of_tasks<Task>::bag_of_tasks(){
    this->tasks = new max_heap<Task>();
    this->num_task = 0;
    this->running = true;
    this->waiting = 0;
}
template <class Task>
bag_of_tasks<Task>::~bag_of_tasks(){
    delete this->tasks;
}

template <class Task>
void bag_of_tasks<Task>::insert_task(Task t){
    std::unique_lock<std::mutex> l(this->lock);
    this->tasks->insert(t);
    this->num_task++;
    this->wakeup_workers(false);
}

template <class Task>
int bag_of_tasks<Task>::position_of(int priority){
    /*elaborar uma formula para obter a posição dada uma prioridade
    pensar numa forma de limitar as prioridades;*/
    return priority;
}

template <class Task>
bool bag_of_tasks<Task>::is_running(){
    return this->running;
}

template <class Task>
bool bag_of_tasks<Task>::get_task(Task &ret, int priority){
    std::unique_lock<std::mutex> l(this->lock);
    int pos;
    if(priority == -1){
        pos = 0;
    }else{
        pos = this->position_of(priority);
    }
    while(this->num_task <= 0 && this->running){
        this->waiting++;
        this->has_task.wait(l);
    }
    if(!this->running){
        return false;
    }else{
        ret = this->tasks->at(pos);
        this->tasks->remove_at(pos);
        this->num_task--;
        this->no_task.notify_all();
        return true;
    }
    
}

template <class Task>
void bag_of_tasks<Task>::wait_empty(){
    std::unique_lock<std::mutex> l(this->lock);
    while(this->num_task > 0){
        this->no_task.wait(l);
    }
}

template <class Task>
void bag_of_tasks<Task>::wakeup_workers(bool lock){
    if(lock){
        std::unique_lock<std::mutex> l(this->lock);
    }
    this->waiting=0;
    this->has_task.notify_all();
}

template <class Task>
void bag_of_tasks<Task>::notify_end(){
    this->num_task = 0;
    this->running = false;
    this->wakeup_workers();
}

template <class Task>
int bag_of_tasks<Task>::num_waiting(){
    return this->waiting;
}

template <class Task>
void bag_of_tasks<Task>::print(){
    this->tasks->print();
}

template <class Task>
bool bag_of_tasks<Task>::empty(){
    std::unique_lock<std::mutex> l(this->lock);
    return this->tasks->size() == 0;
}
