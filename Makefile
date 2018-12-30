wichteln: wichteln.cc
	g++ -Wall -Wextra -O3 -pedantic -std=c++11  -o $@ $^

clean:
	rm -f wichteln

.PHONY: clean
