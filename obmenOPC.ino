
#include <SoftwareSerial.h>

#define ID   1

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
}

unsigned long time = millis();

bool l = false;
bool o = false;
int pack[32];
void loop() {
  // // put your main code here, to run repeatedly:
  if (millis() > time+5000){
    int x = random(50,100);
    int y = random(0,50);
    if (x>75){
      l=true;
    }else{
      l=false;
    }
    if (y>25){
      o=true;
    }else{
      o=false;
    }

    for (int i = 0; i<32;i++) pack[i]=0;
    formpack();
    Serial.println("Отправляем:");
    Serial.print(" ID="); Serial.print(ID);
    Serial.print(" l=");  Serial.print(l);
    Serial.print(" o=");  Serial.println(o);
    for (int i = 0; i<32;i++){
      Serial.print(pack[i]);
    }
    Serial.println();
    otpr();
    time = millis();
    priem();
    time = millis();
  }

}

void formpack() {
  // ID
    pack[0]=1;pack[1]=0;pack[2]=1;
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
  // Показ
  for (int i = 7; i <= 26; i++){
    pack[i]=0;
  }
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


int tA,hA,hG;
void priem() {
  for (int i = 0; i<len;i++) testReg[i]=0;
  Serial.println("Получаем:");
  while(1){
  if(millis() > time + 3000){
    Serial.println("Ничего");
    return;
  }

  int data = digitalRead(prm); // читаем значение на входе приёмника
  ShiftReg(data, testReg);

  if (IsCommandDetected()) { // проверяем, нет ли в тестовом регистре искомой последовательности
    for (int i = 0; i < 32; i++){
      pack[i]=testReg[i];
    }
    //RePack();
    for (int i = 0; i<32;i++){
      Serial.print(pack[i]);
    }
    Serial.println();
    obrabot();
    Serial.print(" ID="); Serial.print(ID);
    Serial.print(" l=");  Serial.print(l);
    Serial.print(" o=");  Serial.println(o);
    Serial.print(" tA=");  Serial.print(tA);
    Serial.print(" hA=");  Serial.print(hA);
    Serial.print(" hG=");  Serial.println(hG);
    return;
  }
  delay(5);
  }
}

void obrabot(){
  int x[7]={0,0,0,0,0,0,0};
  for (int i = 0; i < 6; i++){
    x[i]=pack[7+i];
  }
  tA = x[0]*pow(2,5)+x[1]*pow(2,4)+x[2]*pow(2,3)+x[3]*pow(2,2)+x[4]*2+x[5];

  for (int i = 0; i < 7; i++){
    x[i]=pack[13+i];
  }
  hA = x[0]*pow(2,6)+x[1]*pow(2,5)+x[2]*pow(2,4)+x[3]*pow(2,3)+x[4]*pow(2,2)+x[5]*2+x[6];

  for (int i = 0; i < 7; i++){
    x[i]=pack[20+i];
  }
  hG = x[0]*pow(2,6)+x[1]*pow(2,5)+x[2]*pow(2,4)+x[3]*pow(2,3)+x[4]*pow(2,2)+x[5]*2+x[6];
}

// void RePack() {
//   int x;
//   for (int i = 0; i < 7; i++) pack[i]=testReg[i];
//   for (int i = 0; i <= 19; i++){
//     x = testReg[7+i*3]+testReg[8+i*3]+testReg[9+i*3];
//     if (x>=2){
//       pack[7+i]=1;
//     }else{
//       pack[7+i]=0;
//     }
//   }
//   for (int i = 27; i <= 31; i++) pack[i]=testReg[i+40];
// }

bool IsCommandDetected() {
  if ((testReg[0]==pack[0]) && (testReg[1]==pack[1]) && (testReg[2]==pack[2])){
    if ((testReg[3]==pack[3]) && (testReg[4]==pack[4])){
      if ((testReg[5]==pack[5]) && (testReg[6]==pack[6])){
        return true;
      }
      return false;
    }
    return false;
  }
  return false;
}

void ShiftReg(int newVal, int *arr) {
  for (int i=0; i<len; i++) {
    arr[i] = testReg[i+1]; // смещаем значения в регистре на 1 позицию
  }
  arr[len-1] = newVal; // последнюю позицию заменяем только что принятым измерением
}


