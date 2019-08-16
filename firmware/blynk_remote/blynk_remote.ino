/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <SimpleCLI.h>
#include "credentials.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
//BlynkTimer timer;

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
const unsigned long stopAfterNoMessageMs = 10000;
const MotorPins leftEngine = { D7, D6, D5}, rightEngine = { D3, D2, D1};

void setMotorSpeed( MotorPins motor, int speed, bool reverse = false)
{
  /*Serial.print("Speed: ");
  Serial.println(speed);
  Serial.print("PIN: ");
  Serial.println(motor.RPWM);*/

  speed *= reverse ? -1 : 1;
  int pwm = max(0, min(1024, (int)map(abs(speed), 0, 100, 0, 1024)));
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


void setMotorsDirection(int x, int y)
{
  // From http://home.kendra.com/mauser/Joystick.html
  int X = max(-100, min(100, -1 * x));
  int Y = max(-100, min(100, -1 * y));

  double V = (100 - abs(X)) * (Y / 100) + Y;
  double W = (100 - abs(Y)) * (X / 100) + X;

  setMotorSpeed(leftEngine, (V + W) / 2, true);
  setMotorSpeed(rightEngine, (V - W) / 2, false);

  /*Serial.print("motor left speed");
    Serial.println((V + W) / 2);
    Serial.print("motor right speed");
    Serial.println((V - W) / 2);*/
}

void stop_all()
{
  setMotorSpeed(leftEngine, 0);
  setMotorSpeed(rightEngine, 0);
}

// CLI interface
SimpleCLI cli;
Command move, belts, stop, help;

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

void boot_msg()
{
  Serial.print("Innovation Garage AS - ");
  Serial.print(machine_name);
  Serial.print(" Build ");
  Serial.print(__DATE__);
  Serial.print(" ");
  Serial.println(__TIME__);
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
  Serial.print("Sending move to motors: ");
  setMotorsDirection(x.getValue().toInt(), y.getValue().toInt());
  Serial.println("OK");
}

void belts_callback(cmd* commandPointer) {
  Command cmd(commandPointer); // Create wrapper class instance for the pointer
  Argument left = cmd.getArgument("left");
  Argument right = cmd.getArgument("right");
  Serial.print("Sending belt speed to motors: ");
  setMotorSpeed(leftEngine, left.getValue().toInt());
  setMotorSpeed(rightEngine, right.getValue().toInt(), true);
  Serial.println("OK");
}

void setup() {
  Serial.begin(74880);
  cli.setOnError(errorCallback); // Set error Callback
  stop = cli.addCmd("stop", stop_callback);
  help = cli.addCmd("help", help_callback);
  move = cli.addCmd("move", move_callback);
  move.addPosArg("x");
  move.addPosArg("y");
  belts = cli.addCmd("belts", belts_callback);
  belts.addPosArg("left");
  belts.addPosArg("right");
  
  beep();
  boot_msg();

  wifiMulti.addAP(ssid1, pass1);
  wifiMulti.addAP(ssid2, pass2);
  wifiMulti.addAP(ssid3, pass3);

  WiFi.hostname(machine_name);
  WiFi.mode(WIFI_STA);

  Blynk.config(auth, server, server_port);  // I am using the local Server
  check_connection();// It needs to run first to initiate the connection.Same function works for checking the connection!
  //timer.setInterval(5000L, check_connection);

  // Set motor pins
  setMotorPins(leftEngine);
  setMotorPins(rightEngine);
  play_melody(10); // Boot
}

void loop() {
  Blynk.run();

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
