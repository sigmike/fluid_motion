
#include "wiimote.h"
#include <math.h>
#include <stdlib.h>

#define toggle_bit(bf,b)	\
	(bf) = ((bf) & b)		\
	       ? ((bf) & ~(b))	\
	       : ((bf) | (b))


double current_angle[3] = {0, 0, 0};
uint16_t motionplus_cal[3] = {7904,7835,8009};
int reset_motionplus = 1;
cwiid_wiimote_t *wiimote;


cwiid_err_t err;
void err(cwiid_wiimote_t *wiimote, const char *s, va_list ap)
{
	if (wiimote) printf("%d:", cwiid_get_id(wiimote));
	else printf("-1:");
	vprintf(s, ap);
	printf("\n");
}

void motionplus_event(struct cwiid_motionplus_mesg mesg)
{
	int16_t angle_calibrated;
	double angle[3];
	int i;
	
	struct timeval tv;
	
	gettimeofday(&tv, NULL);
	//printf("%15d %15d\n", tv.tv_sec, tv.tv_usec);
	
	if (reset_motionplus)
	{
		for (i=0; i<3; i++)
		{
			motionplus_cal[i] = mesg.angle_rate[i];
			current_angle[i] = 0;
		}
		reset_motionplus = 0;
	}
	
	for (i=0; i<3; i++)
	{
		angle_calibrated = mesg.angle_rate[i] - motionplus_cal[i];
		//printf("calibrated angle %d : %5d, low_speed=%d\n", i, angle_calibrated, mesg.low_speed[i]);
		if (mesg.low_speed[i])
			angle[i] = (double)angle_calibrated / 20.0;
		else
			angle[i] = (double)angle_calibrated / 4.0;
	  if (fabs(angle[i]) > 1.0)
			current_angle[i] += angle[i];
	}
	motionplus_motion(angle[0], angle[1], angle[2]);
}

void cwiid_callback(cwiid_wiimote_t *wiimote, int mesg_count,
                    union cwiid_mesg mesg[], struct timespec *timestamp)
{
	int i, j;
	int valid_source;

