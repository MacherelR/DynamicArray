dynamic_array : dynamic_array.cpp
	clang++ -Wall -o dynamic_array -std=c++14 -Wno-deprecated -g dynamic_array.cpp -framework OpenGL -framework GLUT -lm

run: dynamic_array
	./dynamic_array

clean:
	rm -r dynamic_array dynamic_array.dSYM
