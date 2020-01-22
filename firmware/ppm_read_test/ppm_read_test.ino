void ICACHE_RAM_ATTR read_me ();
const byte interruptPin = D2;
const byte totalChannels = 10;
unsigned long int a, b, c;
int x[15], ch1[15], ch[totalChannels + 1], i;
//specifing arrays and variables to store values

void setup() {
  Serial.begin(115200);
  pinMode(interruptPin, INPUT_PULLUP);          //use pin 2 or pin 3 only
  attachInterrupt(digitalPinToInterrupt(interruptPin), read_me, FALLING);
  // enabling interrupt at pin 2

  Serial.println("Start");
}

void loop() {
  read_rc(); // use the fuction every time before reading channels
  Serial.print(ch[1]); Serial.print("\t");
  Serial.print(ch[2]); Serial.print("\t");
  Serial.print(ch[3]); Serial.print("\t");
  Serial.print(ch[4]); Serial.print("\t");
  Serial.print(ch[5]); Serial.print("\t");
  Serial.print(ch[6]); Serial.print("\t");
  Serial.print(ch[7]); Serial.print("\t");
  Serial.print(ch[8]); Serial.print("\t");
  Serial.print(ch[9]); Serial.print("\t");
  Serial.print(ch[0]); Serial.print("\n");
  delay(10);
}
//--------------------------------------------------------------------------------------------------//
void read_me() {
  //this code reads value from RC reciever from PPM pin (Pin 2 or 3)
  //this code gives channel values from 0-1000 values
  // -: ABHILASH :- //

  a = micros(); //store time value a when pin value falling
  c = a - b; //calculating time inbetween two peaks
  b = a; //
  x[i] = c; //storing 15 value in array
  i = i + 1; if (i == 15) {
    for (int j = 0; j < 15; j++) {
      ch1[j] = x[j];
    }
    i = 0;
  }
}//copy store all values from temporary array another array after 15 reading
void read_rc() {
  int i, j, k = 0;
  for (k = 14; k > -1; k--) {
    if (ch1[k] > 5000) {
      j = k; //detecting separation space 10000us in that another array
    }
  }
  for (i = 0; i <= totalChannels; i++) {
    ch[i] = (ch1[i + j] - 1000); //assign 6 channel values after separation space
  }
}
