
PROJ=NetworkAnalyzer
YICS=$(HOME)/opt/yices

EXTINCD=$(YICS)/include
EXTLIBD=$(YICS)/lib

INTINCD=../inc
INTLIBD=../lib
SRCD=../src
BIND=../bin
OBJD=../obj
EXEC=$(BIND)/$(PROJ)

LIBS = -lyices

SRCS=$(wildcard *.cpp)
OBJS=$(patsubst %.cpp, $(OBJD)/%.o, $(SRCS))
INCS=$(wildcard $(INTINCD)/*.h)

CPP = g++
CPPFLAGS = -g -O2 -I $(EXTINCD)
LDFLAGS = -g -O2 -L$(EXTLIBD)

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CPP) $(LDFLAGS) -o $@ $^ $(LIBS)   

$(OBJD)/%.o: %.cpp $(INCS)
	$(CPP) $(CPPFLAGS) -c -o $@ $< 

clean:
	rm -r $(OBJS) $(EXEC)


