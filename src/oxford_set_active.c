/* USRLIB MODULE INFORMATION

	MODULE NAME: oxford_set_active
	MODULE RETURN TYPE: int 
	NUMBER OF PARMS: 1
	ARGUMENTS:
		cycle_number,	int *,	Output,	,	,	
	INCLUDES:
#include "oxford_kult.h"
	END USRLIB MODULE INFORMATION
*/
/* USRLIB MODULE HELP DESCRIPTION

	END USRLIB MODULE HELP DESCRIPTION */
/* USRLIB MODULE PARAMETER LIST */
#include "oxford_kult.h"

/* USRLIB MODULE MAIN FUNCTION */
int oxford_set_active( int *cycle_number )
{
/* USRLIB MODULE CODE */
OxfordSession session;
int status;

*cycle_number = GetKiteCycle();
oxford_session_init(&session);
status = oxford_open(&session);
if (status == OXFORD_OK)
    status = oxford_prepare_device(&session, OXFORD_ILM_ADDRESS);
if (status == OXFORD_OK)
    status = oxford_controller_active(&session);
return oxford_finish(&session, status);
/* USRLIB MODULE END  */
} 		/* End oxford_set_active.c */

