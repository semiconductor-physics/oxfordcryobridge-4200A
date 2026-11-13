/* USRLIB MODULE INFORMATION

	MODULE NAME: oxford_set_standby
	MODULE RETURN TYPE: int 
	NUMBER OF PARMS: 2
	ARGUMENTS:
		cycle_number,	int *,	Output,	,	,	
		final_source_field_t,	double *,	Output,	,	,	
	INCLUDES:
#include "oxford_kult.h"
	END USRLIB MODULE INFORMATION
*/
/* USRLIB MODULE HELP DESCRIPTION

	END USRLIB MODULE HELP DESCRIPTION */
/* USRLIB MODULE PARAMETER LIST */
#include "oxford_kult.h"

/* USRLIB MODULE MAIN FUNCTION */
int oxford_set_standby( int *cycle_number, double *final_source_field_t )
{
/* USRLIB MODULE CODE */
OxfordSession session;
int status;

*cycle_number = GetKiteCycle();
oxford_session_init(&session);
status = oxford_open(&session);
if (status == OXFORD_OK)
    status = oxford_prepare_all(&session);
if (status == OXFORD_OK)
    status = oxford_controller_standby(&session, final_source_field_t);
return oxford_finish(&session, status);
/* USRLIB MODULE END  */
} 		/* End oxford_set_standby.c */

