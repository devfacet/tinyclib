.DEFAULT_GOAL := help

BUILD_DIR := build
CLANG_FORMAT := $(shell if command -v clang-format >/dev/null 2>&1; then echo clang-format; fi)
CLANG_TIDY := $(shell if command -v clang-tidy >/dev/null 2>&1; then echo clang-tidy; fi)
CPPCHECK := $(shell if command -v cppcheck >/dev/null 2>&1; then echo cppcheck; fi)
CLANG_TIDY_EXTRA_ARGS := $(shell if [ "$$(uname)" = "Darwin" ]; then echo "--extra-arg=--sysroot=$$(xcrun --show-sdk-path)"; fi)

SRC_FILES := src/*.c include/*.h
TEST_FILES := tests/unit/*.c
EXAMPLE_FILES := $(wildcard examples/*/*.c)
ALL_FILES := $(SRC_FILES) $(TEST_FILES) $(EXAMPLE_FILES)

.PHONY: help install build clean clean-bin test format lint check check-all fix

help: ## Show available make targets
	@echo "Usage: make <target>"
	@echo ""
	@echo "Targets:"
	@awk 'BEGIN {FS = ":.*## "} /^[a-zA-Z_-]+:.*## / {printf "  %-15s %s\n", $$1, $$2}' $(MAKEFILE_LIST)

configure: ## Configure cmake
	cmake --preset default

build: ## Build the project
	@test -d "$(BUILD_DIR)" || cmake --preset default
	cmake --build --preset default

clean: ## Remove build directory
	@test -n "$(CURDIR)" && [ "$(CURDIR)" != "/" ]
	rm -rf "$(CURDIR)/$(BUILD_DIR)"

clean-bin: ## Remove built binaries from the build directory
	@test -n "$(CURDIR)" && [ "$(CURDIR)" != "/" ]
	@test -d "$(CURDIR)/$(BUILD_DIR)/bin" || exit 0
	find "$(CURDIR)/$(BUILD_DIR)/bin" -mindepth 1 -delete

test: ## Run tests
	ctest --preset default

format: ## Check code formatting
	@test -n "$(CLANG_FORMAT)" || { echo "error: clang-format not found"; exit 1; }
	$(CLANG_FORMAT) --dry-run --Werror $(ALL_FILES) --verbose

lint: ## Check code linting
	@test -n "$(CLANG_TIDY)" || { echo "error: clang-tidy not found"; exit 1; }
	$(CLANG_TIDY) -p $(BUILD_DIR) $(CLANG_TIDY_EXTRA_ARGS) \
		--header-filter="^$(CURDIR)/(src|include|tests)/" src/*.c tests/unit/*.c

check: ## Static analysis
	@test -n "$(CPPCHECK)" || { echo "error: cppcheck not found"; exit 1; }
	$(CPPCHECK) --enable=warning,style,performance,portability --error-exitcode=1 \
		--check-level=exhaustive --project=$(BUILD_DIR)/compile_commands.json \
		--suppress=missingIncludeSystem -i$(BUILD_DIR)

check-all: format lint check ## Run all checks

fix: ## Fix code formatting and linting issues
	@test -n "$(CLANG_FORMAT)" || { echo "error: clang-format not found"; exit 1; }
	$(CLANG_FORMAT) -i $(ALL_FILES)
