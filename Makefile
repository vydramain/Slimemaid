STB_INCLUDE_PATH = ./libraries/stb
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXrandr -lXi
CFLAGS = -std=c++17 -O2 -I$(STB_INCLUDE_PATH)

clean:
	rm -rf ./build

compile_shaders: ./shaders/*
	./compile_shaders.sh

prepare_raws: ./raws/*
	cp -r ./raws ./build/raws

build:	clean compile_shaders prepare_raws ./source/main.cpp
	mkdir -p build && g++ $(CFLAGS) -o ./build/Slimemaid ./source/main.cpp $(LDFLAGS) -g

.PHONY: clean compile_shaders prepare_raws test

test:	build
	clear && cd ./build && ./Slimemaid
