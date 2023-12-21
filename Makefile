.PHONY: build-solution build-debug build-release clean clean-all

build-solution:
	@mkdir -p build
	@cmake -S . -B build -G "Visual Studio 17 2022"

build-debug: build-solution
	@cmake --build build --config Debug

build-release: build-solution
	@cmake --build build --config Release

clean:
	@rm -rf bin

clean-all: clean
	@rm -rf build
