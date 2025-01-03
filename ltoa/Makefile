all: required-dependencies debug release

clean:
	rm -rf bin/*

debug: bin/debug/benchmark Makefile
release: bin/release/benchmark Makefile

bin/release/benchmark: src/ltoa.cpp
	mkdir -p bin/release
	g++ -DNDEBUG -Wall -O3 -std=c++11 $(shell pkg-config --cflags --libs benchmark) -lpthread $<  -o $@

bin/debug/benchmark: src/ltoa.cpp
	mkdir -p bin/debug
	g++ -g -D_DEBUG -Wall -std=c++11 $(shell pkg-config --cflags --libs benchmark) -lpthread $<  -o $@
	
required-dependencies: .Makefile.dependencies.installed

.Makefile.dependencies.installed: Makefile.dependencies
	@echo "Installing dependencies"
	@make -f Makefile.dependencies
	@touch .Makefile.dependencies.installed
