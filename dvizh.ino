

void setup () {
  Serial.begin (115200); // open serial port, and set baud rate at 9600bps
  pinMode (4, INPUT); // Define PIR as input in D2
  pinMode (18, OUTPUT); // Define LED as output in D13
}

void loop () {
  Serial.println (digitalRead (21));
  if (digitalRead (4) == 1) {
    digitalWrite (18, HIGH); // LED light is on
  } else {
      digitalWrite (18, LOW); // LED light is not on
    }
  
  
}