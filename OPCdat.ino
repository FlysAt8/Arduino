#include <iarduino_DHT.h>
#include <SoftwareSerial.h>
#include <DHT.h>

#define ID   1

#define DHTPIN 10
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
iarduino_DHT sensor1(3); 
int aPin=A0;

#define ledPin  13
#define inB  9
#define inA  8

#define prd  4  // the number of the LED output
const int prm = 3; // пин входа приёмника XY-MK-5V

const int len = 32; // длина массивов
int testReg[len] = {0,0,0,0,0,0,0,// 7
                    0,0,0,0,0,0, //6
                    0,0,0,0,0,0,0, //7
                    0,0,0,0,0,0,0, //7
                    0,0,0,0,0}; //5

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(prd, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(inB, OUTPUT);
  pinMode(inA, OUTPUT);
  dht.begin();
}

bool l=false;
bool o=false;

int pack[32];
void loop() {
  // put your main code here, to run repeatedly:
  int data = digitalRead(prm); // читаем значение на входе приёмника
  ShiftReg(data, testReg);

  if (IsCommandDetected()){ // проверяем, нет ли в тестовом регистре искомой последовательности
    
    for (int i = 0; i < 32; i++){
      pack[i]=testReg[i];
    }
    //RePack();
    Serial.println("Получили:");
    Serial.print(" ");
    for (int i = 0; i<32;i++){
      Serial.print(pack[i]);
    }
    Serial.println();
    obrabot();
    Serial.print(" ID="); Serial.print(ID);
    Serial.print(" l=");  Serial.print(l);
    Serial.print(" o=");  Serial.println(o);
    Serial.println("Отправляем:");
    Serial.print(" ID="); Serial.print(ID);
    Serial.print(" l=");  Serial.print(l);
    Serial.print(" o=");  Serial.println(o);
    formpack();
    delay(100);
    Serial.print(" ");
    for (int i = 0; i<32;i++){
      Serial.print(pack[i]);
    }
    Serial.println();
    otpr();
    for (int i = 0; i<len;i++) testReg[i]=0;
    for (int i = 0; i<32;i++) pack[i]=0;
  }
  delay(5);
}

void otpr() {
  for (int i=0; i<7; i++){
    if (pack[i]==1) digitalWrite(prd, HIGH);
    if (pack[i]==0) digitalWrite(prd, LOW);
    delay(5);
  }
  for (int i=7; i<=26; i++){
    if (pack[i]==1) digitalWrite(prd, HIGH);
    if (pack[i]==0) digitalWrite(prd, LOW);
    delay(5);
  }
  for (int i=27; i<=31; i++){
    if (pack[i]==1) digitalWrite(prd, HIGH);
    if (pack[i]==0) digitalWrite(prd, LOW);
    delay(5);
  }
  digitalWrite(prd, LOW);
}

void formpack() {
  // ID
  if (ID == 1) {
    pack[0]=1;pack[1]=0;pack[2]=1;
  }

  // Fun
  if (l){
    pack[3]=1;pack[4]=1;
  } else{
    pack[3]=0;pack[4]=0;
  }
  if (o){
    pack[5]=1;pack[6]=1;
  } else{
    pack[5]=0;pack[6]=0;
  }

  // Датчики
  float hA1 = dht.readHumidity(); //Измеряем влажность
  float tA1 = dht.readTemperature(); //Измеряем температуру
  int hG = analogRead(aPin);
  hG = 100-(float(hG)/1023*100);
  int tA=tA1;
  int hA=hA1;
  tA = random(10,40);
  hA = random(60,100);
  hG = random(0,100);
  
  Serial.print(" tA=");  Serial.print(tA);
  Serial.print(" hA=");  Serial.print(hA);
  Serial.print(" hG=");  Serial.println(hG);

  pack[12] = tA%2; tA=tA/2;
  pack[11] = tA%2; tA=tA/2;
  pack[10] = tA%2; tA=tA/2;
  pack[9]  = tA%2; tA=tA/2;
  pack[8]  = tA%2; tA=tA/2;
  pack[7]  = tA%2;

  pack[19] = hA%2; hA=hA/2;
  pack[18] = hA%2; hA=hA/2;
  pack[17] = hA%2; hA=hA/2;
  pack[16] = hA%2; hA=hA/2;
  pack[15] = hA%2; hA=hA/2;
  pack[14] = hA%2; hA=hA/2;
  pack[13] = hA%2;

  pack[26] = hG%2; hG=hG/2;
  pack[25] = hG%2; hG=hG/2;
  pack[24] = hG%2; hG=hG/2;
  pack[23] = hG%2; hG=hG/2;
  pack[22] = hG%2; hG=hG/2;
  pack[21] = hG%2; hG=hG/2;
  pack[20] = hG%2;
  // Сумма
  int sum=0;
  for (int i = 7; i <= 26; i++) {
    sum = sum + pack[i];
  }
  pack[31] = sum%2; sum=sum/2;
  pack[30] = sum%2; sum=sum/2;
  pack[29] = sum%2; sum=sum/2;
  pack[28] = sum%2; sum=sum/2;
  pack[27] = sum%2;
}

void obrabot(){
  if ((pack[3]==1) && (pack[4]==1)){
    l = true;
    digitalWrite(ledPin,HIGH);
  } else {
    l = false;
    digitalWrite(ledPin,LOW);
  }
  if ((pack[5]==1) && (pack[6]==1)){
    o = true;
    digitalWrite(inB,HIGH);
    digitalWrite(inA,LOW);
  } else {
    o = false;
    digitalWrite(inB,LOW);
    digitalWrite(inA,LOW);
  }
}

// void RePack() {
//   int x;
//   for (int i = 0; i < 7; i++) pack[i]=testReg[i];
//   for (int i = 7; i <= 26; i++){
//     x=testReg[i*3+0]+testReg[i*3+1]+testReg[i*3+2];
//     if (x >= 2){
//       pack[i]=1;
//     } else {
//       pack[i]=0;
//     }
//   }
//   for (int i = 27; i <= 31; i++) pack[i]=testReg[i];
// }

bool IsCommandDetected() {
  if ((testReg[0]==1) && (testReg[1]==0) && (testReg[2]==1)){
    if (((testReg[3]==1) && (testReg[4]==1)) || ((testReg[3]==0) && (testReg[4]==0))){
      if (((testReg[5]==1) && (testReg[6]==1)) || ((testReg[5]==0) && (testReg[6]==0))){
        return true;
      }
      return false;
    }
    return false;
  }
  return false;
}

void ShiftReg(int newVal, int *arr) {
  for (int i=0; i<7-1; i++) {
    arr[i] = testReg[i+1]; // смещаем значения в регистре на 1 позицию
  }
  arr[7-1] = newVal; // последнюю позицию заменяем только что принятым измерением
}
