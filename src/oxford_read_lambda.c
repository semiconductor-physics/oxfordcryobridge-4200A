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
/* USRLIB MODULE HELP DESCRIPTION

	END USRLIB MODULE HELP DESCRIPTION */
/* USRLIB MODULE PARAMETER LIST */
#include "oxford_kult.h"

/* USRLIB MODULE MAIN FUNCTION */
int oxford_read_lambda( int *cycle_number, int *system_state, int *activity_state, int *control_state, int *sweep_state, int *sensor_state, double *pressure_mbar, double *temperature_k, double *valve_percent )
{
/* USRLIB MODULE CODE */
OxfordSession session;
OxfordLambdaState state;
OxfordLambdaReadings readings;
int status;

*cycle_number = GetKiteCycle();
oxford_session_init(&session);
status = oxford_open(&session);
if (status == OXFORD_OK)
    status = oxford_prepare_device(&session, OXFORD_LAMBDA_ADDRESS);
if (status == OXFORD_OK)
    status = oxford_lambda_get_state(&session, &state);
if (status == OXFORD_OK)
    status = oxford_lambda_read(&session, &readings);
status = oxford_finish(&session, status);

if (status == OXFORD_OK)
{
    *system_state = state.system_state;
    *activity_state = state.activity_state;
    *control_state = state.control_state;
    *sweep_state = state.sweep_state;
    *sensor_state = state.sensor_state;
    *pressure_mbar = readings.pressure_mbar;
    *temperature_k = readings.temperature_k;
    *valve_percent = readings.valve_percent;
}
return status;
/* USRLIB MODULE END  */
} 		/* End oxford_read_lambda.c */

