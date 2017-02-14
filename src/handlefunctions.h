#include <PID_v1.h>
#include <ESP8266WebServer.h>
#ifndef HANDLEFUNCTIONS_H
#define HANDLEFUNCTIONS_H
void handleStatus(void);
void handleWebsite(void);
void handleControl(void);
void handleData(void);
void handleJavascript(void);
void handleStyle(void);

extern ESP8266WebServer server;
extern void changeAutoTune(void);
extern bool turned_on, timer_status, tuning;
extern unsigned long startMillis, timer;
extern double temp, Output, targ, setpoint;
extern double Kp, Ki, Kd;
extern const char *ssid ;
extern const char *password ;
extern PID myPID;
extern void writePIDTunings();
#endif // HANDLEFUNCTIONS_H
