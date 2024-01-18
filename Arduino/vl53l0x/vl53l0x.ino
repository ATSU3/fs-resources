#include <AzureIoTHub.h>
#include <Wire.h>
#include "VL53L0X.h"

VL53L0X sensor;
int fail_count = 0;
const uint8_t PORT0A = 1;
const uint8_t PORT0B = 3;
const uint8_t PORT1A = 4;
const uint8_t PORT1B = 5;
const uint8_t PORT_POWER = 15; // (common with RED_LED)

const uint8_t FUNC_BTN = 0;
const uint8_t BLUE_LED = 2;
const uint8_t RED_LED = PORT_POWER;

const uint8_t UART_TX = PORT0A;
const uint8_t UART_RX = PORT0B;
const uint8_t I2C_SDA = PORT1A;
const uint8_t I2C_SCL = PORT1B;


unsigned long durationtime;
unsigned long pretime;

void setup() {
  Serial.begin(115200);
  Serial.println("");
  Serial.println("*** Wake up WioNode...");

  // WiFi及びAzure周りの初期化
  WiFi.begin("", "");    // 2.4GHz帯のWiFiを指定すること
  Azure.begin("");
  Azure.setCallback(azureCallback);

  // IO周りの初期化
  pinMode(BLUE_LED, OUTPUT);
  pinMode(PORT_POWER, OUTPUT);
  digitalWrite(PORT_POWER, HIGH);
  digitalWrite(BLUE_LED, HIGH);
  delay(5); 

  // VL53L0Xセンサーの初期化
  Wire.begin(I2C_SDA,I2C_SCL); // I2Cの初期化
  sensor.init();
  sensor.setTimeout(500);
  sensor.startContinuous(); // 連続測定モードの開始
}

void azureCallback(String s) {
  Serial.print("azure Message arrived [");
  Serial.print(s);
  Serial.println("] ");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    Azure.connect();
    DataElement a = DataElement();

    durationtime = (millis() - pretime)/1000;
    pretime = millis();

    Serial.print("duration time: "); 
    Serial.print(durationtime);
    Serial.println(" sec for this loop");

    // 距離センサーで距離を測定
    Serial.print("Distance: ");
    int distance = sensor.readRangeContinuousMillimeters();
    Serial.print(distance);
    if (sensor.timeoutOccurred()) { 
        Serial.print(" TIMEOUT"); 
    } else {
        // Azureへ送るデータの用意(距離)
        a.setValue("Sensor", "distance");    
        a.setValue("Distance", distance);
        a.setValue("duration", (int)durationtime);
        // Azure IoT Hub へプッシュ
        Azure.push(&a);
    }

    Serial.println();

    // wait中は青LEDを消灯する
    digitalWrite(BLUE_LED, HIGH);
    
    // 1秒の待機
    delay(1000);
  } else {
    Serial.print("Not connected to the Internet:");
    Serial.println(WiFi.status());
    delay(250);

    if(50 <= fail_count++) {
      ESP.restart();
    }
  }
}

String address(IPAddress ip){
  String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
  return ipStr;
}
