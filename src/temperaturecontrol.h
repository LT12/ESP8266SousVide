#include <PID_v1.h>
#include <DallasTemperature.h>
#include <PID_AutoTune_v0.h>
#ifndef TEMPERATURECONTROL_H
#define TEMPERATURECONTROL_H

extern double Kp, Ki, Kd, targ, setpoint, temp, Output;
extern DallasTemperature DS18B20;
extern PID myPID;
extern bool tuning, turned_on;


void readPIDTunings(void);
void updateTemp(void);
void incSetpoint(void);
void pidCompute(void);
void AutoTuneHelper(bool);



#endif // TEMPERATURECONTROL_H
