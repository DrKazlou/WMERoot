


#CXX           = g++ 

#FLAGS =  -fPIC -DLINUX -O2 -g -I/usr/include -I./include

CXX = g++ -fPIC -DLINUX -I./include

#DEPLIBS	=	-lCAENDPPLib -lm -lc -lrt -lpthread

#LIBS	=	-L/usr/lib

ROOTINCL  := $(shell root-config --incdir)
ROOTLIB  := $(shell root-config --libdir)


LIBS          := -L -lpthread -lutil -L $(ROOTLIB) -lGeom -lRGL -lGed -lTreePlayer -lCore -lHist -lGraf -lGraf3d -lMathCore -lGpad -lTree -lRint -lRIO -lPostscript -lMatrix -lPhysics -lMinuit -lGui -lASImage -lASImageGui -pthread -lm -ldl -rdynamic -lstdc++

#INCLUDES = /usr/include/CAENDPPLib.h /usr/include/CAENDPPLibTypes.h ./include/*

.PHONY: all clean
	
all: clean WMERoot
	
clean:
		@rm -rf WMERoot *.o


guiDict.cxx : include/WMEFrame.h include/WMEOpt.h include/LinkDef.h
	@rootcling -f $@ $^
	$(info [-10%]  Dictionary)	

WMERoot:  WMEFunc.o WMEFrame.o WMEOpt.o WMERoot.o
		$(info [70%]  Linking)
		@$(CXX) -o WMERoot src/WMERoot.o guiDict.cxx src/WMEFrame.o src/WMEOpt.o src/WMEFunc.o $(LIBS) `root-config --cflags --glibs`
		$(info [100%] Built target WMERoot)
	

WMEFunc.o:  src/WMEFunc.c
		$(info [10%] Generating WMEFunc.o)
		@$(CXX) -o src/WMEFunc.o -c src/WMEFunc.c	`root-config --cflags --glibs`	
		

WMEFrame.o:  src/WMEFrame.c
		$(info [30%] Generating WMEFrame.o)
		@$(CXX) -o src/WMEFrame.o -c src/WMEFrame.c `root-config --cflags --glibs`		
		

WMEOpt.o: src/WMEOpt.c guiDict.cxx
		$(info [50%] Generation WMEOpt.o)
		@$(CXX) -o src/WMEOpt.o -c src/WMEOpt.c `root-config --cflags --glibs`			
		

WMERoot.o: src/WMERoot.c guiDict.cxx
		$(info [70%] Generation WMERoot.o)
		@$(CXX) -o src/WMERoot.o -c src/WMERoot.c `root-config --cflags --glibs`	
		
