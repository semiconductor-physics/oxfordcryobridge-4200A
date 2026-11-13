/* USRLIB MODULE INFORMATION

	MODULE NAME: oxford_read_lambda
	MODULE RETURN TYPE: int
	NUMBER OF PARMS: 9
	ARGUMENTS:
		cycle_number,	int *,	Output,	,	,	
		system_state,	int *,	Output,	,	,	
		activity_state,	int *,	Output,	,	,	
		control_state,	int *,	Output,	,	,	
		sweep_state,	int *,	Output,	,	,	
		sensor_state,	int *,	Output,	,	,	
		pressure_mbar,	double *,	Output,	,	,	
		temperature_k,	double *,	Output,	,	,	
		valve_percent,	double *,	Output,	,	,	
	INCLUDES:
#include "oxford_kult.h"
	END USRLIB MODULE INFORMATION
*/
int oxford_read_lambda(int *, int *, int *, int *, int *, int *, double *, double *, double *);
