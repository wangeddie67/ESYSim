CC = gcc

CFLAGS = -O0 -g  -w -Wall -Wno-unused 


.SUFFIXES: .o .c



SIM_SRCS =  packet_retry_algr.c configuration.c flit.c input_buffer.c main.c mess_queue.c optical_statistics.c packet.c sim_conflict.c sim_main.c sim_mess.c sim_receive.c sim_routing.c

SIM_OBJS = $(SIM_SRCS:.c=.o)

TARGET = optical

$(TARGET):$(SIM_OBJS) 

.c.$(OEXT):
	$(CC) $(CFLAGS) -c $<  

clean:
	rm -rf *.o  

#####################################################
packet_retry_algr.o: packet_retry_algr.c point-point.h
configuration.o: configuration.c point-point.h ../smt.h ../host.h
flit.o: flit.c point-point.h
input_buffer.o:  input_buffer.c point-point.h
main.o: main.c point-point.h
mess_queue.o: mess_queue.c point-point.h
optical_statistics.o: optical_statistics.c point-point.h ../smt.h ../host.h
packet.o: packet.c point-point.h
sim_conflict.o: sim_conflict.c point-point.h 
sim_main.o: sim_main.c point-point.h
sim_mess.o: sim_mess.c point-point.h 
sim_receive.o: sim_receive.c point-point.h 
sim_routing.o: sim_routing.c point-point.h


