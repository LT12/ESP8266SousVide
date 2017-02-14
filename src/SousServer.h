#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <PID_v1.h>
#include <OneWire.h>
#include <SimpleTimer.h>
#include <DallasTemperature.h>
#ifndef SOUSSERVER_H
#define SOUSSERVER_H

ESP8266WebServer server(80);
const size_t bufferSize = JSON_OBJECT_SIZE(10);
bool turned_on = false, timer_status = false, tuning = false;
unsigned long startMillis, timer = 0;
SimpleTimer ftimer;
int temp_timer, targ_timer;
double temp = 0, Output = 0, targ = 55, setpoint = 55;
double Kp, Ki, Kd;
const char *ssid = "";
const char *password = "";
PID myPID(&temp, &Output, &targ, 0, 0, 0, DIRECT);
OneWire oneWire(D2);
DallasTemperature DS18B20(&oneWire);

void setupOTA(void);

#endif // SOUSSERVER_H
