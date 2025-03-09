all: clean reload compile_shaders prepare_raws build exec
clean:
	rm -rf cmake-build-debug

mkdir_build:
	[ -d ./cmake-build-debug ] | mkdir -p cmake-build-debug

compile_shaders: ./shaders/*
	./compile_shaders.sh

prepare_raws: ./raws/*
	cp -r ./raws ./cmake-build-debug/

build:
	cd cmake-build-debug;make -j$(nproc)

exec:
	cd cmake-build-debug;./Slimemaid

reload: mkdir_build
	cd cmake-build-debug;cmake ..

fast_build:
	cd cmake-build-debug;make -j$(nproc)

test:
	cd cmake-build-debug;make test

run: fast_build exec

