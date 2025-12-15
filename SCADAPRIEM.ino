#include <iarduino_DHT.h>
#include <ModbusRtu.h>
#include <SoftwareSerial.h>
#include <DHT.h>

#define ID   1      // the address of Modbus Slave
// placed on the Arduino board
#define prd  4  // the number of the LED output
const int prm = 3; // пин входа приёмника XY-MK-5V
#define led  13
#define DHTPIN 7
DHT dht(DHTPIN, DHT11);
iarduino_DHT sensor1(3); 

const int len = 20; // длина массивов
int pattern[len] = {0,0,0,1,1,1,1,0,0,1,1,0,0,1,1,1,1,0,0,0}; // эталонный массив - маска команды, которую нужно «словить»
int testReg[len] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

Modbus slave(ID, 0, 0);
int8_t state = 0;

uint16_t au16data[11];

void setup() {
  // put your setup code here, to run once:
  io_setup();
  Serial.begin(115200);
  // set up the sequential port of Slave 
  slave.begin( 9600 );
  // turn on the LED for 100 ms
  dht.begin();
  pinMode(led, OUTPUT);   // устанавливаем 9 ногу как выход
  pinMode(prd, OUTPUT);   // устанавливаем 9 ногу как выход
  
  au16data[2]=20;
  au16data[3]=80;
}

void io_setup() {
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
}

float h = dht.readHumidity(); //Измеряем влажность
float t = dht.readTemperature(); //Измеряем температуру

void loop() {
  // put your main code here, to run repeatedly:
  int data = digitalRead(prm); // читаем значение на входе приёмника
  ShiftReg(data, testReg);

  if (IsCommandDetected()) { // проверяем, нет ли в тестовом регистре искомой последовательности
    for (int i = 0; i < len; i++) testReg[i] = 0;
    priem();
    for (int i = 0; i < len; i++) testReg[i] = 0;
  }

  state = slave.poll(au16data, 11);
  if ( state > 4){
    digitalWrite(led, LOW);
  }  
  //update data in Modbus registers and in the custom program 
  io_poll();
  delay(5);
}

void priem(){
  delay(15);
///////////////////////////////// t
  for (int i = 0; i <= 19; i++){
    int data = digitalRead(prm); // читаем значение на входе приёмника
    ShiftReg(data, testReg);
    delay(10);
  }
  int x[6];
  for (int i=0; i<=5; i++){
    int x1 = testReg[2+i*3] + testReg[3+i*3] + testReg[4+i*3];
    if (x1 >= 2) x[i] = 1;
    if (x1 <= 1) x[i] = 0;
  }
///////////////////////////////// h
  for (int i = 0; i <= 19; i++){
    int data = digitalRead(prm); // читаем значение на входе приёмника
    ShiftReg(data, testReg);
    delay(10);
  }
  int y[6];
  for (int i=0; i<=5; i++){
    int y1 = testReg[2+i*3] + testReg[3+i*3] + testReg[4+i*3];
    if (y1 >= 2) y[i] = 1;
    if (y1 <= 1) y[i] = 0;
  }

  t = x[0]*pow(2,5)+x[1]*pow(2,4)+x[2]*pow(2,3)+x[3]*pow(2,2)+x[4]*2+x[5];
  h = y[0]*pow(2,5)+y[1]*pow(2,4)+y[2]*pow(2,3)+y[3]*pow(2,2)+y[4]*2+y[5];
  h=h+36;
  au16data[2]=t;
  au16data[3]=h;
  return;
}

int buf1 = 0;
int buf2 = 0;

void io_poll() {
  //Copy Coil[1] to Discrete[0]
  au16data[0] = au16data[1];
  //Output the value of register 1.3 on the LED 
  digitalWrite(led, au16data[1]);
  
  buf2 = bitRead(au16data[1], 3);
  if (buf2>0) buf2=1; 
  if (buf1 != buf2){
    unsigned long time = millis();
    sendCommand();
    while(1){
      if (time < millis()-1000){
        time = millis();
        sendCommand();
      }

      int data = digitalRead(prm); // читаем значение на входе приёмника
      ShiftReg(data, testReg);
      if (IsCommandDetected()) { // проверяем, нет ли в тестовом регистре искомой последовательности
        for (int i = 0; i < len; i++) testReg[i] = 0;
        buf1 = buf2;
        return;
      }
      delay(5);
      if (time < millis()-5000){
        return;
      }
    }
  }
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

