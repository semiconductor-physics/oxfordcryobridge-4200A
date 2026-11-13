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
/* USRLIB MODULE HELP DESCRIPTION

	END USRLIB MODULE HELP DESCRIPTION */
/* USRLIB MODULE PARAMETER LIST */
#include "oxford_kult.h"

/* USRLIB MODULE MAIN FUNCTION */
int oxford_read_all( int *cycle_number, double *helium_percent, double *nitrogen_percent, double *temperature_1_k, double *temperature_2_k, double *temperature_3_k, double *temperature_setpoint_k, double *temperature_error_k, double *gas_percent, double *heater_volts, double *lambda_pressure_mbar, double *lambda_temperature_k, double *lambda_valve_percent, double *current_output_a, double *current_setpoint_a, double *magnet_current_a, double *persistent_current_a, double *trip_current_a, double *field_output_t, double *field_setpoint_t, double *persistent_field_t, double *trip_field_t, double *supply_voltage_v, double *switch_heater_current_ma )
{
/* USRLIB MODULE CODE */
OxfordSession session;
OxfordSnapshot snapshot;
int status;

*cycle_number = GetKiteCycle();
oxford_session_init(&session);
status = oxford_open(&session);
if (status == OXFORD_OK)
    status = oxford_prepare_all(&session);
if (status == OXFORD_OK)
    status = oxford_read_snapshot(&session, &snapshot);
status = oxford_finish(&session, status);

if (status == OXFORD_OK)
{
    *helium_percent = snapshot.ilm.helium_percent;
    *nitrogen_percent = snapshot.ilm.nitrogen_percent;
    *temperature_1_k = snapshot.itc.temperature_1_k;
    *temperature_2_k = snapshot.itc.temperature_2_k;
    *temperature_3_k = snapshot.itc.temperature_3_k;
    *temperature_setpoint_k = snapshot.itc.setpoint_k;
    *temperature_error_k = snapshot.itc.error_k;
    *gas_percent = snapshot.itc.gas_percent;
    *heater_volts = snapshot.itc.heater_volts;
    *lambda_pressure_mbar = snapshot.lambda.pressure_mbar;
    *lambda_temperature_k = snapshot.lambda.temperature_k;
    *lambda_valve_percent = snapshot.lambda.valve_percent;
    *current_output_a = snapshot.ips.current_output_a;
    *current_setpoint_a = snapshot.ips.current_setpoint_a;
    *magnet_current_a = snapshot.ips.magnet_current_a;
    *persistent_current_a = snapshot.ips.persistent_current_a;
    *trip_current_a = snapshot.ips.trip_current_a;
    *field_output_t = snapshot.ips.field_output_t;
    *field_setpoint_t = snapshot.ips.field_setpoint_t;
    *persistent_field_t = snapshot.ips.persistent_field_t;
    *trip_field_t = snapshot.ips.trip_field_t;
    *supply_voltage_v = snapshot.ips.supply_voltage_v;
    *switch_heater_current_ma = snapshot.ips.switch_heater_current_ma;
}
return status;
/* USRLIB MODULE END  */
} 		/* End oxford_read_all.c */

