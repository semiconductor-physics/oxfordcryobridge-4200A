/* USRLIB MODULE INFORMATION

	MODULE NAME: oxford_read_ilm
	MODULE RETURN TYPE: int
	NUMBER OF PARMS: 13
	ARGUMENTS:
		cycle_number,	int *,	Output,	,	,	
		channel_1_usage,	int *,	Output,	,	,	
		channel_2_usage,	int *,	Output,	,	,	
		channel_3_usage,	int *,	Output,	,	,	
		channel_1_status,	int *,	Output,	,	,	
		channel_2_status,	int *,	Output,	,	,	
		channel_3_status,	int *,	Output,	,	,	
		relay_status,	int *,	Output,	,	,	
		helium_percent,	double *,	Output,	,	,	
		nitrogen_percent,	double *,	Output,	,	,	
		channel_1_current,	int *,	Output,	,	,	
		channel_2_current,	int *,	Output,	,	,	
		valve_position,	int *,	Output,	,	,	
	INCLUDES:
#include "oxford_kult.h"
	END USRLIB MODULE INFORMATION
*/
int oxford_read_ilm(int *, int *, int *, int *, int *, int *, int *, int *, double *, double *, int *, int *, int *);
