#ifndef OXFORD_KULT_H
#define OXFORD_KULT_H

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define OXFORD_OK                     0
#define OXFORD_ERR_ARGUMENT      (-21001)
#define OXFORD_ERR_OPEN          (-21002)
#define OXFORD_ERR_CONFIG        (-21003)
#define OXFORD_ERR_TIMEOUT       (-21004)
#define OXFORD_ERR_PURGE         (-21005)
#define OXFORD_ERR_WRITE         (-21006)
#define OXFORD_ERR_READ          (-21007)
#define OXFORD_ERR_RESPONSE      (-21008)
#define OXFORD_ERR_DEVICE        (-21009)
#define OXFORD_ERR_CLOSE         (-21010)
#define OXFORD_ERR_RANGE         (-21011)
#define OXFORD_ERR_CYCLE         (-21012)

#define OXFORD_COM_PORT              "\\\\.\\COM1"
#define OXFORD_MAX_FRAME             128
#define OXFORD_READ_TIMEOUT_MS       5000UL
#define OXFORD_READ_SLICE_MS         100UL
#define OXFORD_WRITE_TIMEOUT_MS      2000UL
#define OXFORD_INTERCOMMAND_MS       10UL
#define OXFORD_ADAPTER_SETTLE_MS     1000UL
#define OXFORD_Q2_SETTLE_MS          50UL

#define OXFORD_ITC_ADDRESS           1
#define OXFORD_IPS_ADDRESS           2
#define OXFORD_LAMBDA_ADDRESS        3
#define OXFORD_ILM_ADDRESS           5

typedef struct OxfordSessionTag
{
    HANDLE port;
    int open;
} OxfordSession;

typedef struct OxfordIlmReadingsTag
{
    double helium_percent;
    double nitrogen_percent;
    int channel_1_current;
    int channel_2_current;
    int valve_position;
} OxfordIlmReadings;

typedef struct OxfordIlmStateTag
{
    int channel_1_usage;
    int channel_2_usage;
    int channel_3_usage;
    int channel_1_status;
    int channel_2_status;
    int channel_3_status;
    int relay_status;
} OxfordIlmState;

typedef struct OxfordItcReadingsTag
{
    double setpoint_k;
    double temperature_1_k;
    double temperature_2_k;
    double temperature_3_k;
    double error_k;
    double heater_percent;
    double heater_volts;
    double gas_percent;
    double proportional_band_k;
    double integral_minutes;
    double derivative_minutes;
} OxfordItcReadings;

typedef struct OxfordItcStateTag
{
    int system_state;
    int auto_manual_state;
    int control_state;
    int sweep_state;
    int heater_sensor;
    int auto_pid_state;
} OxfordItcState;

typedef struct OxfordLambdaReadingsTag
{
    double pressure_mbar;
    double temperature_k;
    double valve_percent;
} OxfordLambdaReadings;

typedef struct OxfordLambdaStateTag
{
    int system_state;
    int activity_state;
    int control_state;
    int sweep_state;
    int sensor_state;
} OxfordLambdaState;

typedef struct OxfordIpsReadingsTag
{
    double current_output_a;
    double negative_current_limit_a;
    double positive_current_limit_a;
    double current_setpoint_a;
    double current_sweep_rate_a_per_min;
    double magnet_current_a;
    double persistent_current_a;
    double trip_current_a;
    double supply_voltage_v;
    double voltage_limit_v;
    double field_output_t;
    double persistent_field_t;
    double trip_field_t;
    double field_setpoint_t;
    double field_sweep_rate_t_per_min;
    double switch_heater_current_ma;
    double lead_resistance_mohm;
    double magnet_inductance_h;
} OxfordIpsReadings;

typedef struct OxfordIpsStateTag
{
    int system_m;
    int system_n;
    int activity;
    int control_state;
    int switch_heater;
    int mode_m;
    int mode_n;
    int polarity_m;
    int polarity_n;
} OxfordIpsState;

typedef struct OxfordSnapshotTag
{
    OxfordIlmReadings ilm;
    OxfordItcReadings itc;
    OxfordLambdaReadings lambda;
    OxfordIpsReadings ips;
} OxfordSnapshot;

static void oxford_session_init(OxfordSession *session)
{
    session->port = INVALID_HANDLE_VALUE;
    session->open = 0;
}

static int oxford_close(OxfordSession *session)
{
    BOOL ok;

    if (session == NULL)
        return OXFORD_ERR_ARGUMENT;
    if (session->port == INVALID_HANDLE_VALUE)
    {
        session->open = 0;
        return OXFORD_OK;
    }

    ok = CloseHandle(session->port);
    session->port = INVALID_HANDLE_VALUE;
    session->open = 0;
    return ok ? OXFORD_OK : OXFORD_ERR_CLOSE;
}

