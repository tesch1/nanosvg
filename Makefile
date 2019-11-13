
all:
	[ -d build/Makefile ] || cmake . -Bbuild
	cmake --build build

fuzz:
	[ -d build/Makefile ] || cmake . -Bbuild -DBUILD_FUZZ=ON
	cmake --build build --target fuzz

test:
	[ -d build/Makefile ] || cmake . -Bbuild -DSVG_ENABLE_TESTING:BOOL=ON
	cmake --build build
