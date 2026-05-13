CC=gcc
CFLAGS=-Wall -Wextra -Werror -std=c11 -D_POSIX_C_SOURCE=200809L
TEST_CFLAGS=$(CFLAGS) --coverage
CURSES_LIB=-lncurses
CHECK_LIBS=$(shell pkg-config --libs check 2>/dev/null || printf '%s' '-lcheck -lm -lsubunit')
CHECK_CFLAGS=$(shell pkg-config --cflags check 2>/dev/null)

APP=brick_game_cli
BUILD_DIR=build
DIST_DIR=dist
REPORT_DIR=report
PREFIX?=$(HOME)/.local
BINDIR=$(PREFIX)/bin

LIB_SRC=$(wildcard src/brick_game/tetris/*.c)
CLI_SRC=$(wildcard src/gui/cli/*.c)
TEST_SRC=$(wildcard tests/*.c)
LIB_GCNO=$(patsubst src/brick_game/tetris/%.c,$(BUILD_DIR)/tests-%.gcno,$(LIB_SRC))

all: $(BUILD_DIR)/$(APP)

$(BUILD_DIR)/$(APP): $(LIB_SRC) $(CLI_SRC)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(CURSES_LIB)

install: all
	mkdir -p $(DESTDIR)$(BINDIR)
	cp $(BUILD_DIR)/$(APP) $(DESTDIR)$(BINDIR)/$(APP)

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(APP)

clean:
	rm -rf $(BUILD_DIR) $(DIST_DIR) $(REPORT_DIR) *.gcda *.gcno *.info
	find . -name '*.gcda' -o -name '*.gcno' | xargs -r rm -f

dvi:
	@printf 'README.md\n\ndocs/fsm.md\n'

dist:
	mkdir -p $(DIST_DIR)
	tar -czf $(DIST_DIR)/brick_game_v1.tar.gz README.md Makefile docs src tests

test:
	mkdir -p $(BUILD_DIR)
	$(CC) $(TEST_CFLAGS) $(CHECK_CFLAGS) $(LIB_SRC) $(TEST_SRC) -o $(BUILD_DIR)/tests $(CHECK_LIBS)
	$(BUILD_DIR)/tests

gcov_report: test
	@mkdir -p $(REPORT_DIR)
	@if command -v lcov >/dev/null 2>&1 && command -v genhtml >/dev/null 2>&1; then \
		lcov --capture --directory . --output-file $(REPORT_DIR)/coverage.info; \
		genhtml $(REPORT_DIR)/coverage.info --output-directory $(REPORT_DIR)/html; \
	else \
		gcov $(LIB_GCNO) | tee $(REPORT_DIR)/coverage.txt; \
		mv -f *.gcov $(REPORT_DIR)/ 2>/dev/null || true; \
	fi

.PHONY: all install uninstall clean dvi dist test gcov_report
