TEST0 := $(BLD_DIR)/utest_app

$(TEST0) : $(OBJ_DIRS) $(OBJ)
	make -C srcs/app/test

.PHONY: test
test:
	make $(TEST0)

.PHONY: run
run: $(TEST0)
	$(TEST0)
