all: required-dependencies bin/benchmark

bin/benchmark:
	g++ -DNDEBUG -O3 -std=c++11 $(shell pkg-config --cflags --libs benchmark) -lpthread ltoa.cpp -o bin/benchmark
	
required-dependencies: .Makefile.dependencies.installed

.Makefile.dependencies.installed: Makefile.dependencies
	@echo "Installing dependencies"
	@make -f Makefile.dependencies
	@touch .Makefile.dependencies.installed
