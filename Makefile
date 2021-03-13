# compilation, flags, and linking

CXX = clang++

CXXFLAGS = -std=c++2a \
		   -Wall \
		   -fimplicit-modules \
		   -fimplicit-module-maps \
		   -Iinclude

# configuration

BUILD_DIR ?= build

# source and object files

HDRS = include/chanchan/chanchan.h

TEST_SRCS = test/chanchan_test.cpp

TEST_OBJS = $(addprefix $(BUILD_DIR)/,$(notdir $(TEST_SRCS:.cpp=.o)))

# rules

$(TEST_OBJS): $(BUILD_DIR) $(HDRS)
$(TEST_OBJS): $(TEST_SRCS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/main_test: $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(TEST_OBJS)

build:
	mkdir -p $@

# phonies

.PHONY: runtest
runtest: $(BUILD_DIR)/main_test
	$(BUILD_DIR)/main_test

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
