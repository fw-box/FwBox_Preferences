//
// Copyright (c) 2022 Fw-Box (https://fw-box.com)
// Author: Hartman Hsieh
//
// Description :
//
// Libraries :
//

#if defined(ESP32)
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif

#include "FwBox_Preferences.h"

String strcmd = "";

void setup() {
  Serial.begin(115200);

  delay(3000);

  String wifi_ssid = "";
  String wifi_pass = "";
  int i_test = 0;

  //
  // Read the settings from SPIFFS
  //
  FwBox_Preferences prefs;
  prefs.begin("WIFI");
  wifi_ssid = prefs.getString("WIFI_SSID");
  wifi_pass = prefs.getString("WIFI_PW");
  i_test = prefs.getInt("INT_TEST");
  prefs.end();

  Serial.printf("WIFI_SSID : %s\n", wifi_ssid.c_str());
  Serial.printf("WIFI_PW : %s\n", wifi_pass.c_str());
  Serial.printf("INT_TEST : %d\n", i_test);

  if (wifi_ssid.length() > 0) {
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(wifi_ssid);
  
    /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
       would try to act as both a client and an access-point and could cause
       network-issues with your other WiFi-devices on your WiFi-network. */
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());

    for (int ii = 0; ii < 30; ii++) {
      if (WiFi.status() == WL_CONNECTED)
        break;
      delay(500);
      Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }

  //
  // Save integer to SPIFFS
  //
  prefs.begin("WIFI");
  prefs.putInt("INT_TEST", millis()); // Save a value
  prefs.end();

  Serial.println("Key in 'SSID:your_ssid' and press 'Enter' to save WiFi SSID.");
  Serial.println("Key in 'PASS:your_password' and press 'Enter' to Save WiFi password.");
}

void loop() {
  String wifi_ssid = "";
  String wifi_pass = "";

  //
  // Read command from Serial
  //
  while (Serial.available() > 0) {
    char cc = Serial.read();
    if (cc == '\n') { // User press 'Enter' key
      Serial.println(strcmd);
      if (strcmd.indexOf("SSID:") == 0) { // Receive input "SSID:XXX"
        wifi_ssid = strcmd.substring(5);
        if (wifi_ssid.length() > 0) {
          Serial.println("Save to SPIFFS");
          FwBox_Preferences prefs;
          prefs.begin("WIFI");
          prefs.putString("WIFI_SSID", wifi_ssid);
          prefs.end();
        }
      }
      else if (strcmd.indexOf("PASS:") == 0) { // Receive input "PASS:XXX"
        wifi_pass = strcmd.substring(5);
        if (wifi_pass.length() > 0) {
          Serial.println("Save to SPIFFS");
          FwBox_Preferences prefs;
          prefs.begin("WIFI");
          prefs.putString("WIFI_PW", wifi_pass);
          prefs.end();
        }
      }
      strcmd = "";
    }
    else {
      strcmd += cc;
    }
  }
}
