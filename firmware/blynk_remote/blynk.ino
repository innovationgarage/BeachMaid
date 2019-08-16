BLYNK_WRITE(V1)
{
  setMotorSpeed(leftEngine, param.asInt());
}

BLYNK_WRITE(V2)
{
  setMotorSpeed(rightEngine, param.asInt(), true);
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
  setMotorsDirection(param[0].asInt(), param[1].asInt());
}

BLYNK_WRITE(V3)
{
  play_melody(param.asInt());
}
