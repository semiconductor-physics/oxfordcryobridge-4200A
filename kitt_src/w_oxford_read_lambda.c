#include <COM_usrlib.h>
__declspec(dllimport)
extern int HardwareOffline;
extern void outputf(char *fmt, ...);

int oxford_read_lambda(int * cycle_number, int * system_state, int * activity_state, int * control_state, int * sweep_state, int * sensor_state, double * pressure_mbar, double * temperature_k, double * valve_percent);

int w_oxford_read_lambda(callinfo_t *callinfo)
{
	int*ret = (int*)malloc(sizeof(int));
	int *p0 = (int *)malloc(sizeof(int));
	int *p1 = (int *)malloc(sizeof(int));
	int *p2 = (int *)malloc(sizeof(int));
	int *p3 = (int *)malloc(sizeof(int));
	int *p4 = (int *)malloc(sizeof(int));
	int *p5 = (int *)malloc(sizeof(int));
	double *p6 = (double *)malloc(sizeof(double));
	double *p7 = (double *)malloc(sizeof(double));
	double *p8 = (double *)malloc(sizeof(double));
	if(!HardwareOffline)
		*ret = (int)oxford_read_lambda(p0, p1, p2, p3, p4, p5, p6, p7, p8);

	add_result("oxford_read_lambda", RETVAL_NAME, INT_P, ret, 1);
	add_result("oxford_read_lambda", S(0), INT_P, p0, 1);
	add_result("oxford_read_lambda", S(1), INT_P, p1, 1);
	add_result("oxford_read_lambda", S(2), INT_P, p2, 1);
	add_result("oxford_read_lambda", S(3), INT_P, p3, 1);
	add_result("oxford_read_lambda", S(4), INT_P, p4, 1);
	add_result("oxford_read_lambda", S(5), INT_P, p5, 1);
	add_result("oxford_read_lambda", S(6), DOUBLE_P, p6, 1);
	add_result("oxford_read_lambda", S(7), DOUBLE_P, p7, 1);
	add_result("oxford_read_lambda", S(8), DOUBLE_P, p8, 1);
	return(0);
}
