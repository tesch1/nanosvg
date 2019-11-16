
all: test

test:
	[ -d build/Makefile ] || CXX=clang++ CC=clang cmake . -Bbuild -DNSVG_BUILD_TEST:BOOL=ON
	cmake --build build --target all
	cmake --build build --target test

fuzz:
	CXX=clang++ CC=clang cmake . -Bbuild-fuzz -DBUILD_FUZZ=ON
	cd build-fuzz && $(MAKE) fuzz

distclean:
	rm -rf build