static int oxford_open(OxfordSession *session)
{
    DCB config;
    COMMTIMEOUTS timeouts;

    if (session == NULL)
        return OXFORD_ERR_ARGUMENT;

    oxford_session_init(session);
    session->port = CreateFileA(OXFORD_COM_PORT,
                                GENERIC_READ | GENERIC_WRITE,
                                0,
                                NULL,
                                OPEN_EXISTING,
                                0,
                                NULL);
    if (session->port == INVALID_HANDLE_VALUE)
        return OXFORD_ERR_OPEN;
    session->open = 1;

    if (!SetupComm(session->port, 4096, 4096))
    {
        oxford_close(session);
        return OXFORD_ERR_CONFIG;
    }

    memset(&config, 0, sizeof(config));
    config.DCBlength = sizeof(config);
    if (!GetCommState(session->port, &config))
    {
        oxford_close(session);
        return OXFORD_ERR_CONFIG;
    }

    config.BaudRate = CBR_9600;
    config.ByteSize = 8;
    config.Parity = NOPARITY;
    config.StopBits = TWOSTOPBITS;
    config.fBinary = TRUE;
    config.fParity = FALSE;
    config.fOutxCtsFlow = FALSE;
    config.fOutxDsrFlow = FALSE;
    config.fDtrControl = DTR_CONTROL_ENABLE;
    config.fDsrSensitivity = FALSE;
    config.fTXContinueOnXoff = TRUE;
    config.fOutX = FALSE;
    config.fInX = FALSE;
    config.fErrorChar = FALSE;
    config.fNull = FALSE;
    config.fRtsControl = RTS_CONTROL_ENABLE;
    config.fAbortOnError = FALSE;

    if (!SetCommState(session->port, &config))
    {
        oxford_close(session);
        return OXFORD_ERR_CONFIG;
    }

    memset(&timeouts, 0, sizeof(timeouts));
    timeouts.ReadIntervalTimeout = OXFORD_READ_SLICE_MS;
    timeouts.ReadTotalTimeoutConstant = OXFORD_READ_SLICE_MS;
    timeouts.WriteTotalTimeoutConstant = OXFORD_WRITE_TIMEOUT_MS;
    if (!SetCommTimeouts(session->port, &timeouts))
    {
        oxford_close(session);
        return OXFORD_ERR_CONFIG;
    }

    if (!EscapeCommFunction(session->port, SETDTR) ||
        !EscapeCommFunction(session->port, SETRTS))
    {
        oxford_close(session);
        return OXFORD_ERR_CONFIG;
    }

    Sleep(OXFORD_ADAPTER_SETTLE_MS);
    if (!PurgeComm(session->port,
                   PURGE_RXABORT | PURGE_RXCLEAR |
                   PURGE_TXABORT | PURGE_TXCLEAR))
    {
        oxford_close(session);
        return OXFORD_ERR_PURGE;
    }

    return OXFORD_OK;
}

static int oxford_write_frame(OxfordSession *session, const char *frame)
{
    DWORD length;
    DWORD written;

    if (session == NULL || !session->open || frame == NULL)
        return OXFORD_ERR_ARGUMENT;

    length = (DWORD)strlen(frame);
    written = 0;
    if (!WriteFile(session->port, frame, length, &written, NULL))
        return OXFORD_ERR_WRITE;
    if (written != length)
        return OXFORD_ERR_WRITE;
    if (!FlushFileBuffers(session->port))
        return OXFORD_ERR_WRITE;
    return OXFORD_OK;
}

static int oxford_read_record(OxfordSession *session,
                              char *response,
                              int capacity)
{
    DWORD start;
    DWORD count;
    DWORD received;
    char c;

    if (session == NULL || !session->open ||
        response == NULL || capacity < 2)
        return OXFORD_ERR_ARGUMENT;

    start = GetTickCount();
    count = 0;
    response[0] = '\0';

    while ((DWORD)(GetTickCount() - start) < OXFORD_READ_TIMEOUT_MS)
    {
        received = 0;
        if (!ReadFile(session->port, &c, 1, &received, NULL))
            return OXFORD_ERR_READ;
        if (received == 0)
            continue;
        if (c == '\r')
        {
            response[count] = '\0';
            return count == 0 ? OXFORD_ERR_RESPONSE : OXFORD_OK;
        }
        if (c == '\n' && count == 0)
            continue;
        if (count >= (DWORD)(capacity - 1))
            return OXFORD_ERR_RESPONSE;
        response[count++] = c;
    }

    response[count] = '\0';
    return OXFORD_ERR_TIMEOUT;
}

static int oxford_prepare_device(OxfordSession *session, int address)
{
    char frame[16];
    int status;

    if (address < 0 || address > 9)
        return OXFORD_ERR_ARGUMENT;

    sprintf(frame, "@%dQ2\r", address);
    status = oxford_write_frame(session, frame);
    if (status != OXFORD_OK)
        return status;

    Sleep(OXFORD_Q2_SETTLE_MS);
    if (!PurgeComm(session->port, PURGE_RXABORT | PURGE_RXCLEAR))
        return OXFORD_ERR_PURGE;
    return OXFORD_OK;
}

static int oxford_ask(OxfordSession *session,
                      int address,
                      const char *command,
                      char *response,
                      int capacity)
{
    char frame[OXFORD_MAX_FRAME];
    int status;

    if (command == NULL || response == NULL ||
        address < 0 || address > 9)
        return OXFORD_ERR_ARGUMENT;
    if ((int)strlen(command) + 4 >= OXFORD_MAX_FRAME)
        return OXFORD_ERR_ARGUMENT;

    sprintf(frame, "@%d%s\r", address, command);
    if (!PurgeComm(session->port, PURGE_RXABORT | PURGE_RXCLEAR))
        return OXFORD_ERR_PURGE;

    Sleep(OXFORD_INTERCOMMAND_MS);
    status = oxford_write_frame(session, frame);
    if (status != OXFORD_OK)
        return status;

    status = oxford_read_record(session, response, capacity);
    if (status != OXFORD_OK)
        return status;

    if (strlen(response) == strlen(frame) - 1 &&
        strncmp(response, frame, strlen(response)) == 0)
    {
        status = oxford_read_record(session, response, capacity);
        if (status != OXFORD_OK)
            return status;
    }

    return response[0] == '?' ? OXFORD_ERR_DEVICE : OXFORD_OK;
}

static int oxford_write_no_response(OxfordSession *session,
                                    int address,
                                    const char *command)
{
    char frame[OXFORD_MAX_FRAME];

    if (command == NULL || address < 0 || address > 9)
        return OXFORD_ERR_ARGUMENT;
    if ((int)strlen(command) + 5 >= OXFORD_MAX_FRAME)
        return OXFORD_ERR_ARGUMENT;

    Sleep(OXFORD_INTERCOMMAND_MS);
    sprintf(frame, "$@%d%s\r", address, command);
    return oxford_write_frame(session, frame);
}

static int oxford_finish(OxfordSession *session, int status)
{
    int close_status;

    close_status = oxford_close(session);
    return status == OXFORD_OK ? close_status : status;
}

