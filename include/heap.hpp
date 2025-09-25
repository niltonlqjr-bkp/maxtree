#include <vector>
#include <cinttypes>

#ifndef __HEAP_HPP__
#define __HEAP_HPP__

// template<typename T, class Compare = std::less<T>>
template<typename T>
class max_heap{
    private:
        std::vector<T> data;
        int parent_idx(int idx);
        int right_child_idx(int idx);
        int left_child_idx(int idx);
        void max_heapfy(int idx);
        void build_max_heap();
        void up_idx(int idx);

    public:
        
        max_heap(std::vector<T> ini = std::vector<T>());
        ~max_heap();
        void insert(T value);
        T at(int idx);
        void remove_at(int idx);
        void print();
        int size();
};

#include "heap.tpp"

#endif