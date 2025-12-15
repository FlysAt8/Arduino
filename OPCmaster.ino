#include <ModbusRtu.h>
#include <SoftwareSerial.h>

#define ID   1
Modbus slave(ID, 1, 0);
uint16_t au16data[11];

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
  slave.begin(9600);
  pinMode(prd, OUTPUT);
}

unsigned long time = millis();

bool l = false;
bool o = false;
int pack[32];
void loop() {
  // // put your main code here, to run repeatedly:
  slave.poll( au16data, 11); 

  if ((millis() > time+5000) || (l != bitRead(au16data[1],2) || (o != bitRead(au16data[1],3)))){
    l = bitRead(au16data[1],2);
    o = bitRead(au16data[1],3);

    for (int i = 0; i<32;i++) pack[i]=0;
    formpack();
    otpr();
    time = millis();
    priem();
    time = millis();
  }

  io_poll();
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
  for (int i = 0; i <= 26; i++) {
    sum = sum + pack[i];
  }
  pack[31] = sum%2; sum=sum/2;
  pack[30] = sum%2; sum=sum/2;
  pack[29] = sum%2; sum=sum/2;
  pack[28] = sum%2; sum=sum/2;
  pack[27] = sum%2;
}

void otpr() {
  for (int i=0; i<31; i++){
    if (pack[i]==1) digitalWrite(prd, HIGH);
    if (pack[i]==0) digitalWrite(prd, LOW);
    delay(5);
  }
  digitalWrite(prd, LOW);
}


int tA,hA,hG;
void priem() {
  for (int i = 0; i<len;i++) testReg[i]=0;
  while(1){
    if(millis() > time + 3000){
      return;
    }

    int data = digitalRead(prm); // читаем значение на входе приёмника
    ShiftReg(data, testReg);

    if (IsCommandDetected()) { // проверяем, нет ли в тестовом регистре искомой последовательности
      for (int i = 0; i < 32; i++){
        pack[i]=testReg[i];
      }
      obrabot();
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

void io_poll() {
  
  au16data[0] = au16data[1];
  au16data[5] = tA;
  au16data[6] = hA;
  au16data[7] = hG;
  l = bitRead(au16data[1],2);
  o = bitRead(au16data[1],3);
}

