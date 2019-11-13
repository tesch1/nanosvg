
all:
	[ -d build ] || cmake . -Bbuild
	cmake --build build

test:
	[ -d build ] || cmake . -Bbuild -DSVG_ENABLE_TESTING:BOOL=ON
	cmake --build build
