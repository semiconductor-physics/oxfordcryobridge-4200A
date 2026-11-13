#include <COM_usrlib.h>
__declspec(dllimport)
extern int HardwareOffline;
extern void outputf(char *fmt, ...);

int oxford_set_temperature(double *temperature_k, int temperature_count, int * cycle_number, double * selected_temperature_k, double * final_temperature_1_k);

int w_oxford_set_temperature(callinfo_t *callinfo)
{
	int*ret = (int*)malloc(sizeof(int));
	double *p0;
	int *p2 = (int *)malloc(sizeof(int));
	double *p3 = (double *)malloc(sizeof(double));
	double *p4 = (double *)malloc(sizeof(double));
	int status;
	extern void *GetReusableParm(char *search_name, int *status);

	p0 = (double *)GetReusableParm(S(0), &status);
	if((p0 == NULL) && (status < 0))
	{
		outputf("\"%s\" ", S(0));
		return(RP_ERROR);
	}

	if(!HardwareOffline)
		*ret = (int)oxford_set_temperature(p0, I(1), p2, p3, p4);

	add_result("oxford_set_temperature", RETVAL_NAME, INT_P, ret, 1);
	add_result("oxford_set_temperature", S(2), INT_P, p2, 1);
	add_result("oxford_set_temperature", S(3), DOUBLE_P, p3, 1);
	add_result("oxford_set_temperature", S(4), DOUBLE_P, p4, 1);
	return(0);
}
