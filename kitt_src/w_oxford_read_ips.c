#include <COM_usrlib.h>
__declspec(dllimport)
extern int HardwareOffline;
extern void outputf(char *fmt, ...);

int oxford_read_ips(int * cycle_number, int * system_m, int * system_n, int * activity, int * control_state, int * switch_heater, int * mode_m, int * mode_n, int * polarity_m, int * polarity_n, double * current_output_a, double * negative_current_limit_a, double * positive_current_limit_a, double * current_setpoint_a, double * current_sweep_rate_a_per_min, double * magnet_current_a, double * persistent_current_a, double * trip_current_a, double * supply_voltage_v, double * voltage_limit_v, double * field_output_t, double * persistent_field_t, double * trip_field_t, double * field_setpoint_t, double * field_sweep_rate_t_per_min, double * switch_heater_current_ma, double * lead_resistance_mohm, double * magnet_inductance_h);

int w_oxford_read_ips(callinfo_t *callinfo)
{
	int*ret = (int*)malloc(sizeof(int));
	int *p0 = (int *)malloc(sizeof(int));
	int *p1 = (int *)malloc(sizeof(int));
	int *p2 = (int *)malloc(sizeof(int));
	int *p3 = (int *)malloc(sizeof(int));
	int *p4 = (int *)malloc(sizeof(int));
	int *p5 = (int *)malloc(sizeof(int));
	int *p6 = (int *)malloc(sizeof(int));
	int *p7 = (int *)malloc(sizeof(int));
	int *p8 = (int *)malloc(sizeof(int));
	int *p9 = (int *)malloc(sizeof(int));
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
	double *p24 = (double *)malloc(sizeof(double));
	double *p25 = (double *)malloc(sizeof(double));
	double *p26 = (double *)malloc(sizeof(double));
	double *p27 = (double *)malloc(sizeof(double));
	if(!HardwareOffline)
		*ret = (int)oxford_read_ips(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27);

	add_result("oxford_read_ips", RETVAL_NAME, INT_P, ret, 1);
	add_result("oxford_read_ips", S(0), INT_P, p0, 1);
	add_result("oxford_read_ips", S(1), INT_P, p1, 1);
	add_result("oxford_read_ips", S(2), INT_P, p2, 1);
	add_result("oxford_read_ips", S(3), INT_P, p3, 1);
	add_result("oxford_read_ips", S(4), INT_P, p4, 1);
	add_result("oxford_read_ips", S(5), INT_P, p5, 1);
	add_result("oxford_read_ips", S(6), INT_P, p6, 1);
	add_result("oxford_read_ips", S(7), INT_P, p7, 1);
	add_result("oxford_read_ips", S(8), INT_P, p8, 1);
	add_result("oxford_read_ips", S(9), INT_P, p9, 1);
	add_result("oxford_read_ips", S(10), DOUBLE_P, p10, 1);
	add_result("oxford_read_ips", S(11), DOUBLE_P, p11, 1);
	add_result("oxford_read_ips", S(12), DOUBLE_P, p12, 1);
	add_result("oxford_read_ips", S(13), DOUBLE_P, p13, 1);
	add_result("oxford_read_ips", S(14), DOUBLE_P, p14, 1);
	add_result("oxford_read_ips", S(15), DOUBLE_P, p15, 1);
	add_result("oxford_read_ips", S(16), DOUBLE_P, p16, 1);
	add_result("oxford_read_ips", S(17), DOUBLE_P, p17, 1);
	add_result("oxford_read_ips", S(18), DOUBLE_P, p18, 1);
	add_result("oxford_read_ips", S(19), DOUBLE_P, p19, 1);
	add_result("oxford_read_ips", S(20), DOUBLE_P, p20, 1);
	add_result("oxford_read_ips", S(21), DOUBLE_P, p21, 1);
	add_result("oxford_read_ips", S(22), DOUBLE_P, p22, 1);
	add_result("oxford_read_ips", S(23), DOUBLE_P, p23, 1);
	add_result("oxford_read_ips", S(24), DOUBLE_P, p24, 1);
	add_result("oxford_read_ips", S(25), DOUBLE_P, p25, 1);
	add_result("oxford_read_ips", S(26), DOUBLE_P, p26, 1);
	add_result("oxford_read_ips", S(27), DOUBLE_P, p27, 1);
	return(0);
}
