#include "utils.hpp"

std::string terminal_color_string(int color){
    if(color == RESET)
        return COLORS[RESET];
    if(color >= 0)
        return COLORS[color];
    return COLORS[BLACK];
}



void label_components(std::vector<maxtree_node*> *mt){
    for(auto p: *mt){
        if(p->gval == mt->at(p->parent)->gval){
            p->label = p->parent;
        }else{
            p->label = p->idx;
        }
    }
}

std::ostream &operator<<(std::ostream &o, maxtree_node &n){
    o << "(idx:" << n.idx << " gval:"<< n.gval <<") ";
    return o;
}

void print_stack(std::stack<maxtree_node*> s){
    std::cout <<"=========STACK===========\n";
    while(!s.empty()){
        std::cout << *(s.top()) << " ";
        s.pop();
    }
    std::cout<<"\n";
    std::cout <<"============================\n";
}

unsigned int index_of(unsigned int l, unsigned int c, int h, int w){
    return l*w+c;    
}



void print_VImage_band(vips::VImage *in, int band){
    double p;
    int h=in->height();
    int w=in->width();
    vips::VImage img = in->copy_memory();

    for(int l=0; l < h; l++){
        for(int c=0; c < w; c++){
            //p = in->getpoint(c,l)[band];
            VipsPel *vpel = VIPS_IMAGE_ADDR(img.get_image(), c, l);
            double val = (int) *vpel;
            int color = (int) val / 31;
            std::cout << terminal_color_string(color);
            std::cout.width(4);
            std::cout << val ;
            std::cout << terminal_color_string(RESET);
        }
        std::cout << "\n";
    }
    
    return;
}


std::string ltrim(std::string s, const std::string b){
    s.erase(0, s.find_first_not_of(b));
    return s;
}

std::string rtrim(std::string s, const std::string b){
    s.erase(s.find_last_not_of(b)+1);
    return s;
}


bool is_blank(std::string s, std::vector<char> b){
    for(int i=0; i<s.size(); i++){
        if(std::find(b.begin(), b.end(), s[i]) == b.end()){
            return false;
        }
    }
    return true;
}

double round_decimal(double d, uint32_t c){
    auto m = std::pow(10,c);
    d *= m;
    d = std::round(d);
    d /= m;
    return d;
}

std::string double_to_string(double d, uint32_t c){
    std::string ret = std::to_string(round_decimal(d,c));
    
    auto p = ret.find(".");
    c = c==0? c : c+1; // if no decimal part, so we exclude the '.' symbol
    ret = ret.substr(0,p+c);
    return ret;
}


std::string fill(std::string s, int size){
    if(size <= s.length()){
        return s;
    }
    
    std::string r;
    int missing = size - s.length();

    for(int i=0;i<missing; i++){
        r+=" ";
    }
    r+=s;
    return r;
}

void print_labels(std::vector<maxtree_node*> *m, uint64_t  h, uint64_t w, bool metadata){
    std::cout << h << ", " << w << "\n";
    std::cout << m->size()<<"\n";
    int l,c;
    for(l=0;l<h; l++){
        if(metadata) std::cout << l << ":";
        for(c=0;c<w; c++){
            if(metadata) std::cout << "("<< l << "," << c <<")";
            std::cout.width(4);
            std::cout << m->at(index_of(l,c,h,w))->label << " ";
        }
        std::cout << "\n";
    }
}


void print_matrix(std::vector<maxtree_node*> *m, uint64_t  h, uint64_t w, bool metadata){
    std::cout << h << ", " << w << "\n";
    std::cout << m->size()<<"\n";
    int l,c;
    for(l=0;l<h; l++){
        if(metadata) std::cout << l << ":";
        for(c=0;c<w; c++){
            if(metadata) std::cout << "("<< l << "," << c <<")";
            //std::cout.width(4);
            std::cout << m->at(index_of(l,c,h,w))->parent << " ";
        }
        std::cout << "\n";
    }
}

std::unordered_map<std::string, std::string> *parse_config(char arg[]){
    std::ifstream f(arg);
    std::string l;
    std::unordered_map<std::string, std::string> *ret = new std::unordered_map<std::string, std::string>();
    while (!f.eof()){
        std::getline(f, l);
        bool blank = is_blank(l);
        
        if(!blank && l.front() != '#'){
            size_t spl_pos = l.find("=");
            std::string k = ltrim(rtrim(l.substr(0,spl_pos)));
            std::string v = ltrim(rtrim(l.substr(spl_pos+1)));
            (*ret)[k] = v;
        }
    }
    return ret;
}

void print_unordered_map(std::unordered_map<std::string, std::string> *m){
    for(auto x: *m){
        std::cout << x.first << "=>" << x.second << "\n";
    }
}

maxtree_node *min_gval(std::vector<maxtree_node*> *t){
    unsigned int i; 
    maxtree_node *min = t->at(0);
    
    for(i=1; i < t->size(); i++){
        if (t->at(i)->gval < min->gval){
            min = t->at(i);
        }
    }
    return min;
}

maxtree_node *min_gval(std::unordered_map<int, maxtree_node*> *t){
    unsigned int i; 
    maxtree_node *min = t->begin()->second;
    for(auto p: *t){
        if (p.second->gval < min->gval){
            min = p.second;
        }
    }
    return min;
}



void print_pq(std::priority_queue<maxtree_node*> pq){
    std::cout <<"===========QUEUE=============\n";
    while(!pq.empty()){
        auto e=pq.top();
        std::cout<<*e<<" ";
        pq.pop();
    }
    std::cout<<"\n";
    std::cout <<"============================\n";
}
