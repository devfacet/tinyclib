.PHONY: help install build clean test format lint check check-all fix

BUILD_DIR := build
CLANG_FORMAT := $(shell if command -v clang-format >/dev/null 2>&1; then echo clang-format; fi)
CLANG_TIDY := $(shell if command -v clang-tidy >/dev/null 2>&1; then echo clang-tidy; fi)
CPPCHECK := $(shell if command -v cppcheck >/dev/null 2>&1; then echo cppcheck; fi)
CLANG_TIDY_EXTRA_ARGS := $(shell if [ "$$(uname)" = "Darwin" ]; then echo "--extra-arg=--sysroot=$$(xcrun --show-sdk-path)"; fi)

SRC_FILES := src/*.c include/*.h
TEST_FILES := tests/arch/generic/*.c
ALL_FILES := $(SRC_FILES) $(TEST_FILES)

help: ## Show available make targets
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | \
		awk 'BEGIN {FS = ":.*?## "}; {printf "  %-15s %s\n", $$1, $$2}'

configure: ## Configure cmake
	cmake --preset default

build: ## Build the project
	cmake --build --preset default

clean: ## Remove build directory
	@test -n "$(CURDIR)" && [ "$(CURDIR)" != "/" ]
	rm -rf "$(CURDIR)/$(BUILD_DIR)"

test: ## Build and run tests
	cmake --workflow --preset default

format: ## Check code formatting
	@test -n "$(CLANG_FORMAT)" || { echo "error: clang-format not found"; exit 1; }
	$(CLANG_FORMAT) --dry-run --Werror $(ALL_FILES) --verbose

lint: ## Check code linting
	@test -n "$(CLANG_TIDY)" || { echo "error: clang-tidy not found"; exit 1; }
	$(CLANG_TIDY) -p $(BUILD_DIR) $(CLANG_TIDY_EXTRA_ARGS) \
		--header-filter="^$(CURDIR)/(src|include|tests)/" src/*.c tests/arch/generic/*.c

check: ## Static analysis
	@test -n "$(CPPCHECK)" || { echo "error: cppcheck not found"; exit 1; }
	$(CPPCHECK) --enable=warning,style,performance,portability --error-exitcode=1 \
		--project=$(BUILD_DIR)/compile_commands.json --suppress=missingIncludeSystem \
		-i$(BUILD_DIR)

check-all: format lint check ## Run all checks

fix: ## Fix code formatting and linting issues
	@test -n "$(CLANG_FORMAT)" || { echo "error: clang-format not found"; exit 1; }
	$(CLANG_FORMAT) -i $(ALL_FILES)
