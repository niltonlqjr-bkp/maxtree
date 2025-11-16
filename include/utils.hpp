#include <vips/vips8>
#include <iostream>
#include <vector>
#include <tuple>
#include <ostream>
#include <stack>
#include <queue>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include <cinttypes>
#include <cmath>

#include "maxtree_node.hpp"

#ifndef __UTILS_HPP__
#define __UTILS_HPP__


#define COLOR_BLACK	  "\033[0;30m"
#define	COLOR_RED     "\033[0;31m"	
#define	COLOR_GREEN   "\033[0;32m"	
#define	COLOR_YELLOW  "\033[0;33m"	
#define	COLOR_BLUE    "\033[0;34m"	
#define	COLOR_MAGENTA "\033[0;35m"
#define	COLOR_CYAN    "\033[0;36m"	
#define	COLOR_WHITE   "\033[0;37m"	
#define COLOR_RESET   "\033[0m"

const std::string COLORS[] = {COLOR_WHITE,
                              COLOR_RED,
                              COLOR_GREEN,
                              COLOR_YELLOW,
                              COLOR_BLUE,
                              COLOR_MAGENTA,
                              COLOR_CYAN,
                              COLOR_WHITE,
                              COLOR_RESET
                            };
                            
enum terminal_colors {BLACK,RED,GREEN,YELLOW,BLUE,MAGENTA,CYAN,WHITE,RESET};

std::string terminal_color_string(int color);

void label_components(std::vector<maxtree_node*> *mt);

struct cmp_maxtree_nodes{
    bool operator()(const maxtree_node* lhs, const maxtree_node* rhs) const{
        return lhs->gval < rhs->gval;
    }
};



std::ostream &operator<<(std::ostream &o, maxtree_node &n);

std::unordered_map<std::string, std::string> *parse_config(char arg[]);

void print_unordered_map(std::unordered_map<std::string, std::string> *m);

unsigned int index_of(unsigned int l, unsigned int c, int h, int w);

void print_VImage_band(vips::VImage *in, int band = 0);

void print_labels(std::vector<maxtree_node*> *m, uint64_t  h, uint64_t w, bool metadata=false);

void print_matrix(std::vector<maxtree_node*> *m, uint64_t  h, uint64_t w, bool metadata=false);

void print_stack(std::stack<maxtree_node*> s);

bool is_blank(std::string s, std::vector<char> b = {' ', '\t', '\n', '\r'});

std::string int_pair_to_string(std::pair<uint32_t, uint32_t> &p);

std::string ltrim(std::string s, const std::string b = std::string(" \t\n\r"));

std::string rtrim(std::string s, const std::string b = std::string(" \t\n\r"));

std::string fill(std::string s, int size);

std::string double_to_string(double n, uint32_t c);

maxtree_node *min_gval(std::vector<maxtree_node*> *t);

maxtree_node *min_gval(std::unordered_map<int, maxtree_node*> *t);

void print_pq(std::priority_queue<maxtree_node*> pq);

uint64_t get_levroot_pair_idx(std::unordered_map<int64_t, int64_t> &lrp, uint64_t idx);

int64_t int_pow(int64_t base, int64_t exp);


#endif