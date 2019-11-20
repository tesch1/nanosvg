
all: test

test:
	[ -d build/Makefile ] || CXX=clang++ CC=clang cmake . -Bbuild -DNSVG_BUILD_TEST:BOOL=ON
	cmake --build build --target all
	cmake --build build --target test

fuzz:
	CXX=clang++ CC=clang cmake . -Bbuild-fuzz -DNSVG_BUILD_FUZZ=ON -DCMAKE_BUILD_TYPE=Release
	cd build-fuzz && $(MAKE) fuzz

distclean:
	rm -rf build
