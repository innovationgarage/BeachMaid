float get_battery_voltage()
{
  return map(analogRead(A0), 0, 1024, 0, 18300) / 1000.0;
}

void boot_msg(bool info = false)
{
  Serial.println();
  Serial.print("Innovation Garage AS - ");
  Serial.print(machine_name);
  Serial.print(" Build ");
  Serial.print(__DATE__);
  Serial.print(" ");
  Serial.print(__TIME__);
  Serial.println(offline_mode ? " - OFFLINE MODE" : "");

  if (info)
  {
    Serial.println("Type help for more information");
    Serial.println();
  }
}

void stop_callback(cmd* commandPointer) {
  Serial.print("Sending stop: ");
  stop_all();
  Serial.println("OK");
}

void beep_callback(cmd* commandPointer) {
  Command cmd(commandPointer);
  Argument melody = cmd.getArgument("melody");
  int m = melody.getValue().toInt();
  if (!m)
  {
    Serial.print("Executing beep: ");
    beep();
  }
  else
  {
    Serial.print("Executing melody ");
    Serial.print(m);
    Serial.print(": ");
    play_melody(m);
  }
  Serial.println("OK");
}

void reboot_callback(cmd* commandPointer) {
  Serial.print("Rebooting...");
  stop_all();

  Command cmd(commandPointer);
  if (cmd.getArgument("force").isSet())
    ESP.reset();
  else
    ESP.restart();
}

void status_callback(cmd* commandPointer) {
  Command cmd(commandPointer);

  bool enabled = cmd.getArgument("stream").isSet();
  bool disabled = cmd.getArgument("quiet").isSet();

  if (enabled && !disabled)
    verbose = true;
  else
  {
    if (disabled && !enabled)
      verbose = false;
    else
    {
      Serial.print("$battery=");
      Serial.println(get_battery_voltage());
      Serial.print("$leftbelt_setpoint=");
      Serial.println(leftbelt_setpoint);
      Serial.print("$rightbelt_setpoint=");
      Serial.println(rightbelt_setpoint);
      Serial.print("$leftbelt_current=");
      Serial.println(leftbelt_current);
      Serial.print("$rightbelt_current=");
      Serial.println(rightbelt_current);
      Serial.print("$smoothing=");
      Serial.println(smoothing);
      Serial.print("$verbose=");
      Serial.println(verbose);
    }
  }
}

void help_callback(cmd* commandPointer) {
  boot_msg();
  Serial.println("Available commands: ");
  Serial.println(cli.toString());
}

void move_callback(cmd* commandPointer) {
  Command cmd(commandPointer); // Create wrapper class instance for the pointer
  Argument x = cmd.getArgument("x");
  Argument y = cmd.getArgument("y");
  Argument smoothing = cmd.getArgument("smoothing");
  setSmoothing(smoothing.getValue().toInt());

  Serial.print("Sending move to motors: ");
  setMotorsDirection(x.getValue().toInt(), y.getValue().toInt());
  Serial.println("OK");
}

void belts_callback(cmd* commandPointer) {
  Command cmd(commandPointer); // Create wrapper class instance for the pointer
  Argument left = cmd.getArgument("left");
  Argument right = cmd.getArgument("right");
  Argument smoothing = cmd.getArgument("smoothing");
  setSmoothing(smoothing.getValue().toInt());

  Serial.print("Sending belt speed to motors: ");
  setBothMotorsSpeed(left.getValue().toInt(), right.getValue().toInt());
  Serial.println("OK");
}
