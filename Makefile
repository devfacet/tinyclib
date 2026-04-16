.DEFAULT_GOAL := help

CLANG_FORMAT := $(shell if command -v clang-format >/dev/null 2>&1; then echo clang-format; fi)
CLANG_TIDY := $(shell if command -v clang-tidy >/dev/null 2>&1; then echo clang-tidy; fi)
CPPCHECK := $(shell if command -v cppcheck >/dev/null 2>&1; then echo cppcheck; fi)
CLANG_TIDY_EXTRA_ARGS := $(shell if [ "$$(uname)" = "Darwin" ]; then echo "--extra-arg=--sysroot=$$(xcrun --show-sdk-path)"; fi)

SRC_FILES := src/*.c include/*.h
TEST_FILES := tests/unit/*.c
CMD_FILES := $(wildcard cmd/*/*.c cmd/*/*.h)
ALL_FILES := $(SRC_FILES) $(TEST_FILES) $(CMD_FILES)

PRESET ?= default
JOBS ?= 4
PROJECT_DIR := $(CURDIR)
BUILD_ROOT := build
PRESET_BUILD_DIR := $(if $(filter default,$(PRESET)),$(BUILD_ROOT),$(BUILD_ROOT)/$(PRESET))

.PHONY: help configure build clean clean-bin clean-cache test format lint check check-all fix

help: ## Show available make targets
	@echo "Usage: make <target>"
	@echo ""
	@echo "Targets:"
	@awk 'BEGIN {FS = ":.*## "} /^[a-zA-Z_-]+:.*## / {printf "  %-15s %s\n", $$1, $$2}' $(MAKEFILE_LIST)

configure: ## Configure cmake (use PRESET=release for release mode)
	cmake --preset $(PRESET)

build: ## Build the project (use PRESET=release for release mode)
	cmake --preset $(PRESET)
	cmake --build --preset $(PRESET) -j $(JOBS)

clean: ## Remove build directories
	@test -n "$(PROJECT_DIR)" && [ "$(PROJECT_DIR)" != "/" ]
	rm -rf "$(PROJECT_DIR)/$(BUILD_ROOT)"

clean-bin: ## Remove built binaries for the selected preset
	@test -n "$(PROJECT_DIR)" && [ "$(PROJECT_DIR)" != "/" ]
	@test -d "$(PROJECT_DIR)/$(PRESET_BUILD_DIR)/cmd/bin" || exit 0
	find "$(PROJECT_DIR)/$(PRESET_BUILD_DIR)/cmd/bin" -mindepth 1 -delete

clean-cache: ## Remove CMake cache for the selected preset
	@test -n "$(PROJECT_DIR)" && [ "$(PROJECT_DIR)" != "/" ]
	rm -f "$(PROJECT_DIR)/$(PRESET_BUILD_DIR)/CMakeCache.txt"

test: ## Run tests
	@test -f "$(PROJECT_DIR)/$(PRESET_BUILD_DIR)/CMakeCache.txt" || cmake --preset $(PRESET)
	cmake --build --preset $(PRESET) -j $(JOBS)
	ctest --preset $(PRESET)

format: ## Check code formatting
	@test -n "$(CLANG_FORMAT)" || { echo "error: clang-format not found"; exit 1; }
	$(CLANG_FORMAT) --dry-run --Werror $(ALL_FILES) --verbose

lint: ## Check code linting
	@test -n "$(CLANG_TIDY)" || { echo "error: clang-tidy not found"; exit 1; }
	@test -f "$(PRESET_BUILD_DIR)/compile_commands.json" || cmake --preset default
	$(CLANG_TIDY) --config-file=$(PROJECT_DIR)/.clang-tidy -p $(PRESET_BUILD_DIR) $(CLANG_TIDY_EXTRA_ARGS) \
		--header-filter="^$(PROJECT_DIR)/(src|include|tests)/" src/*.c tests/unit/*.c

check: ## Static analysis
	@test -n "$(CPPCHECK)" || { echo "error: cppcheck not found"; exit 1; }
	@test -f "$(PRESET_BUILD_DIR)/compile_commands.json" || cmake --preset default
	$(CPPCHECK) --enable=warning,style,performance,portability --error-exitcode=1 \
		--check-level=exhaustive --project=$(PRESET_BUILD_DIR)/compile_commands.json \
		--suppress=missingIncludeSystem -i$(PRESET_BUILD_DIR)

check-all: test format lint check ## Run all checks

fix: ## Fix code formatting and linting issues
	@test -n "$(CLANG_FORMAT)" || { echo "error: clang-format not found"; exit 1; }
	$(CLANG_FORMAT) -i $(ALL_FILES)
