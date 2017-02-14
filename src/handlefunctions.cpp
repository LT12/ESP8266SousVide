#include"FS.h"
#include "handlefunctions.h"
#include <ArduinoJson.h>


void handleWebsite()
{
    File f = SPIFFS.open("/index.html", "r");
    char buf[1024];
    int siz = f.size();
    while (siz > 0) {
        size_t len = min((int)(sizeof(buf) - 1), siz);
        f.read((uint8_t *)buf, len);
        server.client().write((const char *)buf, len);
        siz -= len;
    }

}

void handleStatus()
{
    StaticJsonBuffer<400> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["status"] = turned_on;
    root["timer"] = timer_status;
    root["tuning"] = tuning;
    root["Kp"] = myPID.GetKp();
    root["Ki"] = myPID.GetKi();
    root["Kd"] = myPID.GetKd();
    size_t len = root.measureLength() + 1;
    char json[len];
    root.printTo(json, len);
    server.send(200, "text/status", json);
}


void handleControl()
{
    int num_args = server.args();
    bool changed_tune = false;
    for (int i = 0; i < num_args; i++) {
        Serial.println(server.argName(i));
        if (server.argName(i) == "on") {
            turned_on = true;
            startMillis = millis();
        } else if (server.argName(i) == "off") {
            turned_on = false;
        } else if (server.argName(i) == "temp") {
            setpoint = server.arg(i).toFloat();
        } else if (server.argName(i) == "timer") {
            timer = server.arg(i).toInt();
            timer_status = true;
            startMillis = millis();
        } else if (server.argName(i) == "P") {
            Kp = server.arg(i).toFloat();
            changed_tune = true;
        } else if (server.argName(i) == "I") {
            Ki = server.arg(i).toFloat();
            changed_tune = true;
        } else if (server.argName(i) == "D") {
            Kd = server.arg(i).toFloat();
            changed_tune = true;
        } else if (server.argName(i) == "autotune") {
            changeAutoTune();
        } else {
        }
    }
    if (changed_tune) {
        myPID.SetTunings(Kp, Ki, Kd);
        writePIDTunings();
    }

    server.send(200, "text/plain", "");
}

void handleData()
{
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["time"] = (millis() - startMillis);
    root["temp"] = temp;
    root["targ"] = targ;
    root["out"] = Output;
    root["timer"] = timer;
    size_t len = root.measureLength() + 1;
    char json[len];
    root.printTo(json, len);
    server.send(200, "text/data", json);
}
