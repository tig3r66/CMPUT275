all:	complex_postfix.o student_hash.o
	g++ complex_postfix.o -o calc
	g++ student_hash.o -o student
student: student_hash.o
	g++ student_hash.o -o student
calc: complex_postfix.o
	g++ complex_postfix.o -o calc
complex_postfix.o: complex_postfix.cpp complex_postfix.h
	g++ complex_postfix.cpp -o complex_postfix.o -c -Wall
student_hash.o: student_hash.cpp
	g++ student_hash.cpp -o student_hash.o -c -Wall
clean:
	rm -f student_hash.o complex_postfix.o student calc

