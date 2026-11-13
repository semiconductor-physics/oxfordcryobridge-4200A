/* USRLIB MODULE INFORMATION

	MODULE NAME: oxford_set_temperature
	MODULE RETURN TYPE: int
	NUMBER OF PARMS: 5
	ARGUMENTS:
		temperature_k,	D_ARRAY_T,	Input,	,	,	
		temperature_count,	int,	Input,	,	,	
		cycle_number,	int *,	Output,	,	,	
		selected_temperature_k,	double *,	Output,	,	,	
		final_temperature_1_k,	double *,	Output,	,	,	
	INCLUDES:
#include "oxford_kult.h"
	END USRLIB MODULE INFORMATION
*/
int oxford_set_temperature(double *, int, int *, double *, double *);
