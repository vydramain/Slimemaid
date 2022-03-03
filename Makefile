CFLAGS =  -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXrandr -lXi
build: main.cpp
	g++ $(CFLAGS) -o Slimemaid main.cpp $(LDFLAGS) -g

.PHONY: test clean

test:	build
	clear
	./Slimemaid

clean:
	rm -rf Slimemaid
