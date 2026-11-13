#include <COM_usrlib.h>
__declspec(dllimport)
extern int HardwareOffline;
extern void outputf(char *fmt, ...);

int oxford_initialize(int * cycle_number);

int w_oxford_initialize(callinfo_t *callinfo)
{
	int*ret = (int*)malloc(sizeof(int));
	int *p0 = (int *)malloc(sizeof(int));
	if(!HardwareOffline)
		*ret = (int)oxford_initialize(p0);

	add_result("oxford_initialize", RETVAL_NAME, INT_P, ret, 1);
	add_result("oxford_initialize", S(0), INT_P, p0, 1);
	return(0);
}
