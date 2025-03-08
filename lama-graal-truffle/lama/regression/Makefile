TESTS=$(sort $(basename $(wildcard test*.lama)))


.PHONY: check $(TESTS)

check: $(TESTS)

$(TESTS): %: %.lama
	@echo "regression/$@"
	gradle -p ../ run --args "regression/$@.lama regression/$@.input regression/$@.log" && diff $@.log orig/$@.log

clean:
	$(RM) test*.log *~ $(TESTS)
