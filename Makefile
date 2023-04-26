.PHONY: build
build:
	mkdir -p build && cd build && cmake .. && cmake --build .

.PHONY: tests
tests:
	mkdir -p tests/build && cd tests/build && cmake .. && cmake --build .
	tests/build/tests -s

coverage:
	lcov --capture --directory . --output-file /tmp/coverage.info
	lcov --remove /tmp/coverage.info '*tests*' '*include*' -o /tmp/coverage.info

coverage-html: coverage
	genhtml /tmp/coverage.info --output-directory /tmp/lcov
	open /tmp/lcov/index.html
