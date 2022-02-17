CFLAGS =  -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXrandr -lXi
Slimemaid: main.cpp
	g++ $(CFLAGS) -o Slimemaid main.cpp $(LDFLAGS)

.PHONY: test clean

test:	Slimemaid
	clear
	./Slimemaid

clean:
	rm -rf Slimemaid
