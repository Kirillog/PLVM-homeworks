.PHONY: all regression

all:
	$(MAKE) -C runtime
	$(MAKE) -C byterun
	$(MAKE) -C runtime unit_tests.o
	$(MAKE) -C runtime invariants_check.o
	$(MAKE) -C runtime invariants_check_debug_print.o

regression-all: regression regression-expressions

regression:
	$(MAKE) clean check -j8 -C regression

regression-expressions:
	$(MAKE) clean check -j8 -C regression/expressions
	$(MAKE) clean check -j8 -C regression/deep-expressions

negative_scenarios_tests:
	$(MAKE) -C runtime negative_tests

clean:
	$(MAKE) clean -C runtime
	$(MAKE) clean -C regression
	$(MAKE) clean -C byterun