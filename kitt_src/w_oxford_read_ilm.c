#include <COM_usrlib.h>
__declspec(dllimport)
extern int HardwareOffline;
extern void outputf(char *fmt, ...);

int oxford_read_ilm(int * cycle_number, int * channel_1_usage, int * channel_2_usage, int * channel_3_usage, int * channel_1_status, int * channel_2_status, int * channel_3_status, int * relay_status, double * helium_percent, double * nitrogen_percent, int * channel_1_current, int * channel_2_current, int * valve_position);

int w_oxford_read_ilm(callinfo_t *callinfo)
{
	int*ret = (int*)malloc(sizeof(int));
	int *p0 = (int *)malloc(sizeof(int));
	int *p1 = (int *)malloc(sizeof(int));
	int *p2 = (int *)malloc(sizeof(int));
	int *p3 = (int *)malloc(sizeof(int));
	int *p4 = (int *)malloc(sizeof(int));
	int *p5 = (int *)malloc(sizeof(int));
	int *p6 = (int *)malloc(sizeof(int));
	int *p7 = (int *)malloc(sizeof(int));
	double *p8 = (double *)malloc(sizeof(double));
	double *p9 = (double *)malloc(sizeof(double));
	int *p10 = (int *)malloc(sizeof(int));
	int *p11 = (int *)malloc(sizeof(int));
	int *p12 = (int *)malloc(sizeof(int));
	if(!HardwareOffline)
		*ret = (int)oxford_read_ilm(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);

	add_result("oxford_read_ilm", RETVAL_NAME, INT_P, ret, 1);
	add_result("oxford_read_ilm", S(0), INT_P, p0, 1);
	add_result("oxford_read_ilm", S(1), INT_P, p1, 1);
	add_result("oxford_read_ilm", S(2), INT_P, p2, 1);
	add_result("oxford_read_ilm", S(3), INT_P, p3, 1);
	add_result("oxford_read_ilm", S(4), INT_P, p4, 1);
	add_result("oxford_read_ilm", S(5), INT_P, p5, 1);
	add_result("oxford_read_ilm", S(6), INT_P, p6, 1);
	add_result("oxford_read_ilm", S(7), INT_P, p7, 1);
	add_result("oxford_read_ilm", S(8), DOUBLE_P, p8, 1);
	add_result("oxford_read_ilm", S(9), DOUBLE_P, p9, 1);
	add_result("oxford_read_ilm", S(10), INT_P, p10, 1);
	add_result("oxford_read_ilm", S(11), INT_P, p11, 1);
	add_result("oxford_read_ilm", S(12), INT_P, p12, 1);
	return(0);
}
