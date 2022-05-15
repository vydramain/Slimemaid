STB_INCLUDE_PATH = ./libraries/stb
SPDLOG_INCLUDE_PATH = ./libraries/spdlog
TINYOBJ_INCLUDE_PATH = ./libraries/tinyobjloader
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXrandr -lXi
CFLAGS = -std=c++17 -I$(STB_INCLUDE_PATH) -I$(SPDLOG_INCLUDE_PATH) -I$(TINYOBJ_INCLUDE_PATH)
CFLAGS_TEST = -std=c++17 -Wall -Wextra -pedantic -I$(STB_INCLUDE_PATH) -I$(SPDLOG_INCLUDE_PATH) -I$(TINYOBJ_INCLUDE_PATH)


all: clean reload compile_shaders prepare_raws build exec

mkdir_build:
	[ -d ./cmake-build-debug ] | mkdir -p cmake-build-debug

compile_shaders: ./shaders/*
	./compile_shaders.sh

prepare_raws: ./raws/*
	cp -r ./raws ./cmake-build-debug/raws

build:
	cd cmake-build-debug;make

exec:
	cd cmake-build-debug;./Slimemaid

clean:
	rm -rf cmake-build-debug

reload: mkdir_build
	cd cmake-build-debug;cmake ..

fast_build:
	cd cmake-build-debug;make

test:
	cd cmake-build-debug;make test

run: fast_build exec
