// From: https://github.com/rotarucosminleonard/Blynk-ESP8266_OfflineTask_Reconnect/blob/master/Blynk-ESP8266_OfflineTask_Reconnect.ino

void check_connection() {   // check every 11s if connected to Blynk server
  if (offline_mode)
    return;

  int online = 1;
  if (!Blynk.connected()) {
    online = 0;
    yield();
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("Not connected to Wifi! Connect...");
      //Blynk.connectWiFi(ssid, pass); // used with Blynk.connect() in place of Blynk.begin(auth, ssid, pass, server, port);

      int attempts = 30;
      while (wifiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
        if (attempts-- <= 0)
          break;
      }

      if (wifiMulti.run() != WL_CONNECTED)
      {
        Serial.println("Cannot connect to WIFI!");
        online = 0;
      }
      else
      {
        Serial.print("Connected to ");
        Serial.println(WiFi.SSID());              // Tell us what network we're connected to
        Serial.print("IP address:\t");
        Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to the computer
      }
    }

    if ( WiFi.status() == WL_CONNECTED && !Blynk.connected() )
    {
      Serial.println("Not connected to Blynk Server! Connecting...");
      Blynk.connect();  // // It has 3 attempts of the defined BLYNK_TIMEOUT_MS to connect to the server, otherwise it goes to the enxt line
      if (!Blynk.connected()) {
        Serial.println("Connection failed!");
        online = 0;
      }
      else
      {
        online = 1;
      }
    }
  }
  else
    online = 1;

  if (!online)
    play_melody(2); // Error
}
