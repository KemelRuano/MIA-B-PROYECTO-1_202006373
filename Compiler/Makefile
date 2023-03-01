OBJECTS := main.o
INCLUDES := ../SourceFiles/*.cpp

execute : $(OBJECTS)
	g++ -o $@ $^ $(INCLUDES)
	rm -f *.o

*.o: %.cpp
	g++ -o $@ -c $^ -I
