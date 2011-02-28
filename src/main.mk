
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
CPPFLAGS = -O0 -g3 -Wall -I $(EXTINCD)
LDFLAGS = -O0 -g3 -Wall -L$(EXTLIBD)

all: $(EXEC)

$(EXEC): $(OBJS)
	[ ! -d "$(BIND)" ] && mkdir $(BIND); \
	$(CPP) $(LDFLAGS) -o $@ $^ $(LIBS)   

$(OBJD)/%.o: %.cpp $(INCS)
	[ ! -d "$(OBJD)" ] && mkdir $(OBJD); \
	$(CPP) $(CPPFLAGS) -c -o $@ $< 
	

clean:
	rm -r $(OBJS) $(EXEC)


