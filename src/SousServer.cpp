#include"FS.h"
#include "SousServer.h"
#include "handlefunctions.h"
#include "temperaturecontrol.h"
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <Arduino.h>

// set up over the air updating
// be sure to manually reset ESP8266
// after uploading sketch over USB
void setupOTA() {
  ArduinoOTA.setPort(8266); // default port
  // add security through password
  ArduinoOTA.setHostname(");
  ArduinoOTA.setPassword("");
  ArduinoOTA.onStart([]()
                       {
                       String type;
                       if (ArduinoOTA.getCommand() == U_FLASH)
                         type = "sketch";
                       else // U_SPIFFS
                         type = "filesystem";

                       SPIFFS.end(); // end filesystem in case it was open during OTA
                       Serial.println("Start updating " + type);
                       });
  ArduinoOTA.onEnd([]()
                     {
                     Serial.println("\nEnd");
                     });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                          {
                          Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
                          });
  ArduinoOTA.onError([](ota_error_t error)
                       {
                       Serial.printf("Error[%u]: ", error);
                       if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
                       else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
                       else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
                       else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
                       else if (error == OTA_END_ERROR) Serial.println("End Failed");
                       });
  ArduinoOTA.begin();
}

void setup() {
  Serial.begin(115200);
  bool ok = SPIFFS.begin(); // start filesystem
  WiFi.begin(ssid, password); // connect to WIFI
  while (WiFi.status() != WL_CONNECTED)delay(500);
  WiFi.mode(WIFI_STA);
  //WiFi.mode(WIFI_AP);
  //WiFi.softAP("EspSousVide");
  //Serial.print("Station IP address: ");
  //Serial.println(WiFi.localIP());
  setupOTA();
  readPIDTunings();
  // give callback functions to web server
  server.on("/", handleWebsite);
  server.on("/data", handleData);
  server.on("/status", handleStatus);
  server.on("/control", handleControl);
  server.begin(); // start webserver

  DS18B20.begin(); // start 1 wire temperature sensor
  DS18B20.setResolution(11); // set temperature resolution (9-12)
  DS18B20.setWaitForConversion(false); // allows async temperature acquisition
  DS18B20.requestTemperatures(); // start temperature acquisition
  analogWriteRange(4095); // set PWM resolution to 12 bits. requires low PWM freq.
  analogWriteFreq(2); // set PWM frequency to 2 HZ, not sure if this works
  myPID.SetOutputLimits(0, 4095);
  temp_timer = ftimer.setInterval(500, updateTemp); //update temperature every 1.5 s
  targ_timer = ftimer.setInterval(500, incSetpoint); // ramp setpoint gradually
  myPID.SetMode(AUTOMATIC); // start PID
}

void loop() {
  server.handleClient();
  ArduinoOTA.handle();
  if (turned_on)
    {

    if (myPID.GetMode() == MANUAL)
      {
      myPID.SetMode(AUTOMATIC);
      }
    if (!ftimer.isEnabled(temp_timer))
      {
      ftimer.enable(temp_timer);
      ftimer.enable(targ_timer);

      }
    ftimer.run();
    pidCompute();
    // check if Sous Vide timer has completed
    if (timer_status)
      {
      if ((millis() - startMillis) >= timer)
        {
        // turn off system if timer is complete
        turned_on = false;
        timer_status = false;
        Output = 0;
        }
      }
    }
  else
    {
    Output = 0;
    targ = 20;
    timer_status = false;
    tuning = true;
    changeAutoTune();
    ftimer.disable(temp_timer);
    ftimer.disable(targ_timer);
    myPID.SetMode(MANUAL);
    }

  analogWrite(D1, Output);
}
