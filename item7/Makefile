head = Myvmrp.h
srcs = Myvmrp.cc
objs = Myvmrp.o
opts = -w -g -c

all: Myvmrp

Myvmrp: $(objs)
	g++ $(objs) -o Myvmrp

Myvmrp.o: $(srcs) $(head)
	g++ $(opts) $(srcs)

clean:
	rm Myvmrp *.o
