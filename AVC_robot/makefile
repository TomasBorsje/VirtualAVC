DIR = C:\\Users\Me\Source\Repos\TomasBorsje\VirtualAVC\SFML-2.5.1
CFLAGS = -I ${DIR}\include 
LFLAGS = -L ${DIR}\lib 
LIBS = -lsfml-window  -lsfml-graphics -lsfml-system -lsfml-network 
robot: robot.o 
	g++ $(LFLAGS) -o robot robot.o ${LIBS} 
robot.o: robot.cpp 
	g++  -c $(CFLAGS) robot.cpp 


