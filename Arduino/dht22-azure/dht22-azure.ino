#include <WiFi.h>
#include <AzureIoTHub.h>
#include <DHT.h>

#define WIFI_SSID "Wifi"
#define WIFI_PASS "pass"
#define AZURE_KEY "プライマリ接続キー"
#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
int fail_count = 0;
unsigned long durationtime;
unsigned long pretime;

unsigned long lastDataSendTime = 0; 
const unsigned long dataSendInterval = 300000; 

void azureCallback(String s) {
  Serial.print("Azure Message arrived [");
  Serial.print(s);
  Serial.println("] ");
}

void setup()
{
  Serial.begin(115200);
  Serial.println("");
  Serial.println("*** Wake up ESP32...");

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Azure.begin(AZURE_KEY);
  Azure.setCallback(azureCallback);  

  dht.begin();
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED) {
    Azure.connect();

    if (millis() - lastDataSendTime >= dataSendInterval) {
      lastDataSendTime = millis();
      DataElement a = DataElement();

      durationtime = (millis() - pretime)/1000;
      pretime = millis();

      Serial.print("duration time: "); 
      Serial.print(durationtime);
      Serial.println(" sec for this loop");

      float humidity = dht.readHumidity();
      float temperature = dht.readTemperature();

      if (isnan(humidity) || isnan(temperature)) {
        Serial.println("Failed to read from DHT sensor!");
      } else {
        Serial.print("Temp: "); 
        Serial.print(temperature, 4);
        Serial.println("*C");
        Serial.print("Humi: "); 
        Serial.print(humidity, 4);
        Serial.println("%"); 

        a.setValue("Sensor", "DHT22");    
        a.setValue("TempValue", temperature);
        a.setValue("HumiValue", humidity);
        a.setValue("duration", (int)durationtime);
        Azure.push(&a);
      }
    }
  } else {
    Serial.print("Not connected to the Internet:");
    Serial.println(WiFi.status());
    delay(250);

    if(50 <= fail_count++) {
      ESP.restart();
    }
  }
}
