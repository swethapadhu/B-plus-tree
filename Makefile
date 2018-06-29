all:	treesearch

prog1: treesearch.cpp
	g++ -o treesearch treesearch.cpp
clean:
	rm -rf treesearch
