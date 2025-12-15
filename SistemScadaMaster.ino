#include <SPI.h>
#include <iarduino_RF433_Transmitter.h>  // Передатчик FS1000A
#include <nRF24L01.h>                    // Модуль RF24
#include <RF24.h>
#include <AESLib.h>

#include <ModbusRtu.h>  // библиотека
#define ID 1            // адрес ардуино

uint8_t key[] = { 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1 };        // ключ шифрования
byte data[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };  // массив шифрования

Modbus slave(ID, 0, 0);  // создание устройства

#define led1_pin 3

uint16_t mb[12];  // массив для передачи

RF24 radio(9, 10);                     // система для данных
iarduino_RF433_Transmitter radio1(2);  // система для опроса и команд

struct packet_data {  // пакеты с данными
  int id;
  bool valve[8];
  int data[3];
};

struct packet_com {  // пакет для команд
  int id;
  bool valve[8];
};

const uint32_t pipe = 111156789;  // труба передачи

unsigned long time;

void setup() {
  Serial.begin(9600);
  radio1.begin();                  // Инициируем работу передатчика FS1000A
  radio1.setDataRate(i433_1KBPS);  // Указываем скорость передачи данных
  radio1.openWritingPipe(5);       // Открываем 5 трубу для передачи данных
  radio.begin();
  delay(2000);
  radio.setChannel(0x6f);
  radio.setDataRate(RF24_2MBPS);  // Указываем скорость передачи данных
  radio.setCRCLength(RF24_CRC_16);
  radio.setPALevel(RF24_PA_MAX);   // Указываем мощность передатчика
  radio.openReadingPipe(1, pipe);  // Открываем 1 трубу
  radio.startListening();          // Начинаем слушать
  slave.begin(9600);
  time = millis();
  pinMode(led1_pin, OUTPUT);
  digitalWrite(led1_pin, LOW);
}


bool state[8] = { false, false, false, false, false, false, false, false };

void loop() {

  slave.poll(mb, 12);  // считываем регистры

  for (int i = 0; i < 8; i++) {
    if (state[i] != mb[i]) {
      state[0] = mb[0];
      state[1] = mb[1];
      state[2] = mb[2];
      state[3] = mb[3];
      state[4] = mb[4];
      state[5] = mb[5];
      state[6] = mb[6];
      state[7] = mb[7];
      opros();
      break;
    }
  }

  if (millis() - 5000 >= time) opros();
}

void opros() {
  packet_com pac_c1;
  pac_c1.id = 1;

  for (int i = 0; i < 8; i++) {
    pac_c1.valve[i] = state[i];
  }

  int k = 10;
  aes128_enc_single(key, data);
  radio1.write(&pac_c1, sizeof(packet_com));
  aes128_dec_single(key, data);
  Serial.println("Опросил устройство 1");
  digitalWrite(led1_pin, HIGH);
  time = millis();
  otvet();
  memset(&pac_c1, 0, sizeof(packet_com));
  time = millis();
}

void otvet() {
  while (millis() - 2000 < time) {
    packet_data pac_d1;
    if (radio.available()) {                     // Если в буфере имеются принятые данные
      radio.read(&pac_d1, sizeof(packet_data));  // Читаем данные в массив data и указываем сколько байт читать
      Serial.println("Принял пакет:");
      Serial.print("Заслонка 1 = " + String(pac_d1.valve[0]) + '\t');
      Serial.println("Заслонка 2 = " + String(pac_d1.valve[1]));
      Serial.print("Заслонка 3 = " + String(pac_d1.valve[2]) + '\t');
      Serial.println("Заслонка 4 = " + String(pac_d1.valve[3]));
      Serial.print("Заслонка 5 = " + String(pac_d1.valve[4]) + '\t');
      Serial.println("Заслонка 6 = " + String(pac_d1.valve[5]));
      Serial.print("Заслонка 7 = " + String(pac_d1.valve[6]) + '\t');
      Serial.println("Заслонка 8 = " + String(pac_d1.valve[7]));
      Serial.println("Температура = " + String(pac_d1.data[0]));
      Serial.println("Влажность = " + String(pac_d1.data[1]));
      Serial.println("Давление = " + String(pac_d1.data[2]));
      Serial.println();

      for (int i = 0; i < 8; i++) {
        mb[i] = pac_d1.valve[i];
      }
      mb[8] = pac_d1.data[0];
      mb[9] = pac_d1.data[1];
      mb[10] = pac_d1.data[2];

      mb[11] = false;
      memset(&pac_d1, 0, sizeof(packet_data));
      digitalWrite(led1_pin, LOW);
      return;
    }
  }
  mb[11] = true;
  Serial.println("Нет ответа");
  digitalWrite(led1_pin, LOW);
}