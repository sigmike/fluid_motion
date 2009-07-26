CFLAGS=-I../cwiid/libcwiid -Wall
LDFLAGS=-lGL -lGLU -lglut -lcwiid -L../cwiid/libcwiid

fluid_motion: fluid_motion.c solver.c quaternion.c wiimote.c
	gcc $(CFLAGS) fluid_motion.c solver.c quaternion.c wiimote.c $(LDFLAGS) -o fluid_motion

lesson5: lesson5.c wiimote.c
	gcc $(CFLAGS) lesson5.c wiimote.c $(LDFLAGS) -o lesson5
