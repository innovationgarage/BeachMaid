/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG // Optional, this enables more detailed prints

#include <SimpleCLI.h>
#include "credentials.h"
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <WiFiMulti.h>
#include <analogWrite.h>

WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
BlynkTimer speed_controller, system_status, remote_receiver;

// Motor pins
struct MotorPins
{
  byte RPWM;
  byte LPWM;
  byte L_R_EN;
};

// PINS
const byte battery_level_pin = 15;
const byte pwm_left_pin = 1, pwm_right_pin = 2;
const MotorPins rightEngine = { 3, 4, 5}, leftEngine = { 6, 7, 8};

int remote_x = 0, remote_y = 0;
unsigned long remote_stop = 0;
unsigned long noMessagesTimeout = 0;
const unsigned long stopAfterNoMessageMs = 4000;
float smoothing = 1, leftbelt_current = 0, rightbelt_current = 0;
int leftbelt_setpoint = 0, rightbelt_setpoint = 0;
boolean stop_all_signals_to_motors = false, offline_mode = false, verbose = false;

void setMotorSpeed(MotorPins motor, int speed, bool reverse = false)
{
  if (stop_all_signals_to_motors)
    return;

  speed *= reverse ? 1 : -1;
  int pwm = max(0, min(1024, (int)map(abs(speed), 0, 100, 0, 1024)));
  bool direction = speed > 0;

  /*Serial.print("MOTOR:");
  Serial.print(motor.RPWM);
  Serial.print("\tSPD:");
  Serial.print(direction ? "+" : "-");
  Serial.print(pwm);
  if (motor.RPWM != 0)
    Serial.println();*/

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
}

void there_is_activity()
{
  noMessagesTimeout = millis() + stopAfterNoMessageMs;
}

void setBothMotorsSpeed(int left, int right)
{
  there_is_activity();
  leftbelt_setpoint = left;
  rightbelt_setpoint = right;
  check_and_set_speed();
}

void setMotorsDirection(int x, int y)
{
  int left, right;
  convertMotorsDirection(x, y, left, right);
  setBothMotorsSpeed(left, right);
}

void convertMotorsDirection(int x, int y, int &left, int &right)
{
  // From http://home.kendra.com/mauser/Joystick.html
  double X = max(-100, min(100, -1 * x));
  double Y = max(-100, min(100, -1 * y));

  double V = (100 - abs(X)) * (Y / 100) + Y;
  double W = (100 - abs(Y)) * (X / 100) + X;

  left = (V + W) / 2;
  right = (V - W) / 2;
}

void stop_all()
{
  float old = smoothing;
  smoothing = 1;
  setBothMotorsSpeed(0, 0);
  smoothing = old;
}

// CLI interface
SimpleCLI cli;
Command moveCmd, beltsCmd, stopCmd, helpCmd, statusCmd, beepCmd, rebootCmd;

// Callback in case of an error
void errorCallback(cmd_error* e)
{
  CommandError cmdError(e); // Create wrapper object

  Serial.print("ERROR: ");
  Serial.println(cmdError.toString());

  if (cmdError.hasCommand()) {
    Serial.print("Did you mean \"");
    Serial.print(cmdError.getCommand().toString());
    Serial.println("\"?");
  }
}

void setMotorPins( MotorPins motor)
{
  pinMode(motor.RPWM, OUTPUT);
  pinMode(motor.LPWM, OUTPUT);
  pinMode(motor.L_R_EN, OUTPUT); // Only using one pin for both
  //pinMode(motor.R_EN, OUTPUT);
}

void check_and_set_speed()
{
  leftbelt_current += (leftbelt_setpoint - leftbelt_current) / smoothing;
  rightbelt_current += (rightbelt_setpoint - rightbelt_current) / smoothing;

  // Just remove some noise
  if (abs(leftbelt_setpoint - leftbelt_current) < 10)
    leftbelt_current = leftbelt_setpoint;

  if (abs(rightbelt_setpoint - rightbelt_current) < 10)
    rightbelt_current = rightbelt_setpoint;

  setMotorSpeed(leftEngine, leftbelt_current, true);
  setMotorSpeed(rightEngine, rightbelt_current, false);

  if (verbose)
  {
    Serial.print("$status=");
    Serial.print(millis());
    Serial.print(",");
    Serial.print(leftbelt_current);
    Serial.print(",");
    Serial.println(rightbelt_current);
  }
}

void loop() {
  if (!offline_mode)
  {
    Blynk.run();

    // Report info back to the remote
    system_status.run();
  }

  // Receiver
  remote_receiver.run();

  // Control the engines
  speed_controller.run();

  if (Serial.available())
  {
    // Read out string from the serial monitor
    String input = Serial.readStringUntil('\n');

    // Parse the user input into the CLI
    cli.parse(input);
  }

  if (cli.errored())
  {
    CommandError cmdError = cli.getError();

    Serial.print("ERROR: ");
    Serial.println(cmdError.toString());

    if (cmdError.hasCommand()) {
      Serial.print("Did you mean \"");
      Serial.print(cmdError.getCommand().toString());
      Serial.println("\"?");
    }
  }

  if ( remote_stop > 0 && millis() > remote_stop)
  {
    remote_stop = 0;
    stop_all();
  }
  else if (millis() > noMessagesTimeout)
  {
    stop_all();

    there_is_activity();
    check_connection();
  }
}
