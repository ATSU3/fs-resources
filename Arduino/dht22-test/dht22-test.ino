#include <WiFi.h>
#include <DHT.h>

#define WIFI_SSID "WIFI"
#define WIFI_PASS "PASSWORD"

#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

int fail_count = 0;

unsigned long durationtime;
unsigned long pretime;

void setup()
{
  Serial.begin(115200);
  Serial.println("");
  Serial.println("*** Wake up ESP32...");

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  delay(5); 

  dht.begin();
  delay(5);
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED) {

    durationtime = (millis() - pretime) / 1000;
    pretime = millis();

    Serial.print("duration time: "); 
    Serial.print(durationtime);
    Serial.println(" sec for this loop");

    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    Serial.print("Temp: "); 
    Serial.print(temperature, 4);
    Serial.println("*C");
    Serial.print("Humi: "); 
    Serial.print(humidity, 4);
    Serial.println("%"); 

    delay(5000);
  } else {
    Serial.print("Not connected to the Internet:");
    Serial.println(WiFi.status());
    delay(250);

    if (50 <= fail_count++) {
      ESP.restart();
    }
  }
}
