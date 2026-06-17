.PHONY: run

run:
	meson compile -C build && ./build/joltman
