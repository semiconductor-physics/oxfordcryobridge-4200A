/* USRLIB MODULE INFORMATION

	MODULE NAME: oxford_read_ips
	MODULE RETURN TYPE: int
	NUMBER OF PARMS: 28
	ARGUMENTS:
		cycle_number,	int *,	Output,	,	,	
		system_m,	int *,	Output,	,	,	
		system_n,	int *,	Output,	,	,	
		activity,	int *,	Output,	,	,	
		control_state,	int *,	Output,	,	,	
		switch_heater,	int *,	Output,	,	,	
		mode_m,	int *,	Output,	,	,	
		mode_n,	int *,	Output,	,	,	
		polarity_m,	int *,	Output,	,	,	
		polarity_n,	int *,	Output,	,	,	
		current_output_a,	double *,	Output,	,	,	
		negative_current_limit_a,	double *,	Output,	,	,	
		positive_current_limit_a,	double *,	Output,	,	,	
		current_setpoint_a,	double *,	Output,	,	,	
		current_sweep_rate_a_per_min,	double *,	Output,	,	,	
		magnet_current_a,	double *,	Output,	,	,	
		persistent_current_a,	double *,	Output,	,	,	
		trip_current_a,	double *,	Output,	,	,	
		supply_voltage_v,	double *,	Output,	,	,	
		voltage_limit_v,	double *,	Output,	,	,	
		field_output_t,	double *,	Output,	,	,	
		persistent_field_t,	double *,	Output,	,	,	
		trip_field_t,	double *,	Output,	,	,	
		field_setpoint_t,	double *,	Output,	,	,	
		field_sweep_rate_t_per_min,	double *,	Output,	,	,	
		switch_heater_current_ma,	double *,	Output,	,	,	
		lead_resistance_mohm,	double *,	Output,	,	,	
		magnet_inductance_h,	double *,	Output,	,	,	
	INCLUDES:
#include "oxford_kult.h"
	END USRLIB MODULE INFORMATION
*/
int oxford_read_ips(int *, int *, int *, int *, int *, int *, int *, int *, int *, int *, double *, double *, double *, double *, double *, double *, double *, double *, double *, double *, double *, double *, double *, double *, double *, double *, double *, double *);
