# How to read the config from SPIFFS.
    FwBox_Preferences prefs;
    prefs.begin("WIFI");
    String wifi_ssid = prefs.getString("WIFI_SSID");
    String wifi_pass = prefs.getString("WIFI_PW");
    int i_test = prefs.getInt("INT_TEST");
    prefs.end();

# How to save the config to SPIFFS
    FwBox_Preferences prefs;
    prefs.begin("WIFI");
    prefs.putString("WIFI_SSID", "YOUR_SSID");
    prefs.putString("WIFI_PW", "YOUR_PASSWORD");
    prefs.putInt("INT_TEST", 72); // Save a value
    prefs.end();
