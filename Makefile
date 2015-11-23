objects = positive.o positiveHttp.o 
compile = g++ -lpthread
positive:$(objects)
	$(compile)  $(objects) -o positive
	rm $(objects)
positive.o: positive.cpp positive.h
	$(compile)  -c positive.cpp
positiveHttp.o: positiveHttp.cpp positiveHttp.h
	$(compile)  -c positiveHttp.cpp
debug:
	$(compile)  -c positive.cpp
	$(compile)  -c positiveHttp.cpp
	$(compile) -g $(objects) -o positive
	rm $(objects)
clean:
	rm positive