TESTS=$(sort $(basename $(wildcard generated*.lama)))

LAMAC=lamac
BYTERUN=../../byterun/byterun

.PHONY: check $(TESTS)

check: $(TESTS)

$(TESTS): %: %.lama
	@echo "regression/deep-expressions/$@"
	@LAMA=../../runtime $(LAMAC) -b $< && cat $@.input | $(BYTERUN) $@.bc > $@.log && diff $@.log orig/$@.log

clean:
	rm -f *.log *.s *~ *.bc
	find . -maxdepth 1 -type f -not -name '*.*' -not -name 'Makefile' -delete

