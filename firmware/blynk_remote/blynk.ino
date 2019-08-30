void report_status()
{
  Blynk.virtualWrite(V40, (abs(leftbelt_current)+abs(rightbelt_current))/2);
  Blynk.virtualWrite(V41, (abs(leftbelt_setpoint)+abs(rightbelt_setpoint))/2);
  Blynk.virtualWrite(V42, get_battery_voltage());
}

void setSmoothing(int s)
{
  smoothing = min(1000.0, max(1.0, s / 100.0));
}

BLYNK_WRITE(V9) // Smoothing
{
  int s = param.asInt();

  if(s>0)
    setSmoothing(s);
}

BLYNK_WRITE(V1)
{
  //setBothMotorsSpeed(param.asInt(), rightbelt_current);
  leftbelt_setpoint=param.asInt();
}

BLYNK_WRITE(V2)
{
  //setBothMotorsSpeed(leftbelt_current, param.asInt());
  rightbelt_setpoint=param.asInt();
}

BLYNK_WRITE(V11) // X speed
{
  remote_x = max(-100, min(100, param.asInt()));
}
BLYNK_WRITE(V12) // Y speed
{
  remote_y = max(-100, min(100, param.asInt()));
}

BLYNK_WRITE(V13) // time delay and execute
{
  int running_time = min(10000, param.asInt());

  if (running_time > 0)
  {
    setMotorsDirection(remote_x, remote_y);
    remote_stop = millis() + running_time;
  }
}

// Basic (Joystick)
BLYNK_WRITE(V10)
{
  //setMotorsDirection(-param[0].asInt(), -param[1].asInt());
  convertMotorsDirection(-param[0].asInt(), -param[1].asInt(), leftbelt_setpoint, rightbelt_setpoint);
}

BLYNK_WRITE(V4) // Stop
{
  if (param.asInt() == 1)
    stop_all();
}

BLYNK_WRITE(V3)
{
  play_melody(param.asInt());
}