static int oxford_parse_double(const char *response,
                               char prefix,
                               double *value)
{
    char *end;
    double parsed;

    if (response == NULL || value == NULL || response[0] != prefix)
        return OXFORD_ERR_RESPONSE;

    parsed = strtod(response + 1, &end);
    if (end == response + 1)
        return OXFORD_ERR_RESPONSE;
    while (*end != '\0' && isspace((unsigned char)*end))
        ++end;
    if (*end != '\0')
        return OXFORD_ERR_RESPONSE;

    *value = parsed;
    return OXFORD_OK;
}

static int oxford_parse_int(const char *response,
                            char prefix,
                            int *value)
{
    char *end;
    long parsed;

    if (response == NULL || value == NULL || response[0] != prefix)
        return OXFORD_ERR_RESPONSE;

    parsed = strtol(response + 1, &end, 10);
    if (end == response + 1)
        return OXFORD_ERR_RESPONSE;
    while (*end != '\0' && isspace((unsigned char)*end))
        ++end;
    if (*end != '\0')
        return OXFORD_ERR_RESPONSE;

    *value = (int)parsed;
    return OXFORD_OK;
}

static int oxford_get_double(OxfordSession *session,
                             int address,
                             int parameter,
                             double *value)
{
    char command[16];
    char response[OXFORD_MAX_FRAME];
    int status;

    sprintf(command, "R%d", parameter);
    status = oxford_ask(session, address, command,
                        response, sizeof(response));
    return status == OXFORD_OK
        ? oxford_parse_double(response, 'R', value)
        : status;
}

static int oxford_get_int(OxfordSession *session,
                          int address,
                          int parameter,
                          int *value)
{
    char command[16];
    char response[OXFORD_MAX_FRAME];
    int status;

    sprintf(command, "R%d", parameter);
    status = oxford_ask(session, address, command,
                        response, sizeof(response));
    return status == OXFORD_OK
        ? oxford_parse_int(response, 'R', value)
        : status;
}

static int oxford_command(OxfordSession *session,
                          int address,
                          const char *command)
{
    char response[OXFORD_MAX_FRAME];

    return oxford_ask(session, address, command,
                      response, sizeof(response));
}

static int oxford_get_state_text(OxfordSession *session,
                                 int address,
                                 char *state)
{
    return oxford_ask(session, address, "X",
                      state, OXFORD_MAX_FRAME);
}

static int oxford_digit(char c, int *value)
{
    if (value == NULL || c < '0' || c > '9')
        return OXFORD_ERR_RESPONSE;
    *value = c - '0';
    return OXFORD_OK;
}

static int oxford_hex_pair(const char *text, int *value)
{
    char pair[3];
    char *end;

    if (text == NULL || value == NULL ||
        !isxdigit((unsigned char)text[0]) ||
        !isxdigit((unsigned char)text[1]))
        return OXFORD_ERR_RESPONSE;

    pair[0] = text[0];
    pair[1] = text[1];
    pair[2] = '\0';
    *value = (int)strtol(pair, &end, 16);
    return *end == '\0' ? OXFORD_OK : OXFORD_ERR_RESPONSE;
}

static int oxford_two_digits(const char *text, int *value)
{
    if (text == NULL || value == NULL ||
        !isdigit((unsigned char)text[0]) ||
        !isdigit((unsigned char)text[1]))
        return OXFORD_ERR_RESPONSE;
    *value = (text[0] - '0') * 10 + (text[1] - '0');
    return OXFORD_OK;
}

static int oxford_set_control_state(OxfordSession *session,
                                    int address,
                                    int state)
{
    char command[16];

    sprintf(command, "C%d", state);
    return oxford_command(session, address, command);
}

static int oxford_check_communication(OxfordSession *session, int address)
{
    char response[OXFORD_MAX_FRAME];

    return oxford_ask(session, address, "V",
                      response, sizeof(response));
}

static int oxford_ilm_get_state(OxfordSession *session,
                                OxfordIlmState *state)
{
    char response[OXFORD_MAX_FRAME];
    int status;

    if (state == NULL)
        return OXFORD_ERR_ARGUMENT;

    status = oxford_get_state_text(session, OXFORD_ILM_ADDRESS, response);
    if (status != OXFORD_OK)
        return status;
    if (strlen(response) < 14 || response[0] != 'X')
        return OXFORD_ERR_RESPONSE;

    status = oxford_digit(response[1], &state->channel_1_usage);
    if (status == OXFORD_OK)
        status = oxford_digit(response[2], &state->channel_2_usage);
    if (status == OXFORD_OK)
        status = oxford_digit(response[3], &state->channel_3_usage);
    if (status == OXFORD_OK)
        status = oxford_hex_pair(response + 5, &state->channel_1_status);
    if (status == OXFORD_OK)
        status = oxford_hex_pair(response + 7, &state->channel_2_status);
    if (status == OXFORD_OK)
        status = oxford_hex_pair(response + 9, &state->channel_3_status);
    if (status == OXFORD_OK)
        status = oxford_hex_pair(response + 12, &state->relay_status);
    return status;
}

static int oxford_ilm_read(OxfordSession *session,
                           OxfordIlmReadings *readings)
{
    int helium;
    int nitrogen;
    int status;

    if (readings == NULL)
        return OXFORD_ERR_ARGUMENT;

    status = oxford_get_int(session, OXFORD_ILM_ADDRESS, 1, &helium);
    if (status == OXFORD_OK)
        status = oxford_get_int(session, OXFORD_ILM_ADDRESS, 2, &nitrogen);
    if (status == OXFORD_OK)
        status = oxford_get_int(session, OXFORD_ILM_ADDRESS, 6,
                                &readings->channel_1_current);
    if (status == OXFORD_OK)
        status = oxford_get_int(session, OXFORD_ILM_ADDRESS, 7,
                                &readings->channel_2_current);
    if (status == OXFORD_OK)
        status = oxford_get_int(session, OXFORD_ILM_ADDRESS, 10,
                                &readings->valve_position);
    if (status == OXFORD_OK)
    {
        readings->helium_percent = (double)helium / 10.0;
        readings->nitrogen_percent = (double)nitrogen / 10.0;
    }
    return status;
}

