int MQ2 = A7;  // Define MQ2 gas sensor pin at A0
int val = 0;    // declare variable
int led = 18;   // Define the buzzer pin at D3
void setup() {
  pinMode(MQ2, INPUT);   // MQ2 gas sensor as input
  Serial.begin(115200);  // Set the serial port baud rate to 9600
  pinMode(led, OUTPUT);  // Set the digital IO pin mode for output
}
void loop() {
  val = analogRead(MQ2);  // Read the voltage value of A0 port and assign it to val
  Serial.println(val);    // Serial port sends val value
  delay(500);

  
  // if (val > 250) {
  //   digitalWrite(led, HIGH);
  //   delay(300);
  // } else {
  //   digitalWrite(led, LOW);
  // }
}