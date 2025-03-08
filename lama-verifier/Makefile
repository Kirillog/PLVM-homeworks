.PHONY: all regression

all:
	$(MAKE) -C runtime
	$(MAKE) -C byterun

regression:
	$(MAKE) clean check -j8 -C regression

clean:
	$(MAKE) clean -C runtime
	$(MAKE) clean -C regression
	$(MAKE) clean -C byterun
	$(MAKE) clean -C performance