static int oxford_ilm_set_control_state(OxfordSession *session, int state)
{
    if (state != 0 && state != 1 && state != 3)
        return OXFORD_ERR_RANGE;
    return oxford_set_control_state(session, OXFORD_ILM_ADDRESS, state);
}

static int oxford_ilm_set_probe_speed(OxfordSession *session, int fast)
{
    int status;

    status = oxford_command(session, OXFORD_ILM_ADDRESS,
                            fast ? "T1" : "S1");
    if (status == OXFORD_OK && fast)
        Sleep(6000UL);
    return status;
}

static int oxford_ilm_set_stepper_position(OxfordSession *session,
                                           int position)
{
    char command[16];

    sprintf(command, "G%d", position);
    return oxford_command(session, OXFORD_ILM_ADDRESS, command);
}

static int oxford_itc_get_state(OxfordSession *session,
                                OxfordItcState *state)
{
    char response[OXFORD_MAX_FRAME];
    int status;

    if (state == NULL)
        return OXFORD_ERR_ARGUMENT;

    status = oxford_get_state_text(session, OXFORD_ITC_ADDRESS, response);
    if (status != OXFORD_OK)
        return status;
    if (strlen(response) < 13 || response[0] != 'X' ||
        response[2] != 'A' || response[4] != 'C' ||
        response[6] != 'S' || response[9] != 'H' ||
        response[11] != 'L')
        return OXFORD_ERR_RESPONSE;

    status = oxford_digit(response[1], &state->system_state);
    if (status == OXFORD_OK)
        status = oxford_digit(response[3], &state->auto_manual_state);
    if (status == OXFORD_OK)
        status = oxford_digit(response[5], &state->control_state);
    if (status == OXFORD_OK)
        status = oxford_two_digits(response + 7, &state->sweep_state);
    if (status == OXFORD_OK)
        status = oxford_digit(response[10], &state->heater_sensor);
    if (status == OXFORD_OK)
        status = oxford_digit(response[12], &state->auto_pid_state);
    return status;
}

static int oxford_itc_read(OxfordSession *session,
                           OxfordItcReadings *readings)
{
    double *values[11];
    int i;
    int status;

    if (readings == NULL)
        return OXFORD_ERR_ARGUMENT;

    values[0] = &readings->setpoint_k;
    values[1] = &readings->temperature_1_k;
    values[2] = &readings->temperature_2_k;
    values[3] = &readings->temperature_3_k;
    values[4] = &readings->error_k;
    values[5] = &readings->heater_percent;
    values[6] = &readings->heater_volts;
    values[7] = &readings->gas_percent;
    values[8] = &readings->proportional_band_k;
    values[9] = &readings->integral_minutes;
    values[10] = &readings->derivative_minutes;

    status = OXFORD_OK;
    for (i = 0; i < 11 && status == OXFORD_OK; ++i)
        status = oxford_get_double(session, OXFORD_ITC_ADDRESS,
                                   i, values[i]);
    return status;
}

static int oxford_itc_set_control_state(OxfordSession *session, int state)
{
    if (state < 0 || state > 3)
        return OXFORD_ERR_RANGE;
    return oxford_set_control_state(session, OXFORD_ITC_ADDRESS, state);
}

static int oxford_itc_set_auto_manual(OxfordSession *session, int mode)
{
    char command[16];

    if (mode < 0 || mode > 3)
        return OXFORD_ERR_RANGE;
    sprintf(command, "A%d", mode);
    return oxford_command(session, OXFORD_ITC_ADDRESS, command);
}

static int oxford_itc_set_heater_auto(OxfordSession *session, int automatic)
{
    OxfordItcState state;
    int mode;
    int status;

    status = oxford_itc_get_state(session, &state);
    if (status != OXFORD_OK)
        return status;

    if (state.auto_manual_state == 0 || state.auto_manual_state == 1)
        mode = automatic ? 1 : 0;
    else
        mode = automatic ? 3 : 2;
    return oxford_itc_set_auto_manual(session, mode);
}

static int oxford_itc_set_gas_auto(OxfordSession *session, int automatic)
{
    OxfordItcState state;
    int mode;
    int status;

    status = oxford_itc_get_state(session, &state);
    if (status != OXFORD_OK)
        return status;

    if (state.auto_manual_state == 0 || state.auto_manual_state == 2)
        mode = automatic ? 2 : 0;
    else
        mode = automatic ? 3 : 1;
    return oxford_itc_set_auto_manual(session, mode);
}

static int oxford_itc_set_gas_flow(OxfordSession *session, double value)
{
    char command[32];

    sprintf(command, "G%.1f", value);
    return oxford_command(session, OXFORD_ITC_ADDRESS, command);
}

static int oxford_itc_set_heater_sensor(OxfordSession *session, int sensor)
{
    char command[16];

    sprintf(command, "H%d", sensor);
    return oxford_command(session, OXFORD_ITC_ADDRESS, command);
}

static int oxford_itc_set_auto_pid(OxfordSession *session, int enabled)
{
    return oxford_command(session, OXFORD_ITC_ADDRESS,
                          enabled ? "L1" : "L0");
}

static int oxford_itc_set_max_heater_volts(OxfordSession *session,
                                           double volts)
{
    char command[32];

    sprintf(command, "M%.1f", volts);
    return oxford_command(session, OXFORD_ITC_ADDRESS, command);
}

static int oxford_itc_set_heater_output(OxfordSession *session,
                                        double percent)
{
    char command[32];

    sprintf(command, "O%.1f", percent);
    return oxford_command(session, OXFORD_ITC_ADDRESS, command);
}

static int oxford_itc_set_proportional_band(OxfordSession *session,
                                            double value)
{
    char command[32];

    sprintf(command, "P%.3f", value);
    return oxford_command(session, OXFORD_ITC_ADDRESS, command);
}

