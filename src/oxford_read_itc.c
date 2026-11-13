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
/* USRLIB MODULE HELP DESCRIPTION

	END USRLIB MODULE HELP DESCRIPTION */
/* USRLIB MODULE PARAMETER LIST */
#include "oxford_kult.h"

/* USRLIB MODULE MAIN FUNCTION */
int oxford_read_itc( int *cycle_number, int *system_state, int *auto_manual_state, int *control_state, int *sweep_state, int *heater_sensor, int *auto_pid_state, double *setpoint_k, double *temperature_1_k, double *temperature_2_k, double *temperature_3_k, double *error_k, double *heater_percent, double *heater_volts, double *gas_percent, double *proportional_band_k, double *integral_minutes, double *derivative_minutes )
{
/* USRLIB MODULE CODE */
OxfordSession session;
OxfordItcState state;
OxfordItcReadings readings;
int status;

*cycle_number = GetKiteCycle();
oxford_session_init(&session);
status = oxford_open(&session);
if (status == OXFORD_OK)
    status = oxford_prepare_device(&session, OXFORD_ITC_ADDRESS);
if (status == OXFORD_OK)
    status = oxford_itc_get_state(&session, &state);
if (status == OXFORD_OK)
    status = oxford_itc_read(&session, &readings);
status = oxford_finish(&session, status);

if (status == OXFORD_OK)
{
    *system_state = state.system_state;
    *auto_manual_state = state.auto_manual_state;
    *control_state = state.control_state;
    *sweep_state = state.sweep_state;
    *heater_sensor = state.heater_sensor;
    *auto_pid_state = state.auto_pid_state;
    *setpoint_k = readings.setpoint_k;
    *temperature_1_k = readings.temperature_1_k;
    *temperature_2_k = readings.temperature_2_k;
    *temperature_3_k = readings.temperature_3_k;
    *error_k = readings.error_k;
    *heater_percent = readings.heater_percent;
    *heater_volts = readings.heater_volts;
    *gas_percent = readings.gas_percent;
    *proportional_band_k = readings.proportional_band_k;
    *integral_minutes = readings.integral_minutes;
    *derivative_minutes = readings.derivative_minutes;
}
return status;
/* USRLIB MODULE END  */
} 		/* End oxford_read_itc.c */

