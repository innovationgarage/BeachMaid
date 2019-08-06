/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include "credentials.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Motor pins
struct MotorPins
{
  //byte RPWM;
  byte RPWM;
  byte LPWM;
  byte L_R_EN;
};

unsigned long noMessagesTimeout = 0;
const unsigned long stopAfterNoMessageMs = 10000;
const MotorPins leftEngine = { D7, D6, D5}, rightEngine = { D3, D2, D1};

void setMotorPins( MotorPins motor)
{
  pinMode(motor.RPWM, OUTPUT);
  pinMode(motor.LPWM, OUTPUT);
  pinMode(motor.L_R_EN, OUTPUT);
  //pinMode(motor.R_EN, OUTPUT);
}

void setup() {
  // Debug console
  Serial.begin(9600);

  // put your setup code here, to run once:
  Blynk.begin(auth, ssid, pass , "blynk.innovationgarage.no", 80);

  // Set motor pins
  setMotorPins(leftEngine);
  setMotorPins(rightEngine);
}

void setMotorSpeed( MotorPins motor, int speed, bool reverse = false)
{
  Serial.print("Speed: ");
  Serial.println(speed);
  Serial.print("PIN: ");
  Serial.println(motor.RPWM);

  speed *= reverse ? -1 : 1;
  int pwm = map(abs(speed), 0, 100, 0, 1024);
  bool direction = speed > 0;
  noMessagesTimeout = millis() + stopAfterNoMessageMs;

  if (pwm == 0)
  {
    // Electronic break
    digitalWrite(motor.RPWM, LOW);
    digitalWrite(motor.LPWM, LOW);
  }
  else
  {
    digitalWrite(motor.RPWM, direction);
    digitalWrite(motor.LPWM, !direction);
  }

  analogWrite(motor.L_R_EN, pwm);
  //analogWrite(motor.L_EN, pwm);
}

BLYNK_WRITE(V1)
{
  setMotorSpeed(leftEngine, param.asInt());
}

BLYNK_WRITE(V2)
{
  setMotorSpeed(rightEngine, param.asInt(), true);
}

void loop() {
  Blynk.run();

  if (millis() > noMessagesTimeout)
  {
    setMotorSpeed(leftEngine, 0);
    setMotorSpeed(rightEngine, 0);
  }
}
