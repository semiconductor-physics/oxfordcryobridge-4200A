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
/* USRLIB MODULE HELP DESCRIPTION

	END USRLIB MODULE HELP DESCRIPTION */
/* USRLIB MODULE PARAMETER LIST */
#include "oxford_kult.h"

/* USRLIB MODULE MAIN FUNCTION */
int oxford_read_ilm( int *cycle_number, int *channel_1_usage, int *channel_2_usage, int *channel_3_usage, int *channel_1_status, int *channel_2_status, int *channel_3_status, int *relay_status, double *helium_percent, double *nitrogen_percent, int *channel_1_current, int *channel_2_current, int *valve_position )
{
/* USRLIB MODULE CODE */
OxfordSession session;
OxfordIlmState state;
OxfordIlmReadings readings;
int status;

*cycle_number = GetKiteCycle();
oxford_session_init(&session);
status = oxford_open(&session);
if (status == OXFORD_OK)
    status = oxford_prepare_device(&session, OXFORD_ILM_ADDRESS);
if (status == OXFORD_OK)
    status = oxford_ilm_get_state(&session, &state);
if (status == OXFORD_OK)
    status = oxford_ilm_read(&session, &readings);
status = oxford_finish(&session, status);

if (status == OXFORD_OK)
{
    *channel_1_usage = state.channel_1_usage;
    *channel_2_usage = state.channel_2_usage;
    *channel_3_usage = state.channel_3_usage;
    *channel_1_status = state.channel_1_status;
    *channel_2_status = state.channel_2_status;
    *channel_3_status = state.channel_3_status;
    *relay_status = state.relay_status;
    *helium_percent = readings.helium_percent;
    *nitrogen_percent = readings.nitrogen_percent;
    *channel_1_current = readings.channel_1_current;
    *channel_2_current = readings.channel_2_current;
    *valve_position = readings.valve_position;
}
return status;
/* USRLIB MODULE END  */
} 		/* End oxford_read_ilm.c */

