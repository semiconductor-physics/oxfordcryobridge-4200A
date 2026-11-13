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
/* USRLIB MODULE HELP DESCRIPTION

	END USRLIB MODULE HELP DESCRIPTION */
/* USRLIB MODULE PARAMETER LIST */
#include "oxford_kult.h"

/* USRLIB MODULE MAIN FUNCTION */
int oxford_set_temperature( double *temperature_k, int temperature_count, int *cycle_number, double *selected_temperature_k, double *final_temperature_1_k )
{
/* USRLIB MODULE CODE */
OxfordSession session;
int index;
int status;

status = oxford_cycle_index(temperature_count, cycle_number, &index);
if (status != OXFORD_OK)
    return status;

*selected_temperature_k = temperature_k[index];
oxford_session_init(&session);
status = oxford_open(&session);
if (status == OXFORD_OK)
    status = oxford_prepare_all(&session);
if (status == OXFORD_OK)
    status = oxford_controller_set_temperature(&session,
                                               *selected_temperature_k,
                                               final_temperature_1_k);
return oxford_finish(&session, status);
/* USRLIB MODULE END  */
} 		/* End oxford_set_temperature.c */

