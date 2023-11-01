//nama : william deli
//nim : 2540119702
//uts : IOT
//no : 3
//link video : https://youtu.be/bfiMg4e6FEY

#include <Wire.h> 
#include <BH1750.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <device.h>

BH1750 lightmeter(0x23); 
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void koneksi_wifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("System connected with IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("RSSI: %d\n", WiFi.RSSI());
};

void koneksi_mqtt() {
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  for (;;) {
    if (mqttClient.connect("ESP32Client")) {
      Serial.println("Koneksi MQTT berhasil");
      break;
    } else {
      Serial.print("Koneksi Lambat. Status: ");
      Serial.print(mqttClient.state());
      Serial.println(" Mencoba koneksi ulang dalam 5 detik");
      delay(5000);
    }
  }
}

void kirim_data(uint16_t lux) {
  String kondisi;
  String pesan;
  if (lux < LUX_THRESHOLD) {
    kondisi = "Closed";
     pesan = kondisi + ", " + 2540119702 + ", " + String(lux);
     Serial.println("Nothing happened / Normal : " + pesan);
  }else{
    kondisi = "Warning";
     pesan = kondisi + ", " + 2540119702 + ", " + String(lux);
     Serial.println("opened : " + pesan);
  }
   mqttClient.publish(MQTT_TOPIC, pesan.c_str());
}

void bacaluxmeter(void *pvParameters) {
  for(;;) {
    uint16_t lux = lightmeter.readLightLevel();
    kirim_data(lux);
    vTaskDelay(2000/ portTICK_PERIOD_MS); 
  }
}

void setup() { 
  Serial.begin(115200); 
  Wire.begin(SDA,SCL); 
  lightmeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23); 
  koneksi_wifi();
  koneksi_mqtt();
  xTaskCreate(bacaluxmeter, "Light", 2048, NULL, 1, NULL);
}

void loop() {
  if (!mqttClient.connected()) {
    koneksi_mqtt();
  }
  mqttClient.loop();
}
