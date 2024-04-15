INCLUDE_DIRS=. test/include
INC_PARAMS=$(foreach d, $(INCLUDE_DIRS), -I$d)

CC=clang
CXX=clang
LINTER=clang-tidy
MC_EXPORT="-fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls"
ASAN_EXPORT="symbolize=1 ASAN_OPTIONS=fast_unwind_on_malloc=0 ASAN_SYMBOLIZER_PATH=$(shell which llvm-symbolizer)"
CFLAGS=-x c++ -c -Wall -Wpedantic -Werror -std=c++17 -fno-elide-constructors -fno-inline $(MC_FLAGS) $(INC_PARAMS) -g -D_GLIBCXX_DEBUG -O0
LDFLAGS=-lstdc++ -lm $(MC_FLAGS)# -lprofiler

# Use lld if available for faster link times
ifeq ($(shell uname -s),Linux)
  LDFLAGS:=$(LDFLAGS) -fuse-ld=lld
endif

ifdef COVERAGE
  CC=g++
  CXX=g++
  CFLAGS:=$(CFLAGS) --coverage
  LDFLAGS:=$(LDFLAGS) --coverage
endif

$(info +---------------------------------------------------------------------)
ifdef FIXTURES
  $(info | Environment detected as CodeGrade)
  CC=clang-10
  CXX=clang-10
  LINTER=clang-tidy-10
  MC_EXPORT="-fsanitize=address"
  ASAN_EXPORT="symbolize=0"
  CFLAGS=-x c++ -c -Wall -Wpedantic -Werror -std=c++17 $(MC_FLAGS) $(INC_PARAMS) -O0
  LDFLAGS=-lstdc++ -lm -fuse-ld=lld $(MC_FLAGS)
else ifeq ($(shell uname -s),Linux)
  $(info | Environment detected as Linux/WSL)
  $(info | $(shell uname -av))
  $(info | $(shell lsb_release -d))
else
  $(info | Environment detected as macOS)
  $(info | $(shell uname -av))
endif

$(info | * compiler: $(CC))
$(info | * linker: $(CXX))
$(info | * linter: $(LINTER))
$(info | * home: $(HOME))
$(info | * fixtures: $(FIXTURES))
$(info | * memcheck compile and link flags set to: $(MC_EXPORT))
$(info | * asan flags set to: $(ASAN_EXPORT))
$(info +---------------------------------------------------------------------)

SRC:=$(shell find src -name '*.cpp')
OBJECTS=$(SRC:.cpp=.o)

TEST_SRC=$(shell find test -name '*.cpp')
TEST_OBJECTS=$(TEST_SRC:.cpp=.o)
TEST_EXECUTABLE=runtests

APP_SRC=main.cpp
APP_OBJECTS=$(APP_SRC:.cpp=.o)
APP_EXECUTABLE=app

ZIP_FILE=rbtree-assignment

all: $(SRC) $(APP_SRC) $(APP_EXECUTABLE) $(TEST_SRC) $(TEST_EXECUTABLE)

$(TEST_EXECUTABLE): $(TEST_OBJECTS) $(OBJECTS)
	@echo "Linking test executables"
	$(CXX) $(LDFLAGS) $(TEST_OBJECTS) $(OBJECTS) -o $@

$(APP_EXECUTABLE): $(APP_OBJECTS) $(OBJECTS)
	@echo "Linking app executables"
	$(CXX) $(LDFLAGS) $(APP_OBJECTS) $(OBJECTS) -o $@

.cpp.o:
	@echo "Compiling object files"
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	@echo "Cleaning recreatable files:"
	@echo "  * Executables"
	rm -f $(APP_EXECUTABLE)
	rm -f $(TEST_EXECUTABLE)
	@echo "  * Zip files"
	rm -f $(ZIP_FILE)*.zip
	@echo "  * Data files from compiler, used for test coverage measurements"
	find . -name "*.gcda" -print0 | xargs -0 rm -f
	find . -name "*.gcno" -print0 | xargs -0 rm -f
	find . -name "*.gcov" -print0 | xargs -0 rm -f
	rm -f coverage.info
	@echo "  * Object files"
	find . -name "*.o" -print0 | xargs -0 rm -f
	@echo "  * Other files"
	find . -name "*.txt" -print0 | xargs -0 rm -f
	rm -rf out

.PHONY: test
test: all
	./$(TEST_EXECUTABLE) -a -b

.PHONY: memcheck
memcheck:
	MC_FLAGS=$(MC_EXPORT) $(MAKE) clean all
	ASAN_OPTIONS=$(ASAN_EXPORT) ./$(TEST_EXECUTABLE)

.PHONY: static-code-analysis
static-code-analysis:
	$(LINTER) --header-filter=".*.hpp" --warnings-as-errors='*' --checks="cppcoreguidelines-*, bugprone*, diagnostic-*, -clang-diagnostic-c++20-extensions, analyzer-*, readability-*, -cppcoreguidelines-pro-bounds-pointer-arithmetic, -cppcoreguidelines-special-member-functions, -cppcoreguidelines-owning-memory" ${SRC} ${APP_SRC} -- ${INC_PARAMS}


ifndef OVERRIDE
  OVERRIDE=false
endif
.PHONY: zip
zip:
	${OVERRIDE} || ${MAKE} clean
	${OVERRIDE} || ${MAKE} all
	${OVERRIDE} || ${MAKE} test
	${MAKE} clean
	${MAKE} $(ZIP_FILE)

$(ZIP_FILE):
	zip -r $(ZIP_FILE)-${shell git branch --show-current}-${shell git describe}.zip . -x '*.git*' -x '*.zip' -x 'out/*' -x '*.html' -x 'vs/.vs/*' -x 'vs/x64/*' -x 'vs/*.vcxproj.user'