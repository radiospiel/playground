BINARY_NAME=benchmark
SRC_DIR=src

.PHONY : clean all release debug dependencies

all: debug release

debug:
	@BUILD_MODE=debug make binary
release:
	@BUILD_MODE=release make binary

# --- directories and filenames

BINDIR=bin/$(BUILD_MODE)
TARGET_DIR=bin/$(BUILD_MODE)
OBJ_DIR=tmp/obj/$(BUILD_MODE)
TARGET=$(TARGET_DIR)/$(BINARY_NAME)

# --- compiler flags

CXXFLAGS:=-Wall -std=c++11 $(shell pkg-config --cflags --libs benchmark) -lpthread
ifeq ($(BUILD_MODE),debug)
CXXFLAGS:=-g -D_DEBUG $(CXXFLAGS)
endif
ifeq ($(BUILD_MODE),release)
CXXFLAGS:=-DNDEBUG -O3 $(CXXFLAGS)
endif

# --- source files

VPATH=$(SRC_DIR)
CPPFILES =  $(wildcard $(SRC_DIR)/*.cpp)
CPP_FILENAMES += $(notdir $(CPPFILES)) 
OBJ_FILES := $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(CPP_FILENAMES) )

binary: $(TARGET)

clean:
	rm -rf bin/* tmp/obj

$(TARGET): $(OBJ_FILES)
	@echo "Building in $(BUILD_MODE) mode"
	$(shell mkdir -p $(TARGET_DIR) )
	g++ -o $@ $(CXXFLAGS)  $^

$(OBJ_DIR)/%.o : %.cpp
	$(shell mkdir -p $(OBJ_DIR) )
	$(CXX) $(CXXFLAGS) -Wno-unused-command-line-argument -c $< -o $@
	
dependencies: .Makefile.dependencies.installed

.Makefile.dependencies.installed: Makefile.dependencies
	@echo "Installing dependencies"
	@make -f Makefile.dependencies
	@touch .Makefile.dependencies.installed
