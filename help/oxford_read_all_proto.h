/* USRLIB MODULE INFORMATION

	MODULE NAME: oxford_read_all
	MODULE RETURN TYPE: int
	NUMBER OF PARMS: 24
	ARGUMENTS:
		cycle_number,	int *,	Output,	,	,	
		helium_percent,	double *,	Output,	,	,	
		nitrogen_percent,	double *,	Output,	,	,	
		temperature_1_k,	double *,	Output,	,	,	
		temperature_2_k,	double *,	Output,	,	,	
		temperature_3_k,	double *,	Output,	,	,	
		temperature_setpoint_k,	double *,	Output,	,	,	
		temperature_error_k,	double *,	Output,	,	,	
		gas_percent,	double *,	Output,	,	,	
		heater_volts,	double *,	Output,	,	,	
		lambda_pressure_mbar,	double *,	Output,	,	,	
		lambda_temperature_k,	double *,	Output,	,	,	
		lambda_valve_percent,	double *,	Output,	,	,	
		current_output_a,	double *,	Output,	,	,	
		current_setpoint_a,	double *,	Output,	,	,	
		magnet_current_a,	double *,	Output,	,	,	
		persistent_current_a,	double *,	Output,	,	,	
		trip_current_a,	double *,	Output,	,	,	
		field_output_t,	double *,	Output,	,	,	
		field_setpoint_t,	double *,	Output,	,	,	
		persistent_field_t,	double *,	Output,	,	,	
		trip_field_t,	double *,	Output,	,	,	
		supply_voltage_v,	double *,	Output,	,	,	
		switch_heater_current_ma,	double *,	Output,	,	,	
	INCLUDES:
#include "oxford_kult.h"
	END USRLIB MODULE INFORMATION
*/
int oxford_read_all(int *, double *, double *, double *, double *, double *, double *, double *, double *, double *, double *, double *, double *, double *, double *, double *, double *, double *, double *, double *, double *, double *, double *, double *);