static int oxford_itc_set_integral_time(OxfordSession *session,
                                        double value)
{
    char command[32];

    sprintf(command, "I%.1f", value);
    return oxford_command(session, OXFORD_ITC_ADDRESS, command);
}

static int oxford_itc_set_derivative_time(OxfordSession *session,
                                          double value)
{
    char command[32];

    sprintf(command, "D%d", (int)value);
    return oxford_command(session, OXFORD_ITC_ADDRESS, command);
}

static int oxford_itc_set_sweep_state(OxfordSession *session, int running)
{
    return oxford_command(session, OXFORD_ITC_ADDRESS,
                          running ? "S1" : "S0");
}

static int oxford_itc_set_pid_and_setpoint(OxfordSession *session,
                                           double p,
                                           double i,
                                           double d,
                                           double temperature)
{
    char command[32];
    int status;

    sprintf(command, "P%.3f", p);
    status = oxford_command(session, OXFORD_ITC_ADDRESS, command);
    if (status == OXFORD_OK)
    {
        sprintf(command, "I%.1f", i);
        status = oxford_command(session, OXFORD_ITC_ADDRESS, command);
    }
    if (status == OXFORD_OK)
    {
        sprintf(command, "D%d", (int)d);
        status = oxford_command(session, OXFORD_ITC_ADDRESS, command);
    }
    if (status == OXFORD_OK)
    {
        sprintf(command, "T%f", temperature);
        status = oxford_command(session, OXFORD_ITC_ADDRESS, command);
    }
    return status;
}

static double oxford_interpolate(double x,
                                 const double *xs,
                                 const double *ys,
                                 int count)
{
    int i;
    double fraction;

    if (x <= xs[0])
        return ys[0];
    if (x >= xs[count - 1])
        return ys[count - 1];

    for (i = 1; i < count; ++i)
    {
        if (x <= xs[i])
        {
            fraction = (x - xs[i - 1]) / (xs[i] - xs[i - 1]);
            return ys[i - 1] + fraction * (ys[i] - ys[i - 1]);
        }
    }
    return ys[count - 1];
}

