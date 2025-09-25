#include <iostream>
#include <vips/vips8>

#include "utils.hpp"
#include "maxtree.hpp"
#include "maxtree_node.hpp"

void canonicalize_tree(maxtree *t, std::vector<maxtree_node *> &r){
    for(auto p: r){
        maxtree_node *q = t->at_pos(p->parent);
        if(q->gval == t->at_pos(q->parent)->gval){
            p->parent = q->parent;
        }
    }
}

maxtree_node *find_root(maxtree_node *x, std::unordered_map<int, maxtree_node*> &zpar){
    if(zpar.at(x->idx)->idx == x->idx){
        return x;
    }else{
        zpar.at(x->idx) = find_root(zpar.at(x->idx),zpar);
        return zpar.at(x->idx);
    }
}

void compute_maxtree(maxtree *t){
    std::unordered_map<int, maxtree_node*> zpar, repr;
    std::unordered_map<int, int> rank;
    std::vector<maxtree_node *> s;
    for(int l=0;l<t->h; l++){
        for(int c=0;c<t->w; c++){
            maxtree_node *p = t->at_pos(l,c);
            p->parent=-1;
            s.push_back(p);
            zpar[p->idx] = p;

        }
    }
    std::sort(s.begin(),s.end(), cmp_maxtree_nodes());
    for (int i=s.size()-1; i>=0; i--){
        maxtree_node *p = s.at(i);
        p->parent = p->idx;
        zpar[p->idx] = p;
        rank[p->idx] = 0;
        repr[p->idx] = p;
        maxtree_node *zp = p;
        for(auto n: t->get_neighbours(p->idx)){
            if(t->at_pos(n->idx)->parent != -1){
                maxtree_node *zn = find_root(n, zpar);
                if(zn->idx != zp->idx){
                    repr[zn->idx]->parent = p->idx;
                    if(rank[zp->idx] < rank[zn->idx]){
                        auto aux = zp;
                        zp = zn;
                        zn = aux;
                    }
                    zpar[zn->idx] = zp;
                    repr[zp->idx] = p;
                    if (rank[zp->idx] == rank[zn->idx]){
                        rank[zp->idx]++;
                    }
                }
            }
        }
    }
    canonicalize_tree(t,s);

}

int main(int argc, char *argv[]){
    vips::VImage *in;
    maxtree *t;
    std::cout << "argc: " << argc << " argv:" ;
    for(int i=0;i<argc;i++){
        std::cout << argv[i] << " ";
    }
    std::cout << "\n";


    if(argc < 2){
        std::cout << "usage: " << argv[0] << " input_image config_file\n";
        exit(0);
    }

    if (VIPS_INIT (argv[0])) {
        vips_error_exit (NULL);
    }

    bool verbose=false;
    in = new vips::VImage(vips::VImage::new_from_file(argv[1],NULL));


    if(argc > 2){    
        auto configs = parse_config(argv[2]);
        if (configs->find("verbose") != configs->end()){
            if(configs->at("verbose") == "true"){
                verbose=true;
            }
        }
    }

    std::cout << "start\n";
    
    int h,w;
    h=in->height();
    w=in->width();
    t = new maxtree(h,w);
    vips::VImage cp = in->copy_memory();
    t->fill_from_VImage(cp);
    compute_maxtree(t);
    if(verbose){
        std::cout<<"__________________GVAL________________\n";
        std::cout << t->to_string(GVAL,5);
        std::cout<<"__________________PARENT________________\n";
        std::cout << t->to_string();
    }
    
    
    return 0;
}
