all:
	$(MAKE) -C src
	$(MAKE) -C sample

clean:
	$(MAKE) -C sample clean
	$(MAKE) -C src clean
