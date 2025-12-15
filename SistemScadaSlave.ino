#include <iarduino_RF433_Receiver.h>  // Подключаем библиотеку для работы с приёмником MX-RM-5V
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <AESLib.h>

#include "DHT.h"
#define DHTPIN 3
#define DHTTYPE DHT11  // DHT 11

#define led1_pin 8   // свет з
#define led2_pin 7   // свет к

DHT dht(DHTPIN, DHTTYPE);

uint8_t key[] = { 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1 };        // ключ шифрования
byte data[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };  // массив шифрования

RF24 radio(9, 10);                  // система для данных
iarduino_RF433_Receiver radio1(2);  // система для опроса и команд

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
  pinMode(led1_pin, OUTPUT);
  digitalWrite(led1_pin, LOW);
  pinMode(led2_pin, OUTPUT);
  digitalWrite(led2_pin, HIGH);
  dht.begin();
  radio1.begin();                  // Инициируем работу приёмника MX-RM-5V
  radio1.setDataRate(i433_1KBPS);  // Указываем скорость приёма данных
  radio1.openReadingPipe(5);
  radio1.startListening();  // Включаем приемник, начинаем прослушивать открытую трубу
  radio.begin();
  delay(2000);
  radio.setChannel(0x6f);
  radio.setDataRate(RF24_2MBPS);
  radio.setCRCLength(RF24_CRC_16);
  radio.setPALevel(RF24_PA_MAX);
  radio.openWritingPipe(pipe);
  time = millis();
}

bool valve[8] = { false, false, false, false, false, false, false, false };

void loop() {
  packet_com pac_c1;
  if (radio1.available()) {  // Если в буфере имеются принятые данные
    Serial.println("Пришел опрос");
    radio1.read(&pac_c1, sizeof(packet_com));
    aes128_dec_single(key, data);

    if (pac_c1.id == 1) {
      delay(300);

      for (int i = 0; i < 8; i++) {
        valve[i] = pac_c1.valve[i];
      }

      if (valve[0]) {
        digitalWrite(led1_pin, HIGH);
        digitalWrite(led2_pin, LOW);
      } else {
        digitalWrite(led1_pin, LOW);
        digitalWrite(led2_pin, HIGH);
      }

      packet_data pac_d1;
      pac_d1.id = 1;

      for (int i = 0; i < 8; i++) {
        pac_d1.valve[i] = valve[i];
      }

      pac_d1.data[0] = random(22, 28);
      pac_d1.data[1] = random(60, 75);
      pac_d1.data[2] = random(1, 8);

      radio.write(&pac_d1, sizeof(packet_data));
      Serial.println("Отправил:");
      Serial.print("Заслонка 1 = " + String(valve[0]) + '\t');
      Serial.println("Заслонка 2 = " + String(valve[1]));
      Serial.print("Заслонка 3 = " + String(valve[2]) + '\t');
      Serial.println("Заслонка 4 = " + String(valve[3]));
      Serial.print("Заслонка 5 = " + String(valve[4]) + '\t');
      Serial.println("Заслонка 6 = " + String(valve[5]));
      Serial.print("Заслонка 7 = " + String(valve[6]) + '\t');
      Serial.println("Заслонка 8 = " + String(valve[7]));
      Serial.println("Температура = " + String(pac_d1.data[0]));
      Serial.println("Влажность = " + String(pac_d1.data[1]));
      Serial.println("Давление = " + String(pac_d1.data[2]));
      Serial.println();
      memset(&pac_d1, 0, sizeof(packet_data));
      memset(&pac_c1, 0, sizeof(packet_com));
    }
  }
}