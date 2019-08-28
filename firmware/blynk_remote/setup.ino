void setup() {
  Serial.begin(115200);
  cli.setOnError(errorCallback); // Set error Callback
  stop = cli.addCmd("stop", stop_callback);
  help = cli.addCmd("help", help_callback);
  status = cli.addCmd("status", status_callback);
  
  move = cli.addCmd("move", move_callback);
  move.addPosArg("x");
  move.addPosArg("y");
  move.addPosArg("smoothing", "1");
  belts = cli.addCmd("belts", belts_callback);
  belts.addPosArg("left");
  belts.addPosArg("right");
  belts.addPosArg("smoothing", "1");

  beep();

  wifiMulti.addAP(ssid1, pass1);
  wifiMulti.addAP(ssid2, pass2);
  wifiMulti.addAP(ssid3, pass3);

  WiFi.hostname(machine_name);
  WiFi.mode(WIFI_STA);

  Blynk.config(auth, server, server_port);  // I am using the local Server
  check_connection();// It needs to run first to initiate the connection.Same function works for checking the connection!

  speed_controller.setInterval(100L, check_and_set_speed);
  system_status.setInterval(1000L, report_status);

  // Set motor pins
  setMotorPins(leftEngine);
  setMotorPins(rightEngine);
  play_melody(10); // Boot
  boot_msg(true);
}
