CFLAGS=-I../cwiid/libcwiid -Wall
LDFLAGS=-lGL -lGLU -lglut -lcwiid -L../cwiid/libcwiid

fluid_motion: fluid_motion.c solver.c
	gcc $(CFLAGS) fluid_motion.c solver.c $(LDFLAGS) -o fluid_motion

