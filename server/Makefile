CPP = g++
CFLAGS = -lrt -g # -Wall
INCLDS = -I../vmelib/
LIBS = -L ../vmelib/ -lvmelib -lpthread

#SOURCES = main.cpp param.cpp readout_class.cpp part_iol.cpp 
SOURCES = main.cpp  

DEPS = main.o  
#aio_module_def.o	


server: $(DEPS)
	$(CPP) $(CFLAGS) $(INCLDS) $(LIBS) -o server $(SOURCES) $(LIBS)



create_tcl: create_tcl.o 
	$(CPP) $(CFLAGS) -o create_tcl -lncurses -o create_tcl create_tcl.cpp param.cpp

create_tcl.o: create_tcl.cpp create_tcl.h
	$(CPP) -c create_tcl.cpp


#aio_module_def.o: aio_module_def.cpp
#	$(CPP) -c aio_module_def.cpp
readoutclass.o: readout_class.cpp
	$(CPP) -c readout_class.cpp
part_iol.o: part_iol.cpp
	$(CPP) -c part_iol.cpp

main.o: main.cpp
	$(CPP) -c main.cpp

clean:
	rm -f *.o $(EXECS)
