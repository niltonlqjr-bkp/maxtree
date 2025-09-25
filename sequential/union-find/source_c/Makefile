sources = quadratic.c
args = `pkg-config vips --cflags --libs`


test_files=../../testes/fig.png ../../testes/example.png ../../testes/high_border.png

dest = exec/quadratic

all:
	@mkdir -p exec
	gcc ${sources} ${args} -o ${dest}

clean:
	rm -rf ${dest}

tests: all
	for input_test in ${test_files} ; do \
		./${dest_nonrec_wilk} $$input_test ; \
		./${dest_nonrec} $$input_test ; \
		./${dest_rec} $$input_test ; \
	done