/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG // Optional, this enables more detailed prints

#include <SimpleCLI.h>
#include "credentials.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
BlynkTimer speed_controller, system_status;

// Motor pins
struct MotorPins
{
  //byte RPWM;
  byte RPWM;
  byte LPWM;
  byte L_R_EN;
};

int remote_x = 0, remote_y = 0;
unsigned long remote_stop = 0;
unsigned long noMessagesTimeout = 0;
const unsigned long stopAfterNoMessageMs = 2000;
const MotorPins leftEngine = { D7, D6, D5}, rightEngine = { D3, D2, D1};
float smoothing = 1, leftbelt_current = 0, rightbelt_current = 0;
int leftbelt_setpoint = 0, rightbelt_setpoint = 0;
boolean stop_all_signals_to_motors = false, offline_mode = false;

void setMotorSpeed( MotorPins motor, int speed, bool reverse = false)
{
  if (stop_all_signals_to_motors)
    return;

  speed *= reverse ? 1 : -1;
  int pwm = max(0, min(1024, (int)map(abs(speed), 0, 100, 0, 1024)));
  bool direction = speed > 0;

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

void setBothMotorsSpeed(int left, int right)
{
  noMessagesTimeout = millis() + stopAfterNoMessageMs;
  leftbelt_setpoint = left;
  rightbelt_setpoint = right;
  check_and_set_speed();
}

int setMotorsDirection(int x, int y)
{
  int left, right;
  convertMotorsDirection(x, y, left, right);
  setBothMotorsSpeed(left, right);
}

void convertMotorsDirection(int x, int y, int &left, int &right)
{
  // From http://home.kendra.com/mauser/Joystick.html
  int X = max(-100, min(100, -1 * x));
  int Y = max(-100, min(100, -1 * y));

  float V = (100 - abs(X)) * (Y / 100) + Y;
  float W = (100 - abs(Y)) * (X / 100) + X;

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
Command move, belts, stop, help, status;

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
  pinMode(motor.L_R_EN, OUTPUT);
  //pinMode(motor.R_EN, OUTPUT);
}


void check_and_set_speed()
{
  leftbelt_current += (leftbelt_setpoint - leftbelt_current) / smoothing;
  rightbelt_current += (rightbelt_setpoint - rightbelt_current) / smoothing;
  //leftbelt_current = leftbelt_setpoint;
  //rightbelt_current = rightbelt_setpoint;


  // Just remove some noise
  if (abs(leftbelt_setpoint - leftbelt_current) < 10)
    leftbelt_current = leftbelt_setpoint;

  if (abs(rightbelt_setpoint - rightbelt_current) < 10)
    rightbelt_current = rightbelt_setpoint;

  setMotorSpeed(leftEngine, leftbelt_current, true);
  setMotorSpeed(rightEngine, rightbelt_current, false);
}

void loop() {
  if (!offline_mode)
  {
    Blynk.run();

    // Report info back to the remote
    system_status.run();
  }

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
    check_connection();
  }
}
