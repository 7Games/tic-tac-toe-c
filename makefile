ifeq ($(BUILD),debug)   
# "Debug" build - no optimization, and debugging symbols
CFLAGS += -O0 -g
else
# "Release" build - optimization, and no debug symbols
CFLAGS += -O2 -s -DNDEBUG
endif

all: clean build

debug:
	make "BUILD=debug"

build:
	mkdir -p ./build/
	clang -o ./build/tictactoe ./src/main.c
buildgcc:
	mkdir -p ./build/
	gcc -o ./build/tictactoe ./src/main.c
frun: build run
run:
	./build/tictactoe
clean:
	rm -rf ./build