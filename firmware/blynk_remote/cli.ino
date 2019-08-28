void boot_msg(bool info=false)
{
  Serial.println();
  Serial.print("Innovation Garage AS - ");
  Serial.print(machine_name);
  Serial.print(" Build ");
  Serial.print(__DATE__);
  Serial.print(" ");
  Serial.println(__TIME__);

  if(info)
    Serial.println("Type help for more information");
  Serial.println();
}

void stop_callback(cmd* commandPointer) {
  Serial.print("Sending stop: ");
  stop_all();
  Serial.println("OK");
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
  //setMotorSpeed(leftEngine, left.getValue().toInt());
  //setMotorSpeed(rightEngine, right.getValue().toInt(), true);
  Serial.println("OK");
}
