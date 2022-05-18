all: clean reload compile_shaders prepare_raws build exec

mkdir_build:
	[ -d ./cmake-build-debug ] | mkdir -p cmake-build-debug

compile_shaders: ./shaders/*
	./compile_shaders.sh

prepare_raws: ./raws/*
	cp -r ./raws ./cmake-build-debug/

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

