/* USRLIB MODULE INFORMATION

	MODULE NAME: oxford_read_itc
	MODULE RETURN TYPE: int
	NUMBER OF PARMS: 18
	ARGUMENTS:
		cycle_number,	int *,	Output,	,	,	
		system_state,	int *,	Output,	,	,	
		auto_manual_state,	int *,	Output,	,	,	
		control_state,	int *,	Output,	,	,	
		sweep_state,	int *,	Output,	,	,	
		heater_sensor,	int *,	Output,	,	,	
		auto_pid_state,	int *,	Output,	,	,	
		setpoint_k,	double *,	Output,	,	,	
		temperature_1_k,	double *,	Output,	,	,	
		temperature_2_k,	double *,	Output,	,	,	
		temperature_3_k,	double *,	Output,	,	,	
		error_k,	double *,	Output,	,	,	
		heater_percent,	double *,	Output,	,	,	
		heater_volts,	double *,	Output,	,	,	
		gas_percent,	double *,	Output,	,	,	
		proportional_band_k,	double *,	Output,	,	,	
		integral_minutes,	double *,	Output,	,	,	
		derivative_minutes,	double *,	Output,	,	,	
	INCLUDES:
#include "oxford_kult.h"
	END USRLIB MODULE INFORMATION
*/
int oxford_read_itc(int *, int *, int *, int *, int *, int *, int *, double *, double *, double *, double *, double *, double *, double *, double *, double *, double *, double *);
