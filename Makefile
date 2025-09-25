parallel_dir = ./parallel
flood_dir = ./sequential/flood
union_find_dir = ./sequential/union-find

MAKE_SUBPROJECTS_ARGS=

ifdef TYPE
	MAKE_SUBPROJECTS_ARGS:=TYPE=${TYPE}
endif

ifdef OPT
	MAKE_SUBPROJECTS_ARGS:=${MAKE_SUBPROJECTS_ARGS} OPT=${OPT}
endif

all: parallel_execs flood_execs

parallel_execs:
	$(MAKE) -C ${parallel_dir} ${MAKE_SUBPROJECTS_ARGS}

flood_execs:
	$(MAKE) -C ${flood_dir} ${MAKE_SUBPROJECTS_ARGS}

union:
	$(MAKE) -C ${union_find_dir} ${MAKE_SUBPROJECTS_ARGS}
