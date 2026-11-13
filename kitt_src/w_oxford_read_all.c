#include <COM_usrlib.h>
__declspec(dllimport)
extern int HardwareOffline;
extern void outputf(char *fmt, ...);

int oxford_read_all(int * cycle_number, double * helium_percent, double * nitrogen_percent, double * temperature_1_k, double * temperature_2_k, double * temperature_3_k, double * temperature_setpoint_k, double * temperature_error_k, double * gas_percent, double * heater_volts, double * lambda_pressure_mbar, double * lambda_temperature_k, double * lambda_valve_percent, double * current_output_a, double * current_setpoint_a, double * magnet_current_a, double * persistent_current_a, double * trip_current_a, double * field_output_t, double * field_setpoint_t, double * persistent_field_t, double * trip_field_t, double * supply_voltage_v, double * switch_heater_current_ma);

int w_oxford_read_all(callinfo_t *callinfo)
{
	int*ret = (int*)malloc(sizeof(int));
	int *p0 = (int *)malloc(sizeof(int));
	double *p1 = (double *)malloc(sizeof(double));
	double *p2 = (double *)malloc(sizeof(double));
	double *p3 = (double *)malloc(sizeof(double));
	double *p4 = (double *)malloc(sizeof(double));
	double *p5 = (double *)malloc(sizeof(double));
	double *p6 = (double *)malloc(sizeof(double));
	double *p7 = (double *)malloc(sizeof(double));
	double *p8 = (double *)malloc(sizeof(double));
	double *p9 = (double *)malloc(sizeof(double));
	double *p10 = (double *)malloc(sizeof(double));
	double *p11 = (double *)malloc(sizeof(double));
	double *p12 = (double *)malloc(sizeof(double));
	double *p13 = (double *)malloc(sizeof(double));
	double *p14 = (double *)malloc(sizeof(double));
	double *p15 = (double *)malloc(sizeof(double));
	double *p16 = (double *)malloc(sizeof(double));
	double *p17 = (double *)malloc(sizeof(double));
	double *p18 = (double *)malloc(sizeof(double));
	double *p19 = (double *)malloc(sizeof(double));
	double *p20 = (double *)malloc(sizeof(double));
	double *p21 = (double *)malloc(sizeof(double));
	double *p22 = (double *)malloc(sizeof(double));
	double *p23 = (double *)malloc(sizeof(double));
	if(!HardwareOffline)
		*ret = (int)oxford_read_all(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23);

	add_result("oxford_read_all", RETVAL_NAME, INT_P, ret, 1);
	add_result("oxford_read_all", S(0), INT_P, p0, 1);
	add_result("oxford_read_all", S(1), DOUBLE_P, p1, 1);
	add_result("oxford_read_all", S(2), DOUBLE_P, p2, 1);
	add_result("oxford_read_all", S(3), DOUBLE_P, p3, 1);
	add_result("oxford_read_all", S(4), DOUBLE_P, p4, 1);
	add_result("oxford_read_all", S(5), DOUBLE_P, p5, 1);
	add_result("oxford_read_all", S(6), DOUBLE_P, p6, 1);
	add_result("oxford_read_all", S(7), DOUBLE_P, p7, 1);
	add_result("oxford_read_all", S(8), DOUBLE_P, p8, 1);
	add_result("oxford_read_all", S(9), DOUBLE_P, p9, 1);
	add_result("oxford_read_all", S(10), DOUBLE_P, p10, 1);
	add_result("oxford_read_all", S(11), DOUBLE_P, p11, 1);
	add_result("oxford_read_all", S(12), DOUBLE_P, p12, 1);
	add_result("oxford_read_all", S(13), DOUBLE_P, p13, 1);
	add_result("oxford_read_all", S(14), DOUBLE_P, p14, 1);
	add_result("oxford_read_all", S(15), DOUBLE_P, p15, 1);
	add_result("oxford_read_all", S(16), DOUBLE_P, p16, 1);
	add_result("oxford_read_all", S(17), DOUBLE_P, p17, 1);
	add_result("oxford_read_all", S(18), DOUBLE_P, p18, 1);
	add_result("oxford_read_all", S(19), DOUBLE_P, p19, 1);
	add_result("oxford_read_all", S(20), DOUBLE_P, p20, 1);
	add_result("oxford_read_all", S(21), DOUBLE_P, p21, 1);
	add_result("oxford_read_all", S(22), DOUBLE_P, p22, 1);
	add_result("oxford_read_all", S(23), DOUBLE_P, p23, 1);
	return(0);
}
