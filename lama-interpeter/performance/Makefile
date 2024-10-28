TESTS=$(sort $(basename $(wildcard *.lama)))

LAMAC=lamac
BYTERUN=../byterun/byterun

.PHONY: check $(TESTS)

check: $(TESTS)

$(TESTS): %: %.lama
	@echo $@
	@cat $@.input | `which time` -f "$@\t%U" LAMA=../runtime $(LAMAC) -i $<
	LAMA=../runtime $(LAMAC) -b $@.lama && cat $@.input | `which time` -f "$@\t%U" $(BYTERUN) $@.bc

clean:
	$(RM) test*.log *.s *~ $(TESTS) *.i
