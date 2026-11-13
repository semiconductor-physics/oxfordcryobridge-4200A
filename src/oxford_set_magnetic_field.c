/* USRLIB MODULE INFORMATION

	MODULE NAME: oxford_set_magnetic_field
	MODULE RETURN TYPE: int 
	NUMBER OF PARMS: 5
	ARGUMENTS:
		field_t,	D_ARRAY_T,	Input,	,	,	
		field_count,	int,	Input,	,	,	
		cycle_number,	int *,	Output,	,	,	
		selected_field_t,	double *,	Output,	,	,	
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
int oxford_set_magnetic_field( double *field_t, int field_count, int *cycle_number, double *selected_field_t, double *final_source_field_t )
{
/* USRLIB MODULE CODE */

OxfordSession session;
int index;
int status;

status = oxford_cycle_index(field_count, cycle_number, &index);
if (status != OXFORD_OK)
    return status;

*selected_field_t = field_t[index];
oxford_session_init(&session);
status = oxford_open(&session);
if (status == OXFORD_OK)
    status = oxford_prepare_all(&session);
if (status == OXFORD_OK)
    status = oxford_controller_set_magnetic_field(&session,
                                                  *selected_field_t,
                                                  final_source_field_t);
return oxford_finish(&session, status);
/* USRLIB MODULE END  */
} 		/* End oxford_set_magnetic_field.c */

