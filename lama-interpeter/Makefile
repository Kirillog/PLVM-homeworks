.PHONY: all regression

all:
	$(MAKE) -C runtime
	$(MAKE) -C byterun

regression-all: regression regression-expressions

regression:
	$(MAKE) clean check -j8 -C regression

regression-expressions:
	$(MAKE) clean check -j8 -C regression/expressions
	$(MAKE) clean check -j8 -C regression/deep-expressions

clean:
	$(MAKE) clean -C runtime
	$(MAKE) clean -C regression
	$(MAKE) clean -C byterun
	$(MAKE) clean -C performance