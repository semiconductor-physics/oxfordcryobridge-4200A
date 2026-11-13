/* USRLIB MODULE INFORMATION

	MODULE NAME: oxford_initialize
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
int oxford_initialize( int *cycle_number )
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
    status = oxford_controller_initialize(&session);
return oxford_finish(&session, status);
/* USRLIB MODULE END  */
} 		/* End oxford_initialize.c */

