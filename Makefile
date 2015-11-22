objects = positive.o positiveHttp.o 
command = g++ -lpthread
positive:$(objects)
	$(command)  $(objects) -o positive
positive.o: positive.cpp positive.h
	$(command) -c  positive.cpp
positiveHttp.o: positiveHttp.cpp positiveHttp.h
	$(command) -c  positiveHttp.cpp
debug:
	$(command) -g $(objects) -o positive
clean:
	rm $(objects) positive