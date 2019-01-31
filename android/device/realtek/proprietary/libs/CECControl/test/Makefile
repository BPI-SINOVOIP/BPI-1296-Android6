
##############################################
# Tool Chain
##############################################
include ../../../../include/MakeConfig



##############################################
# Environment
##############################################
SRC     = CECmoduleTest.cpp auxfun.cpp RTK_DVR.cpp
OBJ     = $(SRC:.cpp=.o)
INC     = $(SRC:.cpp=.h)

INCS    = -I . -I ../../../Include/ -I inc/
INCS   += -I ../../../Include/Platform_Lib/CECControl/

LIBS    = -L ../../../../lib
LIBS   += -lCECControl
LIBS   += -lOSAL -lpthread -lStreamingEngine
LIBS   += -lPlatformCommon
LIBS   += -lMoveDataEngine
        
MYPROG  = cec_test

VPATH = src inc obj

LDFLAGS += -static

##############################################
# Make
##############################################
all : $(MYPROG)    
    
.cpp.o:
	$(MYCXX) -g $(INCS) -c $< -o obj/$@		

$(MYPROG): $(OBJ)
	@cp obj/*.o .	
	$(MYCXX) $(LDFLAGS) $^ $(LIBS) -o $(MYPROG)	
	@rm *.o
		
link: RM all
RM : 
	@rm -rf $(MYPROG)

clean: 
	@rm -rf *.bak *~ *.o	
	@rm -rf inc/*.bak inc/*.~
	@rm -rf obj/*
	@rm -rf src/*.bak src/*.~
	@rm -rf $(MYPROG)

