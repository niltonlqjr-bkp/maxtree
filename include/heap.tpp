#include "heap.hpp"
#include <iostream>
//===========================private=============================
template<typename T>
// template<typename T, class Compare>
int max_heap<T>::parent_idx(int idx){
    return (idx-1) >> 1; // (idx - 1) / 2
}

template<typename T>
// template<typename T, class Compare>
int max_heap<T>::right_child_idx(int idx){
    return (idx << 1) + 1; // (idx * 2) + 1
}

template<typename T>
// template<typename T, class Compare>
int max_heap<T>::left_child_idx(int idx){
    return (idx << 1) + 2; // (idx * 2) + 2
}

template<typename T>
// template<typename T, class Compare>
void max_heap<T>::up_idx(int idx){
    //T val = this->data.at(idx);
    int pidx = this->parent_idx(idx);
    while(idx > 0 && this->data.at(pidx) < (this->data.at(idx))){
        T aux = this->data.at(idx);
        this->data.at(idx) = this->data.at(pidx);
        this->data.at(pidx) = aux;
        idx = pidx;
        pidx = this->parent_idx(idx);
    }
}

template<typename T>
// template<typename T, class Compare>
void max_heap<T>::max_heapfy(int idx){
    int promote = idx;
    int old_promote, rc, lc;
    bool change;
    do{
        
        old_promote = promote;
        change =false;
        rc = this->right_child_idx(promote);
        lc = this->left_child_idx(promote);
        
        if(this->data.size() > rc &&
            this->data.at(rc) > this->data.at(promote)){
            promote = rc;
        }

        if(this->data.size() > lc &&
           this->data.at(lc) > this->data.at(promote)){
            promote = lc;
        }



        if(promote != old_promote){
            T *aux = new T(this->data.at(promote));
            this->data.at(promote) = this->data.at(old_promote);
            this->data.at(old_promote) = *aux;
            delete aux;
            change = true;
        }
    }while(change);
}

template<typename T>
// template<typename T, class Compare>
void max_heap<T>::build_max_heap(){
    for(int i=(this->data.size()) >> 1; i >= 0; i-- ){
        this->max_heapfy(i);
    }
}


//===============================public=======================================
template<typename T>
// template<typename T, class Compare>
void max_heap<T>::print(){
    for(auto x: this->data){
        std::cout << x <<" ";
    }
    std::cout << "\n";
}

template<typename T>
// template<typename T, class Compare>
int max_heap<T>::size(){
    return this->data.size();
}

template<typename T>
// template<typename T, class Compare>
max_heap<T>::max_heap(std::vector<T> ini){
    this->data = ini;
    this->build_max_heap();
} 

template<typename T>
// template<typename T, class Compare>
max_heap<T>::~max_heap(){
}

template<typename T>
// template<typename T, class Compare>
void max_heap<T>::insert(T value){
    this->data.push_back(value);
    this->up_idx(this->data.size()-1);
}

template<typename T>
// template<typename T, class Compare>
T max_heap<T>::at(int idx){
    return this->data.at(idx);
}

template<typename T>
// template<typename T, class Compare>
void max_heap<T>::remove_at(int idx){
    this->data.at(idx) = this->data.back();
    this->data.pop_back();
    this->max_heapfy(idx);
}

