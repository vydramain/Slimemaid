CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXrandr -lXi
build:	clean ./source/main.cpp
	mkdir -p build
	./compile_shaders.sh
	g++ $(CFLAGS) -o ./build/Slimemaid ./source/main.cpp $(LDFLAGS) -g

.PHONY: test clean

test:	build
	clear
	./build/Slimemaid

clean:
	rm -rf ./build
