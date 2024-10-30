TESTS=$(sort $(basename $(wildcard *.lama)))

LAMAC=lamac
BYTERUN=../byterun/byterun

.PHONY: check $(TESTS)

check: $(TESTS)

$(TESTS): %: %.lama
	@echo $@
	@LAMA=../runtime $(LAMAC) -b $@.lama && `which time` -f "$@ iter\t%U" $(BYTERUN) $@.bc
	@cat Sort.input | LAMA=../runtime `which time` -f "$@ rec \t%U" $(LAMAC) -s $<

clean:
	$(RM) test*.log *.s *~ $(TESTS) *.i *.bc
