#Makefile para la compilacion de los prog sensor y deposito
#Limpia tambien los semaforos

todo: control deposito sensor

control: control.o colamsg.o
		g++ control.o colamsg.o -o control -lrt -lpthread

deposito: deposito.o memocomp.o semaforo.o
	g++ deposito.o memocomp.o semaforo.o -o deposito -lrt -lpthread

sensor: sensor.o memocomp.o semaforo.o colamsg.o
	g++ sensor.o memocomp.o semaforo.o colamsg.o -o sensor -lrt -lpthread

control.o: control.cpp colamsg.hpp
		g++ -c control.cpp

deposito.o: deposito.cpp memocomp.hpp semaforo.hpp
	g++ -c deposito.cpp

sensor.o: sensor.cpp memocomp.hpp semaforo.hpp colamsg.hpp
	g++ -c sensor.cpp

memocomp.o: memocomp.cpp memocomp.hpp
	g++ -c memocomp.cpp

semaforo.o: semaforo.cpp semaforo.hpp
	g++ -c semaforo.cpp

clean:
	rm /dev/shm/sem.1000*
	rm /dev/shm/1000*
