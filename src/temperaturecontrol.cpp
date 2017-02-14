#include"FS.h"
#include "temperaturecontrol.h"
#include <ArduinoJson.h>
#include <PID_AutoTune_v0.h>

PID_ATune aTune(&temp, &Output);
double aTuneStep = 50, aTuneNoise = 2, aTuneStartValue = 150;
double ramprate = 5; // celsius per minute
unsigned int aTuneLookBack = 20;
unsigned char ATuneModeRemember = 2;

void readPIDTunings() {
  StaticJsonBuffer<200> jsonBuffer;
  File f = SPIFFS.open("/pidconfig.json", "r");
  char json[200];
  f.readBytes(json, sizeof(json));
  f.close();
  JsonObject &root = jsonBuffer.parseObject(json);
  Kp = root["p"];
  Ki = root["i"];
  Kd = root["d"];
  ramprate = root["ramprate"];
  myPID.SetTunings(Kp, Ki, Kd);

}

void writePIDTunings() {
  StaticJsonBuffer<200> jsonBuffer;
  SPIFFS.remove("/pidconfig.json");
  JsonObject &root = jsonBuffer.createObject();
  root["p"] = myPID.GetKp();
  root["i"] = myPID.GetKi();
  root["d"] = myPID.GetKd();
  root["ramprate"] = ramprate;
  File config = SPIFFS.open("/pidconfig.json", "w");
  root.printTo(config);
  config.close();
}

void updateTemp() {
  temp = DS18B20.getTempCByIndex(0);
  DS18B20.requestTemperatures();
  // overtemperature protection
  if (temp > 100)
    {
    turned_on = false;
    }
}

void incSetpoint() {
  if (targ != setpoint)
    {
    if (targ > setpoint + 0.05)
      {
      targ -= ramprate * 0.008333;
      }
    else if (targ < setpoint - 0.05)
      {
      targ += ramprate * 0.008333;
      }
    else
      {
      targ = setpoint;
      }
    }
}
void pidCompute() {
  if (tuning)
    {
    int val = (aTune.Runtime());
    if (val != 0)
      {
      tuning = false;
      }
    if (!tuning)
      {
      //we're done, set the tuning parameters
      Kp = aTune.GetKp();
      Ki = aTune.GetKi();
      Kd = aTune.GetKd();
      myPID.SetTunings(Kp, Ki, Kd);
      writePIDTunings();
      AutoTuneHelper(false);
      }
    }
  else myPID.Compute();
}

void changeAutoTune() {
  if (!tuning)
    {
    //Set the output to the desired starting frequency.
    Output = aTuneStartValue;
    aTune.SetNoiseBand(aTuneNoise);
    aTune.SetOutputStep(aTuneStep);
    aTune.SetLookbackSec((int) aTuneLookBack);
    AutoTuneHelper(true);
    tuning = true;
    }
  else
    {
    //cancel autotune
    aTune.Cancel();
    tuning = false;
    AutoTuneHelper(false);
    }
}

void AutoTuneHelper(bool start) {
  if (start)
    ATuneModeRemember = myPID.GetMode();
  else
    myPID.SetMode(ATuneModeRemember);
}
