STB_INCLUDE_PATH = ./libraries/stb
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXrandr -lXi
CFLAGS = -std=c++17 -O2 -I$(STB_INCLUDE_PATH)

compile_shaders: ./shaders/*
	./compile_shaders.sh

clean:
	rm -rf ./build

build:	clean compile_shaders ./source/main.cpp
	mkdir -p build && g++ $(CFLAGS) -o ./build/Slimemaid ./source/main.cpp $(LDFLAGS) -g

.PHONY: clean compile_shaders test

test:	build
	clear && cd ./build && ./Slimemaid
