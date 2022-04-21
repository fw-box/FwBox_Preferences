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

#include "FwBox_Preferences.h"

FwBox_Preferences::FwBox_Preferences()
{

}

bool FwBox_Preferences::begin(const char * name)
{
#if defined(ESP32)
    return FwBox_Preferences::Prefs.begin(name);
#else
    FWBOX_PREFERENCES_DEBUG_PRINTF("FwBox_Preferences : name = %s\n", name);

    if (strlen(name) > 16)
        return false;

    //
    // Generate the file name.
    //
    FwBox_Preferences::PairListFileName = "/_FwBox_Pref_";
    FwBox_Preferences::PairListFileName += name;
    FwBox_Preferences::PairListFileName += ".cfg";

    //LittleFSConfig cfg;

    /*if (LittleFS.format()) {
        FWBOX_PREFERENCES_DEBUG_PRINTLN("FwBox_Preferences : LittleFS.format true");
    }
    else {
        FWBOX_PREFERENCES_DEBUG_PRINTLN("FwBox_Preferences : LittleFS.format false");
    }*/

    if (LittleFS.begin()) {
        FWBOX_PREFERENCES_DEBUG_PRINTLN("FwBox_Preferences : LittleFS.begin true");
    }
    else {
        FWBOX_PREFERENCES_DEBUG_PRINTLN("FwBox_Preferences : LittleFS.begin false");
    }

    return true;
#endif
}

void FwBox_Preferences::end()
{
#if defined(ESP32)
    FwBox_Preferences::Prefs.end();
#else
    LittleFS.end();
#endif
}

bool FwBox_Preferences::clear()
{
#if defined(ESP32)
    return FwBox_Preferences::Prefs.clear();
#else
    FwBox_Preferences::PairCount = 0;
    FwBox_Preferences::savePairListToFile();
    return true;
#endif
}

bool FwBox_Preferences::remove(const char * key)
{
    if (strlen(key) > 0) {
#if defined(ESP32)
        return FwBox_Preferences::Prefs.remove(key);
#else
        FwBox_Preferences::readPairListFromFile();
        int p_count = FwBox_Preferences::PairCount;
        bool start_to_shift = false;
        for (int pi = 0; pi < p_count; pi++) {
            if (strcmp(key, FwBox_Preferences::PairList[pi].Key.c_str()) == 0) {
                start_to_shift = true;
            }
            if (start_to_shift == true) {
                if (pi == (p_count - 1)) { // The last one
                    
                }
                else {
                    FwBox_Preferences::PairList[pi].Key = FwBox_Preferences::PairList[pi+1].Key;
                    FwBox_Preferences::PairList[pi].Value = FwBox_Preferences::PairList[pi+1].Value;
                }
            }
            //Serial.printf("%s,%s\n", FwBox_Preferences::PairList[pi].Key.c_str(), FwBox_Preferences::PairList[pi].Value.c_str());
        }
        if (start_to_shift == true) {
            FwBox_Preferences::PairCount--;
        }
        FwBox_Preferences::savePairListToFile();
        return start_to_shift;
#endif
    }
}

size_t FwBox_Preferences::putInt(const char* key, int32_t value)
{
    if (strlen(key) > 0) {
#if defined(ESP32)
        return FwBox_Preferences::Prefs.putInt(key, value);
#else
        return FwBox_Preferences::putString(key, String(value));
#endif
    }
    return 0;
}

size_t FwBox_Preferences::putString(const char* key, const char* value)
{
    if (strlen(key) > 0) {
#if defined(ESP32)
        return FwBox_Preferences::Prefs.putString(key, value);
#else
        FwBox_Preferences::readPairListFromFile();
        String* val = FwBox_Preferences::getValueFromPairList(key);
        if (val == 0) {
            //
            // The key is not found, add a new one.
            //
            FwBox_Preferences::PairList[FwBox_Preferences::PairCount].Key = key;
            FwBox_Preferences::PairList[FwBox_Preferences::PairCount].Value = value;
            FwBox_Preferences::PairCount++;
            FwBox_Preferences::savePairListToFile();
            FWBOX_PREFERENCES_DEBUG_PRINTF("putString A PairCount = %d\n", FwBox_Preferences::PairCount);
            return strlen(value);
        }
        else {
            //
            // Found the key-value and save it's value.
            //
            FWBOX_PREFERENCES_DEBUG_PRINTF("putString B CURRENT VALUE = %s\n", val->c_str());
            *val = value;
            FWBOX_PREFERENCES_DEBUG_PRINTF("putString B NEW VALUE = %s\n", val->c_str());
            FwBox_Preferences::savePairListToFile();
            FWBOX_PREFERENCES_DEBUG_PRINTF("putString B PairCount = %d\n", FwBox_Preferences::PairCount);
            return strlen(value);
        }
#endif
    }
    return 0;
}

size_t FwBox_Preferences::putString(const char* key, String value)
{
    return FwBox_Preferences::putString(key, value.c_str());
}

int32_t FwBox_Preferences::getInt(const char* key, int32_t defaultValue)
{
    if (strlen(key) > 0) {
#if defined(ESP32)
    return FwBox_Preferences::Prefs.getInt(key, defaultValue);
#else
    String temp = FwBox_Preferences::getString(key, String(defaultValue));
    return temp.toInt();
#endif
    }
    return defaultValue;
}

