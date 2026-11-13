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
/* USRLIB MODULE HELP DESCRIPTION

	END USRLIB MODULE HELP DESCRIPTION */
/* USRLIB MODULE PARAMETER LIST */
#include "oxford_kult.h"

/* USRLIB MODULE MAIN FUNCTION */
int oxford_read_ips( int *cycle_number, int *system_m, int *system_n, int *activity, int *control_state, int *switch_heater, int *mode_m, int *mode_n, int *polarity_m, int *polarity_n, double *current_output_a, double *negative_current_limit_a, double *positive_current_limit_a, double *current_setpoint_a, double *current_sweep_rate_a_per_min, double *magnet_current_a, double *persistent_current_a, double *trip_current_a, double *supply_voltage_v, double *voltage_limit_v, double *field_output_t, double *persistent_field_t, double *trip_field_t, double *field_setpoint_t, double *field_sweep_rate_t_per_min, double *switch_heater_current_ma, double *lead_resistance_mohm, double *magnet_inductance_h )
{
/* USRLIB MODULE CODE */
OxfordSession session;
OxfordIpsState state;
OxfordIpsReadings readings;
int status;

*cycle_number = GetKiteCycle();
oxford_session_init(&session);
status = oxford_open(&session);
if (status == OXFORD_OK)
    status = oxford_prepare_device(&session, OXFORD_IPS_ADDRESS);
if (status == OXFORD_OK)
    status = oxford_ips_get_state(&session, &state);
if (status == OXFORD_OK)
    status = oxford_ips_read(&session, &readings);
status = oxford_finish(&session, status);

if (status == OXFORD_OK)
{
    *system_m = state.system_m;
    *system_n = state.system_n;
    *activity = state.activity;
    *control_state = state.control_state;
    *switch_heater = state.switch_heater;
    *mode_m = state.mode_m;
    *mode_n = state.mode_n;
    *polarity_m = state.polarity_m;
    *polarity_n = state.polarity_n;
    *current_output_a = readings.current_output_a;
    *negative_current_limit_a = readings.negative_current_limit_a;
    *positive_current_limit_a = readings.positive_current_limit_a;
    *current_setpoint_a = readings.current_setpoint_a;
    *current_sweep_rate_a_per_min = readings.current_sweep_rate_a_per_min;
    *magnet_current_a = readings.magnet_current_a;
    *persistent_current_a = readings.persistent_current_a;
    *trip_current_a = readings.trip_current_a;
    *supply_voltage_v = readings.supply_voltage_v;
    *voltage_limit_v = readings.voltage_limit_v;
    *field_output_t = readings.field_output_t;
    *persistent_field_t = readings.persistent_field_t;
    *trip_field_t = readings.trip_field_t;
    *field_setpoint_t = readings.field_setpoint_t;
    *field_sweep_rate_t_per_min = readings.field_sweep_rate_t_per_min;
    *switch_heater_current_ma = readings.switch_heater_current_ma;
    *lead_resistance_mohm = readings.lead_resistance_mohm;
    *magnet_inductance_h = readings.magnet_inductance_h;
}
return status;
/* USRLIB MODULE END  */
} 		/* End oxford_read_ips.c */

