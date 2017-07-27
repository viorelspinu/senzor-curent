#include <LiquidCrystal.h>
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

#include <SPI.h>

#include <Ethernet2.h>

byte mac[] = { 0x90, 0xA2, 0xDA, 0x10, 0xE1, 0xEE };

EthernetClient client;

char server[] = "iot.robofun.ro";


bool isInternet = true;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("START INTERNET..");

  if (Ethernet.begin(mac) == 0) {
    isInternet = false;
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 0);
    lcd.print("NO INTERNET");
  } else {
    isInternet = true;
    Serial.println(Ethernet.localIP());
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 0);
    lcd.print(Ethernet.localIP());
  }
}

int oldVal = 0;

long iotUpdateTime = 0;

void loop() {
  int val = readCurrent();

  if (val < 16) {
    val = 0;
  }

  if (oldVal != val) {

    if (oldVal == 0) {
      iotUpdateTime = millis();
      sendIOT(oldVal);
      sendIOT(val);
    }

    if (oldVal != 0) {
      if ((millis() - iotUpdateTime) > 60 * 10000) {
        iotUpdateTime = millis();
        sendIOT(val);
      }
    }

    if (val == 0){
        iotUpdateTime = millis();
        sendIOT(oldVal);
        sendIOT(val);
    }
    
    oldVal = val;
  }




  Serial.println(val);
  lcd.setCursor(0, 1);
  lcd.print("     ");
  lcd.setCursor(0, 1);
  lcd.print(val);

  delay(10);

}


void sendIOT(int val) {
  if (isInternet) {
    String temp = "GET /api/v1/senzor/TOKEN/input?value=" + String(val) + " HTTP/1.1";
    char param[100];
    temp.toCharArray(param, temp.length() + 1);

    if (client.connect(server, 80)) {
      client.println(param);
      client.println("Host: iot.robofun.ro");
      client.println("User - Agent: arduino - ethernet");
      client.println("Connection: close");
      client.println();
      Serial.println("GET DONE");
    }
    client.stop();
  }

}

int readCurrent() {
  long minim = 1023;
  long maxim = 0;
  for (int i = 0; i < 500; i++) {
    int v = analogRead(0);
    if (minim > v) {
      minim = v;
    }
    if (maxim < v) {
      maxim = v;
    }
  }
  return maxim - minim;
}


