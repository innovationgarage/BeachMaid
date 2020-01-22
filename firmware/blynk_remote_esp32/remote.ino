// Radio remote control
volatile long pwm_left = -1, pwm_right = -1, pwm_left_prev = 0, pwm_right_prev = 0;

ICACHE_RAM_ATTR void pwm_left_rising()
{
  attachInterrupt(digitalPinToInterrupt(pwm_left_pin), pwm_left_falling, FALLING);
  pwm_left_prev = micros();
}

ICACHE_RAM_ATTR void pwm_left_falling()
{
  attachInterrupt(digitalPinToInterrupt(pwm_left_pin), pwm_left_rising, RISING);

  if (pwm_left_prev > 0)
    pwm_left = micros() - pwm_left_prev;
}

ICACHE_RAM_ATTR void pwm_right_rising()
{
  attachInterrupt(digitalPinToInterrupt(pwm_right_pin), pwm_right_falling, FALLING);
  pwm_right_prev = micros();
}

ICACHE_RAM_ATTR void pwm_right_falling()
{
  attachInterrupt(digitalPinToInterrupt(pwm_right_pin), pwm_right_rising, RISING);

  if (pwm_right_prev > 0)
    pwm_right = micros() - pwm_right_prev;
}

void remote_receive_process()
{
  long left = pwm_left, right = pwm_right;
  if (left > 0 && left < 2000 && right > 0 && right < 2000)
  {
    setMotorsDirection((int)map(right, 1050, 1950, -100, 100), (int)map(left, 1050, 1950, 100, -100));
    there_is_activity();
  }
}
