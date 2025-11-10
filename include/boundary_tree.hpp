#include <vector>
#include <cinttypes>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <sysexits.h>
#include "const_enum_define.hpp"

#ifndef __BOUNDARY_TREE_HPP__
#define __BOUNDARY_TREE_HPP__

#include "maxtree_node.hpp"
#include "utils.hpp"
#

class boundary_node;
class boundary_tree;



class boundary_node{
    public:
        //bool in_tree;
        int64_t border_lr; // levelroot of mergeed boundary trees (when nodes are merged)
        boundary_tree *origin; // index of node on the border that added the boundary tree branch
        int64_t boundary_parent; // parent of the node in boundary tree
        maxtree_node *ptr_node;
        boundary_tree *bound_tree_ptr;
        bool in_lroot_tree;
        bool visited;
        
        /* boundary_node(double gval, uint64_t maxtree_idx, uint64_t origin, 
                      uint64_t global_idx, Tattribute a = Tattr_default, 
                      int64_t bound_parent = NO_BOUNDARY_PARENT, 
                      int64_t border_lr = NO_BORDER_LEVELROOT); */
        boundary_node(maxtree_node *n, boundary_tree *bound_tree_ptr, 
                      int64_t bound_parent = NO_BOUNDARY_PARENT, 
                      int64_t border_lr = NO_BORDER_LEVELROOT);
        boundary_node(boundary_node *b);           
        std::string to_string();
        void accumulate_attr(boundary_node *merged);
        void accumulate_attr(Tattribute value);
};

class boundary_tree{
    /* private:
        //std::vector< std::unordered_map<uint64_t, boundary_node *> *> *border_elements;
        // std::vector<std::vector<boundary_node *> *> *border_elements; */
        
    public:
        std::vector<bool> *tile_borders;
        std::vector<std::vector<uint64_t> *> *border_elements;
        std::unordered_map<uint64_t, boundary_node*> *boundary_tree_lroot;
        uint32_t h;
        uint32_t w;
        uint32_t grid_i;
        uint32_t grid_j;
        bool delete_nodes;
        
        boundary_tree(uint32_t h, uint32_t w, uint32_t grid_i, uint32_t grid_j, bool dn=true);
        
        /*boundary_tree(std::vector<std::unordered_map<uint64_t, boundary_node *>*> *border_elements, 
             uint32_t h, uint32_t w, uint32_t grid_i, uint32_t grid_j);*/
        ~boundary_tree();
        
        /* create a copy of n and insert it at border structure (border_elements). Return the pointer of the created copy */
        // boundary_node *insert_border_element(boundary_node &n, enum borders b);
        void insert_border_element(uint64_t n_idx, enum borders b);

        /* check if the node is already on the boundary tree.
        if it is, returns false, otherwise, insert the node at boundary tree structure and return true*/
        bool insert_bnode_lroot_tree(boundary_node *n, bool copy=true);
        
        /*remove node from tree, if it isn't at tree, just */
        bool remove_bnode_lroot_tree(int64_t global_idx);

        /* get node with global_idx at tree structure (boundary_tree_lroot)*/
        boundary_node *get_border_node(int64_t global_idx);
        

        /* get a border from boundary tree*/
        // std::vector<boundary_node *> *get_border(enum borders b);
        std::vector<uint64_t> *get_border(enum borders b);
        
        
        /* merge the calling tree with t */
        boundary_tree *merge(boundary_tree *t, enum merge_directions d, uint8_t connection = 4);
        
        /* return a copy of the of this boundary tree (copy boundary nodes but keeps maxtree_nodes references)*/
        boundary_tree *get_copy(bool deepcopy = false);
        
        

        /* get the levelroot of the bounday_node with global_idx */
        boundary_node *get_bnode_levelroot(int64_t global_idx);

        /* check if a index is a border node (if it is in the border of the tile) */
        bool is_in_border(int64_t global_idx);

        /* combine borders of t1 and t2 into this tree borders. 
        Its assume that t1 is on the left (when direction is horizontal) 
        or on the top (when direction is vertical) */
        void combine_borders(boundary_tree *t1, boundary_tree *t2, enum merge_directions d);
        
        /*Combine nodes of lroot trees that doesn't were used in merge. They must be inserted because they maybe has changes
        that must be sent to the original nodes */
        void combine_lroot_trees(boundary_tree *t1, boundary_tree *t2);


        /* add a levelroot to tree structure (boundary_tree_lroot) */
        void add_lroot_tree(maxtree_node *levelroot, std::vector<maxtree_node*> *maxtree_data, 
                            bool insert_ancestors=true);
        
        /* add a levelroot to tree structure (boundary_tree_lroot) */
        void add_lroot_tree(boundary_node *levelroot, bool insert_ancestors = false,
                            bool copy = true);

        /* check if a node with n_idx is root of the tree */
        bool is_root(uint64_t n_idx);
        
        /* merge two branches started at nodes this_node(from this tree) and t_node (from t tree) */
        void merge_branches(boundary_node *x, boundary_node *y, 
                            std::unordered_map<uint64_t, bool> &accx,
                            std::unordered_map<uint64_t, bool> &accy,
                            std::unordered_map<int64_t, int64_t> &levelroot_pairs);
        void merge_branches_gaz(boundary_node *x, boundary_node *y, std::unordered_map<uint64_t, bool> &acc);
        
        void merge_branches_errado(boundary_node *x, boundary_node *y, 
                                    std::unordered_map<uint64_t, bool> &accx,
                                    std::unordered_map<uint64_t, bool> &accy,
                                    std::unordered_map<int64_t, int64_t> &levelroot_pairs);

        /* get index given a position */


        /* update the borders of boundary tree tile */
        // void update_borders(boundary_tree *merged);
        void update_borders();
        /* update the boundary tree post merge */
        void update_tree(boundary_tree *merged);

        /* compress the path to remove duplicated levelroots after merge*/
        void compress_path();

        /* return number of nodes in boundary_tree_lroot */
        uint64_t get_lroot_tree_size();
        
        /* return number of nodes in all borders*/
        uint64_t get_border_size();
        
        /* get linear index of pixel at i, j */
        uint64_t index_of(uint32_t i, uint32_t j);
        
        /* change vector of one border (top, left, bottom or right) */
        // void change_border(std::vector<boundary_node *> *new_border, enum borders b);
        void change_border(std::vector<uint64_t> *new_border, enum borders b);
        
        /* obtain the line and column of a given index */
        std::tuple<uint32_t, uint32_t> lin_col(uint64_t index);
        
        /* convert all border_elements to string  */
        std::string border_to_string(enum boundary_tree_field f=BOUNDARY_GLOBAL_IDX, std::string endl="\n", std::string sep= " ");
        
        /* print all nodes from the boundary_tree_lroot */
        std::string lroot_to_string(enum boundary_tree_field f=BOUNDARY_BORDER_LR, std::string endl=" ");
        
        
        /* print boundary tree (borders and levelroots) */
        void print_tree(enum boundary_tree_field lrootf= BOUNDARY_BORDER_LR, enum boundary_tree_field borderf=BOUNDARY_ALL_FIELDS);

        /* functions to help find bugs */
        bool search_cicle(int64_t s);

        /* search for a node with attribute greater or equal than lambda */
        maxtree_node *up_tree_filter(uint64_t gidx, Tattribute lambda);

};


#endif