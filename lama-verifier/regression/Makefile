TESTS=$(sort $(basename $(wildcard test*.lama)))

LAMAC=lamac
BYTERUN=../byterun/byterun

.PHONY: check $(TESTS)

check: $(TESTS)

$(TESTS): %: %.lama
	@echo "regression/$@"
	@LAMA=../runtime $(LAMAC) -b $< && cat $@.input | $(BYTERUN) $@.bc > $@.log && diff $@.log orig/$@.log

clean:
	$(RM) test*.log *.s *.sm *~ $(TESTS) *.i *.bc
