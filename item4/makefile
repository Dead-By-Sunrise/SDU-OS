hdrs = ipc.h
opts = -g -c

a_src = consumerA.c ipc.c
a_obj = consumerA.o ipc.o

b_src = consumerB.c ipc.c
b_obj = consumerB.o ipc.o

c_src = consumerC.c ipc.c
c_obj = consumerC.o ipc.o

p_src = producer.c ipc.c
p_obj = producer.o ipc.o

all: consumerA consumerB consumerC producer

consumerA: $(a_obj)
	gcc $(a_obj) -o consumerA
consumerA.o: $(a_src) $(hdrs)
	gcc $(opts) $(a_src)

consumerB: $(b_obj)
	gcc $(b_obj) -o consumerB
consumerB.o: $(b_src) $(hdrs)
	gcc $(opts) $(b_src)

consumerC: $(c_obj)
	gcc $(c_obj) -o consumerC
consumerC.o: $(c_src) $(hdrs)
	gcc $(opts) $(c_src)

producer: $(p_obj)
	gcc $(p_obj) -o producer
producer.o: $(p_src) $(hdrs)
	gcc $(opts) $(p_src)

clean:
	rm consumerA consumerB consumerC producer *.o