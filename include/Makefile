#Flags on comand line:
#	OPT flag to describe the optimization flags (e.g make OPT=-O3)
#	TYPE flag to describe if you want to compile for (D)ebug or (R)elease, default is Debug (e.g. make TYPE=R)

maxtree_node_dir=.
maxtree_dir=.
boundary_tree_dir=.
utils_dir=.
heap_dir=.

tasks_dir=.


vips_args = `pkg-config vips-cpp --cflags --libs`

ifeq (${TYPE},R)
	OBJ_CPP_FLAGS=-c 
	EXE_CPP_FLAGS=-pthread
	OPT ?= -O2
else 
	OBJ_CPP_FLAGS=-g -c
	EXE_CPP_FLAGS=-g -pthread
	OPT = 
endif

l ?= 3
c ?= 5

figures_flags ?= 

all: maxtree_node.o maxtree.o utils.o boundary_tree.o

boundary_tree.o: ${boundary_tree_dir}/boundary_tree.cpp ${boundary_tree_dir}/boundary_tree.hpp
	g++ ${OBJ_CPP_FLAGS} ${OPT} ${boundary_tree_dir}/boundary_tree.cpp -I${boundary_tree_dir} ${vips_args} -o ${boundary_tree_dir}/boundary_tree.o

maxtree.o: ${maxtree_dir}/maxtree.cpp ${maxtree_dir}/maxtree.hpp
	g++ ${OBJ_CPP_FLAGS} ${OPT} ${maxtree_dir}/maxtree.cpp -I${maxtree_dir} ${vips_args} -o ${maxtree_dir}/maxtree.o

maxtree_node.o: ${maxtree_node_dir}/maxtree_node.cpp ${maxtree_node_dir}/maxtree_node.hpp 
	g++ ${OBJ_CPP_FLAGS} ${OPT} ${maxtree_node_dir}/maxtree_node.cpp -I${maxtree_node_dir} -o ${maxtree_node_dir}/maxtree_node.o

utils.o: ${utils_dir}/utils.cpp ${utils_dir}/utils.hpp
	g++ ${OBJ_CPP_FLAGS} ${OPT} ${utils_dir}/utils.cpp ${vips_args} -I${utils_dir} -o ${utils_dir}/utils.o

clean:
	@rm ${maxtree_node_dir}/maxtree_node.o || true
	@rm ${maxtree_dir}/maxtree.o || true
	@rm ${boundary_tree_dir}/boundary_tree.o || true
	@rm ${utils_dir}/utils.o || true
