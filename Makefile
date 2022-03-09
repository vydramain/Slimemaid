CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXrandr -lXi
build: ./source/main.cpp
	g++ $(CFLAGS) -o Slimemaid ./source/main.cpp $(LDFLAGS) -g

.PHONY: test clean

test:	build
	clear
	./Slimemaid

clean:
	rm -rf Slimemaid
