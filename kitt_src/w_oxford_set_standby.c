#include <COM_usrlib.h>
__declspec(dllimport)
extern int HardwareOffline;
extern void outputf(char *fmt, ...);

int oxford_set_standby(int * cycle_number, double * final_source_field_t);

int w_oxford_set_standby(callinfo_t *callinfo)
{
	int*ret = (int*)malloc(sizeof(int));
	int *p0 = (int *)malloc(sizeof(int));
	double *p1 = (double *)malloc(sizeof(double));
	if(!HardwareOffline)
		*ret = (int)oxford_set_standby(p0, p1);

	add_result("oxford_set_standby", RETVAL_NAME, INT_P, ret, 1);
	add_result("oxford_set_standby", S(0), INT_P, p0, 1);
	add_result("oxford_set_standby", S(1), DOUBLE_P, p1, 1);
	return(0);
}
