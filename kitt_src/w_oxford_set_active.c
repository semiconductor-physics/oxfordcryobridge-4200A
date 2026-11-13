#include <COM_usrlib.h>
__declspec(dllimport)
extern int HardwareOffline;
extern void outputf(char *fmt, ...);

int oxford_set_active(int * cycle_number);

int w_oxford_set_active(callinfo_t *callinfo)
{
	int*ret = (int*)malloc(sizeof(int));
	int *p0 = (int *)malloc(sizeof(int));
	if(!HardwareOffline)
		*ret = (int)oxford_set_active(p0);

	add_result("oxford_set_active", RETVAL_NAME, INT_P, ret, 1);
	add_result("oxford_set_active", S(0), INT_P, p0, 1);
	return(0);
}
