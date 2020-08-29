.PHONY: clean

all: DSE

DSE: 431project.cpp 431project.h 431projectUtils.cpp YOURCODEHERE.cpp
	g++ -O3 431project.cpp 431projectUtils.cpp YOURCODEHERE.cpp -lm -o DSE

clean:
	-rm DSE