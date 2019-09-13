void setup() {
  Serial.begin(115200);
  cli.setOnError(errorCallback); // Set error Callback
  stopCmd = cli.addCmd("stop", stop_callback);
  beepCmd = cli.addCmd("beep", beep_callback);
  rebootCmd = cli.addCmd("reboot", reboot_callback);
  helpCmd = cli.addCmd("help", help_callback);
  statusCmd = cli.addCmd("status", status_callback);
  
  moveCmd = cli.addCmd("move", move_callback);
  moveCmd.addPosArg("x");
  moveCmd.addPosArg("y");
  moveCmd.addPosArg("smoothing", "1");
  beltsCmd = cli.addCmd("belts", belts_callback);
  beltsCmd.addPosArg("left");
  beltsCmd.addPosArg("right");
  beltsCmd.addPosArg("smoothing", "1");
  rebootCmd.addFlagArg("force");
  beepCmd.addPosArg("melody", "0");

  beep();

  wifiMulti.addAP(ssid1, pass1);
  wifiMulti.addAP(ssid2, pass2);
  wifiMulti.addAP(ssid3, pass3);

  WiFi.hostname(machine_name);
  WiFi.mode(WIFI_STA);

  Blynk.config(auth, server, server_port);  // I am using the local Server
  check_connection();// It needs to run first to initiate the connection. Same function works for checking the connection!

  speed_controller.setInterval(100L, check_and_set_speed);
  system_status.setInterval(1000L, report_status);

  // Set motor pins
  setMotorPins(leftEngine);
  setMotorPins(rightEngine);

  if(Blynk.connected())
  play_melody(10); // Boot
  else
  {
    offline_mode = true;
    play_melody(11);
  }
  boot_msg(true);
}
