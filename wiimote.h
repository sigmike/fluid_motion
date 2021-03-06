
#define WIIMOTE_ENABLED

#include <cwiid.h>
#include "quaternion.h"

extern double current_angle[3];
extern uint16_t motionplus_cal[3];
extern int reset_motionplus;
extern QUATERNION quat;

int wiimote_init();
int wiimote_close();

void motionplus_motion(double phi, double theta, double psi);
void button_event(cwiid_wiimote_t *wiimote, int buttons);

