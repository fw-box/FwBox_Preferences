//
// Copyright (c) 2022 Fw-Box (https://fw-box.com)
// Author: Hartman Hsieh
//
// Description :
//   
//
// Connections :
//   
//
// Required Library :
//

#ifndef __FWBOX_PREFERENCES_H__
#define __FWBOX_PREFERENCES_H__

#include <Arduino.h>
#if defined(ESP32)
  #include <Preferences.h>
#else
  #include <LittleFS.h>
#endif

//
// Debug definitions
//
#define FWBOX_PREFERENCES_DEBUG 0

#if FWBOX_PREFERENCES_DEBUG == 1
  #define FWBOX_PREFERENCES_DEBUG_PRINT(VAL) Serial.print(VAL)
  #define FWBOX_PREFERENCES_DEBUG_PRINTLN(VAL) Serial.println(VAL)
  #define FWBOX_PREFERENCES_DEBUG_PRINTF(FORMAT, ARG) Serial.printf(FORMAT, ARG)
  #define FWBOX_PREFERENCES_DEBUG_PRINTF2(FORMAT, ARG1, ARG2) Serial.printf(FORMAT, ARG1, ARG2)
#else
  #define FWBOX_PREFERENCES_DEBUG_PRINT(VAL)
  #define FWBOX_PREFERENCES_DEBUG_PRINTLN(VAL)
  #define FWBOX_PREFERENCES_DEBUG_PRINTF(FORMAT, ARG)
  #define FWBOX_PREFERENCES_DEBUG_PRINTF2(FORMAT, ARG1, ARG2)
#endif

#if defined(ESP32)
#else
  class KeyValue
  {
  public:
    String Key = "";
    String Value = "";
  };
#endif

class FwBox_Preferences
{
public:
    FwBox_Preferences();

    bool begin(const char * name);
    void end();

    bool clear();
    bool remove(const char * key);

    size_t putInt(const char* key, int32_t value);
    size_t putString(const char* key, const char* value);
    size_t putString(const char* key, String value);

    int32_t getInt(const char* key, int32_t defaultValue = 0);
    String getString(const char* key, String defaultValue = String());

    void dumpPairList();

private:
#if defined(ESP32)
    Preferences Prefs;
#else
    String PairListFileName = "";
    KeyValue PairList[20];
    int PairCount = 0;
    void readPairListFromFile();
    void savePairListToFile();
    String readFile(const char * path);
    void writeFile(const char * path, const char * message);
    String* getValueFromPairList(const char* key);
    String* getValueFromPairList(String key);
    String encodeStr(String str);
    String decodeStr(String str);
#endif
};

#endif // __FWBOX_PREFERENCES_H__
