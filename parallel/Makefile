#Flags on comand line:
#	OPT flag to describe the optimization flags (e.g make OPT=-O3)
#	TYPE flag to describe if you want to compile for (D)ebug or (R)elease, default is Debug (e.g. make TYPE=R)

maxtree_node_dir=../include
maxtree_dir=../include
boundary_tree_dir=../include
utils_dir=../include
heap_dir=../include

tasks_dir=.


vips_args = `pkg-config vips-cpp --cflags --libs`

ifeq (${TYPE},R)
	OBJ_CPP_FLAGS=-c 
	EXE_CPP_FLAGS=-pthread
	OPT ?= -O2
else 
	OBJ_CPP_FLAGS=-g -c
	EXE_CPP_FLAGS=-g -pthread
endif

divide_name=divide
merge_name=merge
merge_s_name=merge_s
grow_name=grow_region

source_grow=${grow_name}.cpp
source_merge=${merge_name}.cpp
source_merge_s=${merge_s_name}.cpp
source_divide=${divide_name}.cpp

dest_grow = exec/${grow_name}
dest_divide = exec/${divide_name}
dest_merge = exec/${merge_name}
dest_merge_s = exec/${merge_s_name}




l ?= 3
c ?= 5

test_file ?= ../testes/example_gotz.png
config_file ?= ../configs/print_trees_config_${l}l${c}c.txt
output_text_dir ?= saidas/text
output_text_tree ?= ${output_text_dir}/tree_${l}l${c}c.txt
output_figures_trees ?= saidas/trees/tree_${l}l${c}c

config_verbose_file = ../configs/verbose_config_${l}l${c}c.txt
output_verbose = ${output_text_dir}/verbose_${l}l${c}c.txt

figures_flags ?= 



all: ${dest_merge} 


${dest_merge}:base tasks.o
	@mkdir -p exec
	g++ ${OPT} ${EXE_CPP_FLAGS} ${source_merge} \
		${utils_dir}/utils.o \
	 	${maxtree_node_dir}/maxtree_node.o \
		${maxtree_dir}/maxtree.o \
		${boundary_tree_dir}/boundary_tree.o \
		${tasks_dir}/tasks.o \
		-I${maxtree_node_dir} -I${utils_dir} -I${maxtree_dir} -I${heap_dir} \
		${vips_args} -o ${dest_merge}

${dest_merge_s}: base
	@mkdir -p exec
	g++ ${OPT} ${EXE_CPP_FLAGS} ${source_merge_s} \
		${utils_dir}/utils.o \
	 	${maxtree_node_dir}/maxtree_node.o \
		${maxtree_dir}/maxtree.o \
		${boundary_tree_dir}/boundary_tree.o \
		-I${maxtree_node_dir} -I${utils_dir} -I${maxtree_dir} -I${heap_dir} \
		${vips_args} -o ${dest_merge_s}

${dest_divide}: base
	@mkdir -p exec
	g++ ${OPT} ${EXE_CPP_FLAGS} ${source_divide} \
		${utils_dir}/utils.o \
	 	${maxtree_node_dir}/maxtree_node.o \
		${maxtree_dir}/maxtree.o \
		${boundary_tree_dir}/boundary_tree.o \
		-I${maxtree_node_dir} -I${utils_dir} -I${maxtree_dir} -I${heap_dir} \
		${vips_args} -o ${dest_divide}

vips_region: base
	@mkdir -p exec
	g++ ${OPT} ${EXE_CPP_FLAGS} teste_region.cpp \
		${utils_dir}/utils.o \
	 	${maxtree_node_dir}/maxtree_node.o \
		${maxtree_dir}/maxtree.o \
		${boundary_tree_dir}/boundary_tree.o \
		-I${maxtree_node_dir} -I${utils_dir} -I${maxtree_dir} -I${heap_dir} \
		${vips_args} -o teste_region

${dest_grow}: base
	@mkdir -p exec
	g++ ${OPT} ${EXE_CPP_FLAGS} ${source_grow} \
		${utils_dir}/utils.o \
	 	${maxtree_node_dir}/maxtree_node.o \
		${maxtree_dir}/maxtree.o \
		${boundary_tree_dir}/boundary_tree.o \
		-I${maxtree_node_dir} -I${utils_dir} -I${maxtree_dir} -I${heap_dir} \
		${vips_args} -o ${dest_grow}


tasks.o: ${tasks_dir}/tasks.cpp ${tasks_dir}/tasks.hpp
	g++ ${OBJ_CPP_FLAGS} ${tasks_dir}/tasks.cpp -I${tasks_dir} -I${maxtree_dir} ${vips_args} -o ${tasks_dir}/tasks.o

base: 
	export TYPE=${TYPE}
	@make -C ${maxtree_node_dir} 
	@make -C ${maxtree_dir} 
	@make -C ${boundary_tree_dir} 
	@make -C ${utils_dir}  

test: merge
	@mkdir -p ${output_text_dir} 
	./${dest_merge} ${test_file} ${config_verbose_file} > ${output_verbose} 

figures: merge
	./${dest_merge} ${test_file} ${config_file} > ${output_text_tree} 
	@mkdir -p ${output_figures_trees}
	python3 ../util/trees_logs2fig.py ${output_text_tree} -d ${output_figures_trees} -e pdf ${figures_flags}

test_figures: test figures

clean:
	@make -C ${maxtree_node_dir} clean
	@make -C ${maxtree_dir} clean
	@make -C ${boundary_tree_dir} clean
	@make -C ${utils_dir} clean
	rm ${dest_merge} || true
	rm ${dest_divide} || true
	rm ${dest_grow} || true

#heap.o:
#	g++ ${OBJ_CPP_FLAGS} ${heap_dir}/heap.cpp -I${heap_dir} -o ${heap_dir}/heap.o
