
all: Makefile echo

echo:
	echo "stdlib make done"

fmt:
	clang-format -i contractlib/v1/*.h contractlib/v1/*.hpp contractlib/v1/*.cpp cpp_tests/*.cpp cpp_tests/*.hpp

fmt_check:
	clang-format --dry-run --Werror contractlib/v1/*.h contractlib/v1/*.hpp contractlib/v1/*.cpp cpp_tests/*.cpp cpp_tests/*.hpp

.DEFAULT_GOAL := error

error:
	@echo "Error: You must specify either 'make fmt/fmt_check'"
	@exit 1
