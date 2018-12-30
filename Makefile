wichteln: wichteln.cc wichtel.h argparse.hpp
	g++ -Wall -Wextra -O3 -pedantic -std=c++11  -o $@ wichteln.cc

clean:
	rm -f wichteln

.PHONY: clean
