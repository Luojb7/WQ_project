objects = testMdSpi.o ./thostmduserapi.so

Demo : ${objects}
	g++ -o Demo ${objects}

testMdSpi.o : testMdSpi.cpp 
	g++ -c testMdSpi.cpp

MdSpi.o : MdSpi.cpp MdSpi.h
	g++ -c MdSpi.cpp

clean:
	rm Demo testMdSpi.o MdSpi.o