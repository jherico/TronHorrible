DEBUG         = 0
CXX           = g++
CXXFLAGS 	  = -fPIC -pipe $(DEFINES) -std=c++0x -std=gnu++0x
MD            = mkdir
DELETEFILE    = rm -f
SYSARCH       = $(shell uname -m)
CUSTOMPATH    = $(RELEASETYPE)/$(SYSARCH)

ifeq ($(DEBUG), 1)
	CXXFLAGS      += -DDEBUG -g3
	RELEASETYPE   = Debug
else
	CXXFLAGS      += -O2 
	RELEASETYPE   = Release
endif


TARGETPATH    = Build/$(CUSTOMPATH)
TARGET        = $(TARGETPATH)/TronHorrible
SRCPATH       = Src
OBJPATH       = $(TARGETPATH)

# Filter out Windows and OSX files
LIBS = -lX11 -lGLESv2 -lEGL  -lEncomGl -ljson -lboost_thread -lboost_filesystem -lopencv_imgproc -lopencv_core -lopencv_highgui -lopencv_calib3d -lboost_system -lOvrNsb 
SRCS = $(shell find Src -name "*.cpp")
INCS = -I../EncomGL -I../OculusSDK/nsb/libovr_rift  
OBJS = $(patsubst ${SRCPATH}/%.cpp, ${OBJPATH}/%.o, ${SRCS})
DEPS = $(patsubst ${SRCPATH}/%.cpp, ${OBJPATH}/%.d, ${SRCS})
DIRS = $(subst /,/,$(sort $(dir $(OBJECTS)))) $(TARGETPATH)
CH = $(SRCPATH)/common.hpp
PCH = $(CH).gch


CPP_COMPILE = g++ -I$(OBJPATH) $(INCS) $(CXXFLAGS) -Wall -c -fmessage-length=0 -fPIC

# All Target
all: $(TARGET)

foo:
	@echo $(DIRS)
	@echo $(SRCS)

# Tool invocations
$(TARGET): $(OBJS) $(PCH)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C++ Linker'
	g++ -o $(TARGET) $(OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

$(OBJPATH)/%.o: ${SRCPATH}/%.cpp $(PCH)
	$(MD) -p $(DIRS)
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CPP_COMPILE) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

$(PCH): $(CH)
	$(MD) -p $(DIRS)
	$(CPP_COMPILE) -o $(PCH) $(CH)

# Other Targets
clean:
	-$(RM) $(OBJS) $(DEPS) $(LIBRARIES) $(PCH) $(TARGET)
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

