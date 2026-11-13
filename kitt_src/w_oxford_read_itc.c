#include <COM_usrlib.h>
__declspec(dllimport)
extern int HardwareOffline;
extern void outputf(char *fmt, ...);

int oxford_read_itc(int * cycle_number, int * system_state, int * auto_manual_state, int * control_state, int * sweep_state, int * heater_sensor, int * auto_pid_state, double * setpoint_k, double * temperature_1_k, double * temperature_2_k, double * temperature_3_k, double * error_k, double * heater_percent, double * heater_volts, double * gas_percent, double * proportional_band_k, double * integral_minutes, double * derivative_minutes);

int w_oxford_read_itc(callinfo_t *callinfo)
{
	int*ret = (int*)malloc(sizeof(int));
	int *p0 = (int *)malloc(sizeof(int));
	int *p1 = (int *)malloc(sizeof(int));
	int *p2 = (int *)malloc(sizeof(int));
	int *p3 = (int *)malloc(sizeof(int));
	int *p4 = (int *)malloc(sizeof(int));
	int *p5 = (int *)malloc(sizeof(int));
	int *p6 = (int *)malloc(sizeof(int));
	double *p7 = (double *)malloc(sizeof(double));
	double *p8 = (double *)malloc(sizeof(double));
	double *p9 = (double *)malloc(sizeof(double));
	double *p10 = (double *)malloc(sizeof(double));
	double *p11 = (double *)malloc(sizeof(double));
	double *p12 = (double *)malloc(sizeof(double));
	double *p13 = (double *)malloc(sizeof(double));
	double *p14 = (double *)malloc(sizeof(double));
	double *p15 = (double *)malloc(sizeof(double));
	double *p16 = (double *)malloc(sizeof(double));
	double *p17 = (double *)malloc(sizeof(double));
	if(!HardwareOffline)
		*ret = (int)oxford_read_itc(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17);

	add_result("oxford_read_itc", RETVAL_NAME, INT_P, ret, 1);
	add_result("oxford_read_itc", S(0), INT_P, p0, 1);
	add_result("oxford_read_itc", S(1), INT_P, p1, 1);
	add_result("oxford_read_itc", S(2), INT_P, p2, 1);
	add_result("oxford_read_itc", S(3), INT_P, p3, 1);
	add_result("oxford_read_itc", S(4), INT_P, p4, 1);
	add_result("oxford_read_itc", S(5), INT_P, p5, 1);
	add_result("oxford_read_itc", S(6), INT_P, p6, 1);
	add_result("oxford_read_itc", S(7), DOUBLE_P, p7, 1);
	add_result("oxford_read_itc", S(8), DOUBLE_P, p8, 1);
	add_result("oxford_read_itc", S(9), DOUBLE_P, p9, 1);
	add_result("oxford_read_itc", S(10), DOUBLE_P, p10, 1);
	add_result("oxford_read_itc", S(11), DOUBLE_P, p11, 1);
	add_result("oxford_read_itc", S(12), DOUBLE_P, p12, 1);
	add_result("oxford_read_itc", S(13), DOUBLE_P, p13, 1);
	add_result("oxford_read_itc", S(14), DOUBLE_P, p14, 1);
	add_result("oxford_read_itc", S(15), DOUBLE_P, p15, 1);
	add_result("oxford_read_itc", S(16), DOUBLE_P, p16, 1);
	add_result("oxford_read_itc", S(17), DOUBLE_P, p17, 1);
	return(0);
}
