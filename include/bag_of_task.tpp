

template <class Task>
bag_of_tasks<Task>::bag_of_tasks(bool start_running){
    this->tasks = new std::deque<Task>();
    this->num_task = 0;
    this->running = start_running;
    this->waiting = 0;
}
template <class Task>
bag_of_tasks<Task>::~bag_of_tasks(){
    delete this->tasks;
}
template <class Task>
void bag_of_tasks<Task>::start(){
    this->running = true;
}

template <class Task>
void bag_of_tasks<Task>::insert_task(Task t){
    std::unique_lock<std::mutex> l(this->lock);
    this->tasks->push_back(t);
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
bool bag_of_tasks<Task>::get_task(Task &ret){
    std::unique_lock<std::mutex> l(this->lock);

    while(this->num_task <= 0 && this->running){
        this->waiting++;
        // std::cout << "wait task\n";
        this->has_task.wait(l);
        // std::cout << "wait task\n";

    }
    if(this->num_task > 0){
        ret = this->tasks->front();
        this->tasks->pop_front();
        this->num_task--;
        this->no_task.notify_all();
        return true;
    }else{
        return false;
    }
    
}

// template <class Task>
// bool bag_of_tasks<Task>::get_task_by_position(Task &ret, int position){
// /*     std::unique_lock<std::mutex> l(this->lock);
//     if(this->num_task > 0 && position < this->tasks->size()){
//         ret = this->at(position);
//         this->tasks->remove_at(position);
//         this->num_task--;
//         this->no_task.notify_all();
//         return true;
//     }
//     return false; */
//     return false;
// }

template <class Task>
template <class T> 
bool bag_of_tasks<Task>::get_task_by_field(Task &ret, T value, T getter(Task)){
    std::unique_lock<std::mutex> l(this->lock);
    Task t;
    try{   
        // std::deque<Task>::iterator it;
        for(auto it=this->tasks->begin(); it != this->tasks->end(); it++ ){
            t = *it; 
            if(value == getter(t)){
                ret = t;
                this->num_task--;
                this->tasks->erase(it);
                this->no_task.notify_all();
                return true;
            }
        }
        return false;
    }catch(...){
        return false;
    }
    // catch(std::runtime_error &e){
    //     return false;
    // }catch(std::out_of_range &e){
    //     return false;
    // }
}

template <class Task>
Task bag_of_tasks<Task>::at(int pos){
    return this->tasks->at(pos);
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
int bag_of_tasks<Task>::get_num_task(){
    return this->num_task;
}


template <class Task>
bool bag_of_tasks<Task>::empty(){
    std::unique_lock<std::mutex> l(this->lock);
    return this->tasks->size() == 0;
}

template <class Task>
template <class T>
uint64_t bag_of_tasks<Task>::search_by_field(T value, T getter(Task), bool lock){
    if(lock){
        std::unique_lock<std::mutex> l(this->lock);
    }
    uint64_t i;
    for(i = 0; i < this->tasks->size(); i++){
        if(value == getter(this->tasks->at(i))){
            return i;
        }
    }
    throw std::runtime_error("error on bag_of_task::search_by_field: Task not found on bag\n");
}






/* Priority bag of tasks */


template <class Task>
prio_bag_of_tasks<Task>::prio_bag_of_tasks(bool start_running){
    this->tasks = new min_heap<Task>();
    this->num_task = 0;
    this->running = start_running;
    this->waiting = 0;
}
template <class Task>
prio_bag_of_tasks<Task>::~prio_bag_of_tasks(){
    delete this->tasks;
}
template <class Task>
void prio_bag_of_tasks<Task>::start(){
    this->running = true;
}

template <class Task>
void prio_bag_of_tasks<Task>::insert_task(Task t){
    std::unique_lock<std::mutex> l(this->lock);
    this->tasks->insert(t);
    this->num_task++;
    this->wakeup_workers(false);
}

template <class Task>
int prio_bag_of_tasks<Task>::position_of(int priority){
    /*elaborar uma formula para obter a posição dada uma prioridade
    pensar numa forma de limitar as prioridades;*/
    return priority;
}

template <class Task>
bool prio_bag_of_tasks<Task>::is_running(){
    return this->running;
}

template <class Task>
bool prio_bag_of_tasks<Task>::get_task(Task &ret, int priority){
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
    // if(!this->running){
    //     return false;
    if(num_task > 0){
        ret = this->tasks->at(pos);
        this->tasks->remove_at(pos);
        this->num_task--;
        this->no_task.notify_all();
        return true;
    }else{
        return false;
    }
    
}

template <class Task>
bool prio_bag_of_tasks<Task>::get_task_by_position(Task &ret, int position){
    std::unique_lock<std::mutex> l(this->lock);
    if(this->num_task > 0 && position < this->tasks->size()){
        ret = this->at(position);
        this->tasks->remove_at(position);
        this->num_task--;
        this->no_task.notify_all();
        return true;
    }
    return false;
}

template <class Task>
template <class T> 
bool prio_bag_of_tasks<Task>::get_task_by_field(Task &ret, T value, T getter(Task)){
    std::unique_lock<std::mutex> l(this->lock);
    try{
        auto idx = this->search_by_field<T>(value, getter, false);
        ret = this->at(idx);
        this->tasks->remove_at(idx);
        this->num_task--;
        this->no_task.notify_all();
        return true;
    }catch(std::runtime_error &e){
        return false;
    }catch(std::out_of_range &e){
        return false;
    }
}

template <class Task>
Task prio_bag_of_tasks<Task>::at(int pos){
    return this->tasks->at(pos);
}

template <class Task>
void prio_bag_of_tasks<Task>::wait_empty(){
    std::unique_lock<std::mutex> l(this->lock);
    while(this->num_task > 0){
        this->no_task.wait(l);
    }
}

template <class Task>
void prio_bag_of_tasks<Task>::wakeup_workers(bool lock){
    if(lock){
        std::unique_lock<std::mutex> l(this->lock);
    }
    this->waiting=0;
    this->has_task.notify_all();
}

template <class Task>
void prio_bag_of_tasks<Task>::notify_end(){
    this->num_task = 0;
    this->running = false;
    this->wakeup_workers();
}

template <class Task>
int prio_bag_of_tasks<Task>::num_waiting(){
    return this->waiting;
}

template <class Task>
int prio_bag_of_tasks<Task>::get_num_task(){
    return this->num_task;
}

template <class Task>
void prio_bag_of_tasks<Task>::print(){
    this->tasks->print();
}

template <class Task>
bool prio_bag_of_tasks<Task>::empty(){
    std::unique_lock<std::mutex> l(this->lock);
    return this->tasks->size() == 0;
}

template <class Task>
template <class T>
uint64_t prio_bag_of_tasks<Task>::search_by_field(T value, T getter(Task), bool lock){
    if(lock){
        std::unique_lock<std::mutex> l(this->lock);
    }
    uint64_t i;
    for(i = 0; i < this->tasks->size(); i++){
        if(value == getter(this->tasks->at(i))){
            return i;
        }
    }
    throw std::runtime_error("error on bag_of_task::search_by_field: Task not found on bag\n");
}