sampleobjects = buffer_manager.o file_manager.o sample_run.o

testrun: buffer_manager.o file_manager.o testt.o rtree.o bulk.o
	g++ -std=c++11 -o testrun buffer_manager.o file_manager.o testt.o rtree.o bulk.o

sample_run : $(sampleobjects)
	g++ -std=c++11 -o sample_run $(sampleobjects)

testt.o: testAllocateNode.cpp
	g++ -std=c++11 -o testt.o -c testAllocateNode.cpp

rtree.o: Rtree.cpp
	g++ -Wall -Wextra -pedantic -std=c++11 -O2 -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -Wshift-overflow=2 -Wduplicated-cond -Wcast-qual -Wcast-align -o rtree.o -c Rtree.cpp 

bulk.o: bulk_load.cpp
	g++ -Wall -Wextra -pedantic -std=c++11 -O2 -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -Wshift-overflow=2 -Wduplicated-cond -Wcast-qual -Wcast-align -o bulk.o -c bulk_load.cpp 

sample_run.o : sample_run.cpp
	g++ -std=c++11 -c sample_run.cpp

buffer_manager.o : buffer_manager.cpp
	g++ -std=c++11 -c buffer_manager.cpp 

file_manager.o : file_manager.cpp
	g++ -std=c++11 -c file_manager.cpp 




clean :
	rm -f *.o
	rm -f sample_run
	rm -f testrun
	rm -f temp.txt