	for (i=0; i < mesg_count; i++)
	{
		switch (mesg[i].type) {
		case CWIID_MESG_STATUS:
			printf("Status Report: battery=%d extension=",
			       mesg[i].status_mesg.battery);
			switch (mesg[i].status_mesg.ext_type) {
			case CWIID_EXT_NONE:
				printf("none");
				break;
			case CWIID_EXT_NUNCHUK:
				printf("Nunchuk");
				break;
			case CWIID_EXT_CLASSIC:
				printf("Classic Controller");
				break;
			case CWIID_EXT_BALANCE:
				printf("Balance Board");
				break;
			case CWIID_EXT_MOTIONPLUS:
				printf("MotionPlus");
				break;
			default:
				printf("Unknown Extension");
				break;
			}
			printf("\n");
			break;
		case CWIID_MESG_BTN:
			printf("Button Report: %.4X\n", mesg[i].btn_mesg.buttons);
			button_event(wiimote, mesg[i].btn_mesg.buttons);
			break;
		case CWIID_MESG_ACC:
			printf("Acc Report: x=%d, y=%d, z=%d\n",
                   mesg[i].acc_mesg.acc[CWIID_X],
			       mesg[i].acc_mesg.acc[CWIID_Y],
			       mesg[i].acc_mesg.acc[CWIID_Z]);
			break;
		case CWIID_MESG_IR:
			printf("IR Report: ");
			valid_source = 0;
			for (j = 0; j < CWIID_IR_SRC_COUNT; j++) {
				if (mesg[i].ir_mesg.src[j].valid) {
					valid_source = 1;
					printf("(%d,%d) ", mesg[i].ir_mesg.src[j].pos[CWIID_X],
					                   mesg[i].ir_mesg.src[j].pos[CWIID_Y]);
				}
			}
			if (!valid_source) {
				printf("no sources detected");
			}
			printf("\n");
			break;
		case CWIID_MESG_NUNCHUK:
			printf("Nunchuk Report: btns=%.2X stick=(%d,%d) acc.x=%d acc.y=%d "
			       "acc.z=%d\n", mesg[i].nunchuk_mesg.buttons,
			       mesg[i].nunchuk_mesg.stick[CWIID_X],
			       mesg[i].nunchuk_mesg.stick[CWIID_Y],
			       mesg[i].nunchuk_mesg.acc[CWIID_X],
			       mesg[i].nunchuk_mesg.acc[CWIID_Y],
			       mesg[i].nunchuk_mesg.acc[CWIID_Z]);
			break;
		case CWIID_MESG_CLASSIC:
			printf("Classic Report: btns=%.4X l_stick=(%d,%d) r_stick=(%d,%d) "
			       "l=%d r=%d\n", mesg[i].classic_mesg.buttons,
			       mesg[i].classic_mesg.l_stick[CWIID_X],
			       mesg[i].classic_mesg.l_stick[CWIID_Y],
			       mesg[i].classic_mesg.r_stick[CWIID_X],
			       mesg[i].classic_mesg.r_stick[CWIID_Y],
			       mesg[i].classic_mesg.l, mesg[i].classic_mesg.r);
			break;
		case CWIID_MESG_BALANCE:
			printf("Balance Report: right_top=%d right_bottom=%d "
			       "left_top=%d left_bottom=%d\n",
			       mesg[i].balance_mesg.right_top,
			       mesg[i].balance_mesg.right_bottom,
			       mesg[i].balance_mesg.left_top,
			       mesg[i].balance_mesg.left_bottom);
			break;
		case CWIID_MESG_MOTIONPLUS:
			motionplus_event(mesg[i].motionplus_mesg);
			/*
			printf("MotionPlus Report: angle_rate=(%d,%d,%d) low_speed=(%d,%d,%d)\n",
			       mesg[i].motionplus_mesg.angle_rate[0],
			       mesg[i].motionplus_mesg.angle_rate[1],
			       mesg[i].motionplus_mesg.angle_rate[2],
			       mesg[i].motionplus_mesg.low_speed[0],
			       mesg[i].motionplus_mesg.low_speed[1],
			       mesg[i].motionplus_mesg.low_speed[2]);
			*/
			break;
		case CWIID_MESG_ERROR:
			if (cwiid_close(wiimote)) {
				fprintf(stderr, "Error on wiimote disconnect\n");
				exit(-1);
			}
			exit(0);
			break;
		default:
			printf("Unknown Report");
			break;
		}
	}
}

int wiimote_init()
{
#ifdef WIIMOTE_ENABLED
	bdaddr_t bdaddr;	/* bluetooth device address */
	unsigned char rpt_mode = 0;
	
	cwiid_set_err(err);
	
	bdaddr = *BDADDR_ANY;
	
	printf("Put Wiimote in discoverable mode now (press 1+2)...\n");
	if (!(wiimote = cwiid_open(&bdaddr, 0))) {
		fprintf(stderr, "Unable to connect to wiimote\n");
		return -1;
	}
	if (cwiid_set_mesg_callback(wiimote, cwiid_callback)) {
		fprintf(stderr, "Unable to set message callback\n");
	}

	if (cwiid_enable(wiimote, CWIID_FLAG_MESG_IFC)) {
		fprintf(stderr, "Error enabling messages\n");
	}
	cwiid_enable(wiimote, CWIID_FLAG_MOTIONPLUS);
	toggle_bit(rpt_mode, CWIID_RPT_EXT);
	toggle_bit(rpt_mode, CWIID_RPT_BTN);
	if (cwiid_set_rpt_mode(wiimote, rpt_mode)) {
		fprintf(stderr, "Error setting report mode\n");
	}
#endif
	return 0;
}

int wiimote_close()
{
#ifdef WIIMOTE_ENABLED
	if (cwiid_close(wiimote)) {
		fprintf(stderr, "Error on wiimote disconnect\n");
		return -1;
	}
#endif
	return 0;
}