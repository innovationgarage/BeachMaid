void beep()
{
  setMotorSpeed(rightEngine, 10);
  setMotorSpeed(leftEngine, 10);
  delay(100);
  setMotorSpeed(rightEngine, 0);
  setMotorSpeed(leftEngine, 0);
  delay(400);
}

void play_melody(int m)
{
  switch (m)
  {
    case 0: // No sound
      break;

    case 1: // Alarm
      for (int i = 0; i < 12; i++)
      {
        setMotorSpeed(rightEngine, 0);
        setMotorSpeed(leftEngine, 10);
        delay(10);
        setMotorSpeed(rightEngine, 10);
        setMotorSpeed(leftEngine, 0);
        delay(10);
      }
      break;

    case 2: // Alarm
      for (int i = 0; i < 3; i++)
      {
        setMotorSpeed(rightEngine, 10);
        setMotorSpeed(leftEngine, 10);
        delay(300);
        setMotorSpeed(rightEngine, 0);
        setMotorSpeed(leftEngine, 0);
        delay(100);
        setMotorSpeed(rightEngine, 2);
        setMotorSpeed(leftEngine, 6);
        delay(300);
        setMotorSpeed(rightEngine, 0);
        setMotorSpeed(leftEngine, 0);
        delay(100);
        setMotorSpeed(rightEngine, 6);
        setMotorSpeed(leftEngine, 2);
        delay(300);
        setMotorSpeed(rightEngine, 0);
        setMotorSpeed(leftEngine, 0);
        delay(100);
      }
      break;

    case 3: // Melody
      for (int i = 0; i < 2; i++)
      {
        for (int j = 0; j < 8; j++)
        {
          setMotorSpeed(leftEngine, j);
          delay(100);
          setMotorSpeed(leftEngine, 0);
          delay(100);
        }

        for (int j = 0; j < 8; j++)
        {
          setMotorSpeed(leftEngine, j);
          setMotorSpeed(rightEngine, 0);
          delay(100);
          setMotorSpeed(leftEngine, 0);
          setMotorSpeed(rightEngine, -j);
          delay(100);
        }
      }
      break;

    case 4: // Alarm
      for (int i = 0; i < 3; i++)
        beep();
      break;

    case 5: // Melody 2
      for (int i = 0; i < 2; i++)
      {
        for (int j = -12; j < 12; j++)
        {
          setMotorSpeed(leftEngine, j);
          setMotorSpeed(rightEngine, 0);
          delay(50);
          setMotorSpeed(leftEngine, 0);
          setMotorSpeed(rightEngine, -j);
          delay(50);
          setMotorSpeed(leftEngine, -j);
          setMotorSpeed(rightEngine, j);
          delay(100);
        }
      }
      break;

    case 10: // Boot
      for (int j = 1; j < 10; j++)
      {
        setMotorSpeed(rightEngine, j);
        setMotorSpeed(leftEngine, j);
        delay(50);
      }
      setMotorSpeed(rightEngine, 0);
      setMotorSpeed(leftEngine, 0);
      delay(100);
      beep();
      break;

    default: // Beep
      beep();
      break;
  }

  setMotorSpeed(rightEngine, 0);
  setMotorSpeed(leftEngine, 0);
}