String FwBox_Preferences::getString(const char* key, String defaultValue)
{
    if (strlen(key) > 0) {
#if defined(ESP32)
        return FwBox_Preferences::Prefs.getString(key, defaultValue);
#else
        FwBox_Preferences::readPairListFromFile();
        String* val = FwBox_Preferences::getValueFromPairList(key);
        if (val != 0) {
            //
            // Found the key-value
            //
            FWBOX_PREFERENCES_DEBUG_PRINTF("getString VALUE = %s\n", val->c_str());
            return *val;
        }
#endif
    }
    return defaultValue;
}

void FwBox_Preferences::dumpPairList()
{
    FwBox_Preferences::readPairListFromFile();
    int p_count = FwBox_Preferences::PairCount;
    //Serial.printf("FwBox_Preferences::PairCount = %d\n", p_count);
    //Serial.println("===PairList===");
    for (int pi = 0; pi < p_count; pi++) {
        Serial.printf("%s,%s\n", FwBox_Preferences::PairList[pi].Key.c_str(), FwBox_Preferences::PairList[pi].Value.c_str());
    }
}

void FwBox_Preferences::readPairListFromFile()
{
    FWBOX_PREFERENCES_DEBUG_PRINTF("readPairListFromFile PairListFileName = %s\n", FwBox_Preferences::PairListFileName.c_str());

    String content = FwBox_Preferences::readFile(FwBox_Preferences::PairListFileName.c_str());
    //Serial.println("===CONTENT===");
    //Serial.println(content);
    int clen = content.length();

    FwBox_Preferences::PairCount = 0;
    String token = "";
    String str_key = "";
    for (int ci = 0; ci < clen; ci++) {
        char cc = content[ci];
        if (cc == '=') {
            str_key = decodeStr(token);
            token = "";
        }
        else if (cc == '\n') {
            String* str_temp = FwBox_Preferences::getValueFromPairList(str_key);
            if (str_temp == 0) { // '0' means the key is not found, Keep the key unique
                FwBox_Preferences::PairList[FwBox_Preferences::PairCount].Key = str_key;
                FwBox_Preferences::PairList[FwBox_Preferences::PairCount].Value = decodeStr(token);
                FwBox_Preferences::PairCount++;
            }
            str_key = "";
            token = "";
        }
        else {
            token += cc;
        }
    }

    if (str_key.length() > 0) {
        String* str_temp = FwBox_Preferences::getValueFromPairList(str_key);
        if (str_temp == 0) { // '0' means the key is not found, Keep the key unique
            FwBox_Preferences::PairList[FwBox_Preferences::PairCount].Key = str_key;
            FwBox_Preferences::PairList[FwBox_Preferences::PairCount].Value = decodeStr(token);
            FwBox_Preferences::PairCount++;
        }
    }
}

void FwBox_Preferences::savePairListToFile()
{
    FWBOX_PREFERENCES_DEBUG_PRINTF("savePairListToFile PairListFileName = %s\n", FwBox_Preferences::PairListFileName.c_str());
    String content = "";
    for (int pi = 0; pi < FwBox_Preferences::PairCount; pi++) {
        content = content + encodeStr(FwBox_Preferences::PairList[pi].Key) + "=" + encodeStr(FwBox_Preferences::PairList[pi].Value);
        if (pi != (FwBox_Preferences::PairCount - 1))
            content = content + "\n";
    }
    FwBox_Preferences::writeFile(FwBox_Preferences::PairListFileName.c_str(), content.c_str());
}

String FwBox_Preferences::readFile(const char * path)
{
    FWBOX_PREFERENCES_DEBUG_PRINTF("Reading file: %s\n", path);
    String temp = "";

    File file = LittleFS.open(path, "r");
    if (!file) {
    FWBOX_PREFERENCES_DEBUG_PRINTLN("Failed to open file for reading");
        return "";
    }

    FWBOX_PREFERENCES_DEBUG_PRINT("Read from file: ");
    while (file.available()) {
        char cc = file.read();
        temp += cc;
        //Serial.write(cc);
    }
    file.close();
    //FWBOX_PREFERENCES_DEBUG_PRINTLN();
    return temp;
}

void FwBox_Preferences::writeFile(const char * path, const char * message)
{
  FWBOX_PREFERENCES_DEBUG_PRINTF("Writing file: %s\n", path);

  File file = LittleFS.open(path, "w");
  if (!file) {
    FWBOX_PREFERENCES_DEBUG_PRINTLN("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    FWBOX_PREFERENCES_DEBUG_PRINTLN("File written");
  } else {
    FWBOX_PREFERENCES_DEBUG_PRINTLN("Write failed");
  }
  file.close();
}

String* FwBox_Preferences::getValueFromPairList(const char* key)
{
    FWBOX_PREFERENCES_DEBUG_PRINTF("key:%s\n", key);
    for (int pi = 0; pi < FwBox_Preferences::PairCount; pi++) {
        FWBOX_PREFERENCES_DEBUG_PRINTF2("PairList[%d].Key:%s\n", pi, FwBox_Preferences::PairList[pi].Key.c_str());
        if (strcmp(key, FwBox_Preferences::PairList[pi].Key.c_str()) == 0) { // equals
            return &(FwBox_Preferences::PairList[pi].Value);
        }
    }
    return 0;
}

String* FwBox_Preferences::getValueFromPairList(String key)
{
    return FwBox_Preferences::getValueFromPairList(key.c_str());
}

String FwBox_Preferences::encodeStr(String str)
{
    String temp = str;
    temp.replace("%", "%25");
    temp.replace("=", "%3D");
    temp.replace("\n", "%0A");
    return temp;
}

String FwBox_Preferences::decodeStr(String str)
{
    String temp = str;
    temp.replace("%3D", "=");
    temp.replace("%0A", "\n");
    temp.replace("%25", "%");
    return temp;
}
