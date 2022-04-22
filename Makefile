STB_INCLUDE_PATH = ./libraries/stb
TINYOBJ_INCLUDE_PATH = ./libraries/tinyobjloader
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXrandr -lXi
CFLAGS = -std=c++17 -I$(STB_INCLUDE_PATH) -I$(TINYOBJ_INCLUDE_PATH)

clean:
	rm -rf ./build

compile_shaders: ./shaders/*
	./compile_shaders.sh

prepare_raws: ./raws/*
	cp -r ./raws ./build/raws

build:	clean compile_shaders prepare_raws ./source/main.cpp
	mkdir -p build && g++ $(CFLAGS) -o ./build/Slimemaid ./source/main.cpp $(LDFLAGS) -g -O2

release_build: clean compile_shaders prepare_raws ./source/main.cpp
		mkdir -p build && g++ $(CFLAGS) -o ./build/Slimemaid ./source/main.cpp $(LDFLAGS) -O8

.PHONY: clean compile_shaders prepare_raws test

test:	build
	clear && cd ./build && ./Slimemaid
