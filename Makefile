.PHONY: all clean
all: build
	make -C build
clean:
	rm -rf *~ __pycache__ *.pyc
build:
	mkdir build && cd build && cmake ..