static void oxford_itc_pid(double temperature,
                           double *p,
                           double *i,
                           double *d)
{
    static const double t[] =
        {1.55, 3.0, 4.2, 6.0, 10.0, 20.0, 50.0, 77.0, 100.0, 200.0, 300.0};
    static const double pv[] =
        {60.0, 50.0, 50.0, 50.0, 50.0, 50.0, 5.0, 3.5, 3.5, 3.5, 3.5};
    static const double iv[] =
        {0.2, 0.2, 0.2, 0.2, 0.2, 0.2, 2.5, 2.5, 2.5, 2.5, 2.5};
    static const double dv[] =
        {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

    *p = oxford_interpolate(temperature, t, pv, 11);
    *i = oxford_interpolate(temperature, t, iv, 11);
    *d = oxford_interpolate(temperature, t, dv, 11);
}

static int oxford_itc_set_temperature(OxfordSession *session,
                                      double temperature)
{
    double p;
    double i;
    double d;

    oxford_itc_pid(temperature, &p, &i, &d);
    return oxford_itc_set_pid_and_setpoint(session, p, i, d, temperature);
}

static int oxford_lambda_get_state(OxfordSession *session,
                                   OxfordLambdaState *state)
{
    char response[OXFORD_MAX_FRAME];
    int status;

    if (state == NULL)
        return OXFORD_ERR_ARGUMENT;

    status = oxford_get_state_text(session, OXFORD_LAMBDA_ADDRESS, response);
    if (status != OXFORD_OK)
        return status;
    if (strlen(response) < 10 || response[0] != 'X')
        return OXFORD_ERR_RESPONSE;

    status = oxford_digit(response[1], &state->system_state);
    if (status == OXFORD_OK)
        status = oxford_digit(response[3], &state->activity_state);
    if (status == OXFORD_OK)
        status = oxford_digit(response[5], &state->control_state);
    if (status == OXFORD_OK)
        status = oxford_digit(response[7], &state->sweep_state);
    if (status == OXFORD_OK)
        status = oxford_digit(response[9], &state->sensor_state);
    return status;
}

static int oxford_lambda_read(OxfordSession *session,
                              OxfordLambdaReadings *readings)
{
    double pressure;
    double temperature;
    double valve;
    int status;

    if (readings == NULL)
        return OXFORD_ERR_ARGUMENT;

    status = oxford_get_double(session, OXFORD_LAMBDA_ADDRESS, 1, &pressure);
    if (status == OXFORD_OK)
        status = oxford_get_double(session, OXFORD_LAMBDA_ADDRESS, 2,
                                   &temperature);
    if (status == OXFORD_OK)
        status = oxford_get_double(session, OXFORD_LAMBDA_ADDRESS, 7, &valve);
    if (status == OXFORD_OK)
    {
        readings->pressure_mbar = pressure / 10.0;
        readings->temperature_k = temperature / 100.0;
        readings->valve_percent = valve / 10.0;
    }
    return status;
}

static int oxford_lambda_set_control_state(OxfordSession *session, int state)
{
    if (state != 0 && state != 3)
        return OXFORD_ERR_RANGE;
    return oxford_set_control_state(session, OXFORD_LAMBDA_ADDRESS, state);
}

static int oxford_lambda_set_gas_flow(OxfordSession *session, double value)
{
    char command[32];

    sprintf(command, "G%d", (int)(value * 10.0));
    return oxford_command(session, OXFORD_LAMBDA_ADDRESS, command);
}

static int oxford_lambda_load_linearizer(OxfordSession *session, int table)
{
    char command[16];

    sprintf(command, "L%d", table);
    return oxford_write_no_response(session, OXFORD_LAMBDA_ADDRESS, command);
}

static int oxford_ips_get_state(OxfordSession *session,
                                OxfordIpsState *state)
{
    char response[OXFORD_MAX_FRAME];
    int status;

    if (state == NULL)
        return OXFORD_ERR_ARGUMENT;

    status = oxford_get_state_text(session, OXFORD_IPS_ADDRESS, response);
    if (status != OXFORD_OK)
        return status;
    if (strlen(response) < 15 || response[0] != 'X')
        return OXFORD_ERR_RESPONSE;

    status = oxford_digit(response[1], &state->system_m);
    if (status == OXFORD_OK)
        status = oxford_digit(response[2], &state->system_n);
    if (status == OXFORD_OK)
        status = oxford_digit(response[4], &state->activity);
    if (status == OXFORD_OK)
        status = oxford_digit(response[6], &state->control_state);
    if (status == OXFORD_OK)
        status = oxford_digit(response[8], &state->switch_heater);
    if (status == OXFORD_OK)
        status = oxford_digit(response[10], &state->mode_m);
    if (status == OXFORD_OK)
        status = oxford_digit(response[11], &state->mode_n);
    if (status == OXFORD_OK)
        status = oxford_digit(response[13], &state->polarity_m);
    if (status == OXFORD_OK)
        status = oxford_digit(response[14], &state->polarity_n);
    return status;
}

static int oxford_ips_read(OxfordSession *session,
                           OxfordIpsReadings *readings)
{
    int parameters[18];
    double *values[18];
    int i;
    int status;

    if (readings == NULL)
        return OXFORD_ERR_ARGUMENT;

    parameters[0] = 0;
    parameters[1] = 21;
    parameters[2] = 22;
    parameters[3] = 5;
    parameters[4] = 6;
    parameters[5] = 2;
    parameters[6] = 16;
    parameters[7] = 17;
    parameters[8] = 1;
    parameters[9] = 15;
    parameters[10] = 7;
    parameters[11] = 18;
    parameters[12] = 19;
    parameters[13] = 8;
    parameters[14] = 9;
    parameters[15] = 20;
    parameters[16] = 23;
    parameters[17] = 24;

    values[0] = &readings->current_output_a;
    values[1] = &readings->negative_current_limit_a;
    values[2] = &readings->positive_current_limit_a;
    values[3] = &readings->current_setpoint_a;
    values[4] = &readings->current_sweep_rate_a_per_min;
    values[5] = &readings->magnet_current_a;
    values[6] = &readings->persistent_current_a;
    values[7] = &readings->trip_current_a;
    values[8] = &readings->supply_voltage_v;
    values[9] = &readings->voltage_limit_v;
    values[10] = &readings->field_output_t;
    values[11] = &readings->persistent_field_t;
    values[12] = &readings->trip_field_t;
    values[13] = &readings->field_setpoint_t;
    values[14] = &readings->field_sweep_rate_t_per_min;
    values[15] = &readings->switch_heater_current_ma;
    values[16] = &readings->lead_resistance_mohm;
    values[17] = &readings->magnet_inductance_h;

    status = OXFORD_OK;
    for (i = 0; i < 18 && status == OXFORD_OK; ++i)
        status = oxford_get_double(session, OXFORD_IPS_ADDRESS,
                                   parameters[i], values[i]);
    return status;
}

static int oxford_ips_set_control_state(OxfordSession *session, int state)
{
    if (state < 0 || state > 3)
        return OXFORD_ERR_RANGE;
    return oxford_set_control_state(session, OXFORD_IPS_ADDRESS, state);
}

static int oxford_ips_set_activity(OxfordSession *session, int activity)
{
    char command[16];

    if (activity != 0 && activity != 1 &&
        activity != 2 && activity != 4)
        return OXFORD_ERR_RANGE;
    sprintf(command, "A%d", activity);
    return oxford_command(session, OXFORD_IPS_ADDRESS, command);
}

static int oxford_ips_set_switch_heater(OxfordSession *session, int state)
{
    char command[16];

    if (state < 0 || state > 2)
        return OXFORD_ERR_RANGE;
    sprintf(command, "H%d", state);
    return oxford_command(session, OXFORD_IPS_ADDRESS, command);
}

static int oxford_ips_set_current(OxfordSession *session, double current)
{
    char command[32];

    sprintf(command, "I%.4f", current);
    return oxford_command(session, OXFORD_IPS_ADDRESS, command);
}

static int oxford_ips_set_field(OxfordSession *session, double field)
{
    char command[32];

    sprintf(command, "J%.4f", field);
    return oxford_command(session, OXFORD_IPS_ADDRESS, command);
}

static int oxford_ips_set_mode(OxfordSession *session, int mode)
{
    char command[16];

    if (mode != 0 && mode != 1 && mode != 4 &&
        mode != 5 && mode != 8 && mode != 9)
        return OXFORD_ERR_RANGE;
    sprintf(command, "M%d", mode);
    return oxford_command(session, OXFORD_IPS_ADDRESS, command);
}

static int oxford_ips_set_polarity(OxfordSession *session, int polarity)
{
    char command[16];

    if (polarity != 0 && polarity != 1 &&
        polarity != 2 && polarity != 4)
        return OXFORD_ERR_RANGE;
    sprintf(command, "P%d", polarity);
    return oxford_command(session, OXFORD_IPS_ADDRESS, command);
}

static int oxford_ips_set_current_sweep_rate(OxfordSession *session,
                                             double rate)
{
    char command[32];

    sprintf(command, "S%f", rate);
    return oxford_command(session, OXFORD_IPS_ADDRESS, command);
}

static int oxford_ips_set_field_sweep_rate(OxfordSession *session,
                                           double rate)
{
    char command[32];

    sprintf(command, "T%.4f", rate);
    return oxford_command(session, OXFORD_IPS_ADDRESS, command);
}

static int oxford_ips_set_control_parameter(OxfordSession *session,
                                            int tesla)
{
    return oxford_ips_set_mode(session, tesla ? 9 : 8);
}

static int oxford_ips_set_sweep_speed(OxfordSession *session, int slow)
{
    OxfordIpsState state;
    int status;
    int amps;

    status = oxford_ips_get_state(session, &state);
    if (status != OXFORD_OK)
        return status;

    amps = state.mode_m == 0 || state.mode_m == 4;
    if (slow)
        return oxford_ips_set_mode(session, amps ? 4 : 5);
    return oxford_ips_set_mode(session, amps ? 0 : 1);
}

static int oxford_set_protocol(OxfordSession *session,
                               int address,
                               int protocol)
{
    char command[16];

    sprintf(command, "Q%d", protocol);
    return oxford_write_no_response(session, address, command);
}

static int oxford_set_system_lock(OxfordSession *session,
                                  int address,
                                  int state)
{
    char command[16];

    sprintf(command, "U%d", state);
    return oxford_command(session, address, command);
}

static int oxford_set_output_wait(OxfordSession *session,
                                  int address,
                                  int milliseconds)
{
    char command[16];

    sprintf(command, "W%d", milliseconds);
    return oxford_command(session, address, command);
}

static int oxford_set_display_source(OxfordSession *session,
                                     int address,
                                     int parameter)
{
    char command[16];

    sprintf(command, "F%d", parameter);
    return oxford_command(session, address, command);
}

static int oxford_load_ram(OxfordSession *session, int address, int size_kb)
{
    char command[16];

    sprintf(command, "Y%d", size_kb);
    return oxford_write_no_response(session, address, command);
}

static int oxford_dump_ram(OxfordSession *session, int address, int size_kb)
{
    char command[16];

    sprintf(command, "Z%d", size_kb);
    return oxford_write_no_response(session, address, command);
}

static int oxford_change_address(OxfordSession *session,
                                 int old_address,
                                 int new_address)
{
    char command[16];

    sprintf(command, "!%d", new_address);
    return oxford_write_no_response(session, old_address, command);
}

static int oxford_store_eeprom(OxfordSession *session, int address)
{
    return oxford_command(session, address, "~");
}

static int oxford_prepare_all(OxfordSession *session)
{
    int status;

    status = oxford_prepare_device(session, OXFORD_ILM_ADDRESS);
    if (status == OXFORD_OK)
        status = oxford_prepare_device(session, OXFORD_ITC_ADDRESS);
    if (status == OXFORD_OK)
        status = oxford_prepare_device(session, OXFORD_LAMBDA_ADDRESS);
    if (status == OXFORD_OK)
        status = oxford_prepare_device(session, OXFORD_IPS_ADDRESS);
    return status;
}

static int oxford_read_snapshot(OxfordSession *session,
                                OxfordSnapshot *snapshot)
{
    int status;

    if (snapshot == NULL)
        return OXFORD_ERR_ARGUMENT;

    status = oxford_ilm_read(session, &snapshot->ilm);
    if (status == OXFORD_OK)
        status = oxford_itc_read(session, &snapshot->itc);
    if (status == OXFORD_OK)
        status = oxford_lambda_read(session, &snapshot->lambda);
    if (status == OXFORD_OK)
        status = oxford_ips_read(session, &snapshot->ips);
    return status;
}

static int oxford_controller_initialize(OxfordSession *session)
{
    int status;

    status = oxford_check_communication(session, OXFORD_ILM_ADDRESS);
    if (status == OXFORD_OK)
        status = oxford_check_communication(session, OXFORD_ITC_ADDRESS);
    if (status == OXFORD_OK)
        status = oxford_check_communication(session, OXFORD_LAMBDA_ADDRESS);
    if (status == OXFORD_OK)
        status = oxford_check_communication(session, OXFORD_IPS_ADDRESS);
    if (status == OXFORD_OK)
        status = oxford_ips_set_control_state(session, 3);
    if (status == OXFORD_OK)
        status = oxford_ips_set_field_sweep_rate(session, 0.3382);
    if (status == OXFORD_OK)
        status = oxford_ips_set_control_state(session, 2);
    return status;
}

static int oxford_controller_configure_temperature(OxfordSession *session,
                                                   double temperature)
{
    int status;

    status = oxford_itc_set_control_state(session, 3);
    if (status == OXFORD_OK)
        status = oxford_itc_set_gas_auto(session, 0);
    if (status == OXFORD_OK)
        status = oxford_itc_set_heater_auto(session, 0);
    if (status == OXFORD_OK)
        status = oxford_itc_set_heater_sensor(session, 2);
    if (status == OXFORD_OK)
        status = oxford_itc_set_temperature(session, temperature);
    if (status == OXFORD_OK)
        status = oxford_itc_set_gas_auto(session, 1);
    if (status == OXFORD_OK)
        status = oxford_itc_set_heater_auto(session, 1);
    if (status == OXFORD_OK)
        status = oxford_itc_set_control_state(session, 2);
    return status;
}

static int oxford_controller_set_temperature(OxfordSession *session,
                                             double temperature,
                                             double *final_temperature)
{
    OxfordSnapshot snapshot;
    double errors[20];
    double sum;
    double error;
    int count;
    int position;
    int status;

    if (final_temperature == NULL)
        return OXFORD_ERR_ARGUMENT;

    status = oxford_controller_configure_temperature(session, temperature);
    count = 0;
    position = 0;
    sum = 0.0;

    while (status == OXFORD_OK)
    {
        status = oxford_read_snapshot(session, &snapshot);
        if (status != OXFORD_OK)
            break;

        error = fabs(snapshot.itc.error_k);
        if (count < 20)
        {
            errors[position] = error;
            sum += error;
            ++count;
        }
        else
        {
            sum -= errors[position];
            errors[position] = error;
            sum += error;
        }
        position = (position + 1) % 20;

        if (sum / (double)count < 0.05)
        {
            *final_temperature = snapshot.itc.temperature_1_k;
            break;
        }
    }
    return status;
}

static int oxford_controller_couple_magnet(OxfordSession *session)
{
    int status;

    status = oxford_ips_set_control_state(session, 3);
    if (status == OXFORD_OK)
        status = oxford_ips_set_activity(session, 0);
    if (status == OXFORD_OK)
        status = oxford_ips_set_switch_heater(session, 1);
    if (status == OXFORD_OK)
        status = oxford_ips_set_control_state(session, 2);
    return status;
}

static int oxford_controller_decouple_magnet(OxfordSession *session)
{
    int status;

    status = oxford_ips_set_control_state(session, 3);
    if (status == OXFORD_OK)
        status = oxford_ips_set_activity(session, 0);
    if (status == OXFORD_OK)
        status = oxford_ips_set_switch_heater(session, 0);
    if (status == OXFORD_OK)
        status = oxford_ips_set_control_state(session, 2);
    return status;
}

static int oxford_controller_set_source_field(OxfordSession *session,
                                              double field)
{
    int status;

    status = oxford_ips_set_control_state(session, 3);
    if (status == OXFORD_OK)
        status = oxford_ips_set_control_parameter(session, 1);
    if (status == OXFORD_OK)
        status = oxford_ips_set_activity(session, 0);
    if (status == OXFORD_OK)
        status = oxford_ips_set_field(session, field);
    if (status == OXFORD_OK)
        status = oxford_ips_set_activity(session, 1);
    if (status == OXFORD_OK)
        status = oxford_ips_set_control_state(session, 2);
    return status;
}

static int oxford_controller_goto_zero(OxfordSession *session)
{
    int status;

    status = oxford_ips_set_control_state(session, 3);
    if (status == OXFORD_OK)
        status = oxford_ips_set_activity(session, 2);
    if (status == OXFORD_OK)
        status = oxford_ips_set_control_state(session, 2);
    return status;
}

static int oxford_wait_for_field(OxfordSession *session,
                                 double field,
                                 double *last_field)
{
    OxfordSnapshot snapshot;
    double errors[5];
    double sum;
    double error;
    int count;
    int position;
    int status;

    count = 0;
    position = 0;
    sum = 0.0;
    status = OXFORD_OK;

    while (status == OXFORD_OK)
    {
        status = oxford_read_snapshot(session, &snapshot);
        if (status != OXFORD_OK)
            break;

        *last_field = snapshot.ips.field_output_t;
        error = fabs(*last_field - field);
        if (count < 5)
        {
            errors[position] = error;
            sum += error;
            ++count;
        }
        else
        {
            sum -= errors[position];
            errors[position] = error;
            sum += error;
        }
        position = (position + 1) % 5;

        if (sum / (double)count < 0.0001)
            break;
    }
    return status;
}

static int oxford_controller_set_magnetic_field(OxfordSession *session,
                                                double field,
                                                double *final_source_field)
{
    OxfordSnapshot snapshot;
    double current_field;
    double persistent_field;
    int status;

    if (final_source_field == NULL)
        return OXFORD_ERR_ARGUMENT;

    status = oxford_read_snapshot(session, &snapshot);
    if (status != OXFORD_OK)
        return status;

    current_field = snapshot.ips.field_output_t;
    persistent_field = snapshot.ips.persistent_field_t;

    if (persistent_field == field)
    {
        *final_source_field = current_field;
        return OXFORD_OK;
    }

    if (current_field == persistent_field)
    {
        status = oxford_controller_couple_magnet(session);
        if (status == OXFORD_OK)
            Sleep(60000UL);
    }
    else
    {
        status = oxford_controller_set_source_field(session, persistent_field);
        if (status == OXFORD_OK)
            status = oxford_wait_for_field(session, persistent_field,
                                           &current_field);
        if (status == OXFORD_OK)
            status = oxford_controller_couple_magnet(session);
        if (status == OXFORD_OK)
            Sleep(60000UL);
    }

    if (status == OXFORD_OK)
        status = oxford_controller_set_source_field(session, field);
    if (status == OXFORD_OK)
        status = oxford_wait_for_field(session, field, &current_field);
    if (status == OXFORD_OK)
        status = oxford_controller_decouple_magnet(session);
    if (status == OXFORD_OK)
        Sleep(60000UL);
    if (status == OXFORD_OK)
        status = oxford_controller_goto_zero(session);
    if (status == OXFORD_OK)
        status = oxford_wait_for_field(session, 0.0, &current_field);

    *final_source_field = current_field;
    return status;
}

static int oxford_controller_standby(OxfordSession *session,
                                     double *final_source_field)
{
    int status;

    status = oxford_itc_set_control_state(session, 3);
    if (status == OXFORD_OK)
        status = oxford_itc_set_gas_auto(session, 0);
    if (status == OXFORD_OK)
        status = oxford_itc_set_gas_flow(session, 0.0);
    if (status == OXFORD_OK)
        status = oxford_itc_set_heater_auto(session, 0);
    if (status == OXFORD_OK)
        status = oxford_itc_set_heater_output(session, 0.0);
    if (status == OXFORD_OK)
        status = oxford_itc_set_control_state(session, 2);
    if (status == OXFORD_OK)
        status = oxford_ilm_set_control_state(session, 3);
    if (status == OXFORD_OK)
        status = oxford_ilm_set_probe_speed(session, 0);
    if (status == OXFORD_OK)
        status = oxford_ilm_set_control_state(session, 0);
    if (status == OXFORD_OK)
        status = oxford_controller_set_magnetic_field(session, 0.0,
                                                      final_source_field);
    return status;
}

static int oxford_controller_active(OxfordSession *session)
{
    int status;

    status = oxford_ilm_set_control_state(session, 3);
    if (status == OXFORD_OK)
        status = oxford_ilm_set_probe_speed(session, 1);
    if (status == OXFORD_OK)
        status = oxford_ilm_set_control_state(session, 0);
    return status;
}

static int oxford_cycle_index(int count, int *cycle, int *index)
{
    int current;

    if (cycle == NULL || index == NULL || count < 1)
        return OXFORD_ERR_ARGUMENT;

    current = GetKiteCycle();
    if (current < 1 || current > count)
        return OXFORD_ERR_CYCLE;

    *cycle = current;
    *index = current - 1;
    return OXFORD_OK;
}

#endif

