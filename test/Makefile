SBFFILE = test_result.sbf

all: $(SBFFILE)

gen: random_file_gen.c
	rm -fr data
	mkdir -p data
	gcc -o gen random_file_gen.c

$(SBFFILE): gen
	./gen 100
	../bin/sbfbind $(SBFFILE) data/*

unbind: $(SBFFILE)
	rm -fr result
	mkdir -p result
	../bin/sbfunbind $(SBFFILE) result/

clean:
	rm -fr result data gen $(SBFFILE)
