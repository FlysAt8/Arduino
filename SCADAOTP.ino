#include <iarduino_DHT.h>
#include <SoftwareSerial.h>
#include <DHT.h>
#define DHTPIN 9
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
iarduino_DHT sensor1(3); 

const int prm = 3; // пин входа приёмника XY-MK-5V
#define prd 4 // пин DATA перед1атчика FS1000A 
const int ledPin = 13; // пин встроенного светодиода Arduino
const int len = 20; // длина массивов

bool state = false; // текущее состояние светодиода
int pattern[len] = {0,0,0,1,1,1,1,0,0,1,1,0,0,1,1,1,1,0,0,0}; // эталонный массив - маска команды, которую нужно «словить»
int testReg[len] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // тестовый регистр - массив оцифрованных значений с входа приёмника

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  pinMode(prd, OUTPUT); 
  dht.begin();
}

void loop() {
  if (Serial.available()) {
    int val = Serial.parseInt();
    if (val == 1){
      Serial.println(val);
      sendCommand();
      pered();
    }
  }

  int data = digitalRead(prm); // читаем значение на входе приёмника
  ShiftReg(data, testReg);
  if (IsCommandDetected()) { // проверяем, нет ли в тестовом регистре искомой последовательности
      state = !state;
      digitalWrite(ledPin, state);
      delay(150);
      sendCommand();
  }
  delay(5);
}

bool IsCommandDetected() {
  for (int i=0; i<len; i++) {
    if (testReg[i] != pattern[i]) { // почленно сравниваем 2 массива
      return false;
    }
  }
  return true;
}

void ShiftReg(int newVal, int *arr) {
  for (int i=0; i<len; i++) {
    arr[i] = testReg[i+1]; // смещаем значения в регистре на 1 позицию
  }
  arr[len-1] = newVal; // последнюю позицию заменяем только что принятым измерением
}

void pered() {
  float h1 = dht.readHumidity(); //Измеряем влажность
  float t1 = dht.readTemperature(); //Измеряем температуру
  int t=t1;
  int h=h1;
  t = random (5,35);
  h = random (60,90);
  if (t > 63) t = 63;
  if (t < 0) t = 0;
  if (h > 99) h = 99;
  if (h < 36) h = 36;

  Serial.print("t = ");Serial.print(t);Serial.print(" - ");
  int x[6];
  x[5] = t%2; t=t/2;
  x[4] = t%2; t=t/2;
  x[3] = t%2; t=t/2;
  x[2] = t%2; t=t/2;
  x[1] = t%2; t=t/2;
  x[0] = t%2;
  for (int i=0; i<=5; i++){
    Serial.print(x[i]);
  }
  Serial.println();

  Serial.print("h = ");Serial.print(h);Serial.print(" - ");
  h=h-36;
  int y[6];
  y[5] = h%2; h=h/2;
  y[4] = h%2; h=h/2;
  y[3] = h%2; h=h/2;
  y[2] = h%2; h=h/2;
  y[1] = h%2; h=h/2;
  y[0] = h%2;
  for (int i=0; i<=5; i++){
    Serial.print(y[i]);
  }
  Serial.println();

  digitalWrite(prd, HIGH);
  delay(20);
  for (int i=0; i<=5; i++){
    if (x[i]==1) digitalWrite(prd, HIGH);
    if (x[i]==0) digitalWrite(prd, LOW);
    delay(30);
  }
  digitalWrite(prd, HIGH);
  delay(20);
  for (int i=0; i<=5; i++){
    if (y[i]==1) digitalWrite(prd, HIGH);
    if (y[i]==0) digitalWrite(prd, LOW);
    delay(30);
  }
  digitalWrite(prd, LOW);
  return;
}

void sendCommand() {
  digitalWrite(prd, HIGH);
  delay(20); 
  digitalWrite(prd, LOW);
  delay(10); 
  digitalWrite(prd, HIGH);
  delay(10); 
  digitalWrite(prd, LOW);
  delay(10); 
  digitalWrite(prd, HIGH);
  delay(20); 
  digitalWrite(prd, LOW);
  delay(10);
}