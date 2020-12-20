sim: main.o
	g++ -o sim main.o -lm
	rm *.o

main.o: main.cpp
	g++ -c main.cpp


clean:
	rm -f sim *.o
