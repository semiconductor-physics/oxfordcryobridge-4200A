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
int oxford_set_magnetic_field(double *, int, int *, double *, double *);
