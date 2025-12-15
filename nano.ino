#include <SoftwareSerial.h>
SoftwareSerial ser(2, 3);

const int ledPin = 13; // пин встроенного светодиода Arduino

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  ser.begin(4000);
}

bool state = false;
int buf;
void loop() {
  // put your main code here, to run repeatedly:
  if(ser.readBytes((byte*)&buf, sizeof(buf))){
    Serial.print("Пришло: ");Serial.println(buf);
    if (buf == 1){
    Serial.println("Переключаю светодиод");
    state = !state;
    digitalWrite(ledPin, state);
    buf = 1;
  }
  }
}
