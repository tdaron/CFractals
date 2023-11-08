build:
	meson compile -C builddir

run:
	cd builddir && ninja && cd .. && ./builddir/mandelbrot
