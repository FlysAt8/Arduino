#include <Servo.h>

Servo myservo;
int pos = 90;

byte ena = 3;
byte in1 = 4;
byte in2 = 5;
byte enb = 6;
byte in3 = 8;
byte in4 = 7; 
byte PIN_TRIG = 12;
byte PIN_ECHO = 11;


void setup() {
  myservo.attach(9);

  Serial.begin(115200);

  myservo.write(90);

  pinMode(ena, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enb, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
}


void loop() {

  int sm = 0;
  while (sm == 0) {
    sm = rast(1);
    delay(10);
  }

  int back = 0;

  if (sm < 10 && sm > 1) {
    back = 1;
    dvig(5);
  } else {
    back = 0;
  }
  while (back == 1) {
    sm = rast(1);
    delay(50);
    if (sm > 30) {
      dvig(4);
      back = 0;
    }
  }


  if (Serial.available() > 0) {
    int buf = Serial.parseInt();
    if (buf == 1 || buf == 2 || buf == 3)
      pov(buf);
    if (buf >= 4 && buf <= 8)
      dvig(buf);
  }
}

int rast(int x){

int duration;
int cm;
// Сначала генерируем короткий импульс длительностью 2-5 микросекунд.

digitalWrite(PIN_TRIG, LOW);
delayMicroseconds(5);
digitalWrite(PIN_TRIG, HIGH);

// Выставив высокий уровень сигнала, ждем около 10 микросекунд. В этот момент датчик будет посылать сигналы с частотой 40 КГц.
delayMicroseconds(10);
digitalWrite(PIN_TRIG, LOW);

//  Время задержки акустического сигнала на эхолокаторе.
duration = pulseIn(PIN_ECHO, HIGH);

// Теперь осталось преобразовать время в расстояние
cm = (duration / 2) / 29.1;
return cm;
}

int dvig(int x) {
  if (x == 0) return;
  if (x == 8) {  // Вперед
    analogWrite(ena, 200);
    analogWrite(enb, 200);
    // выставляем режим мотора - вращение по часовой
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
  }
  if (x == 5) {  // назад
    analogWrite(ena, 100);
    analogWrite(enb, 100);
    // режим мотора - вращение против часовой
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
  }
  if (x == 4) {  // тормоз
    analogWrite(ena, 200);
    analogWrite(enb, 200);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
  }
}

int pov(int x) {
  int pos1;
  if (x == 0) return;
  if (x == 3) pos1 = 60;
  if (x == 2) pos1 = 90;
  if (x == 1) pos1 = 120;
  if (pos > pos1) {
    for (int i = pos; i >= pos1; i--) {
      myservo.write(i);
      delay(15);
    }
    pos = pos1;
  }
  if (pos < pos1) {
    for (int i = pos; i <= pos1; i++) {
      myservo.write(i);
      delay(15);
    }
    pos = pos1;
  }
  return 0;
}
