// Demo MQTT, pub temprature, humidity to MQTT broker
// Pin data  DHT21 connect pin D2 Node MCU  

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

#define ssid "Binary"
#define password "123456778"
// Information about MQTT Broker
#define mqtt_server "192.168.43.87" // address MQTT server 
#define mqtt_topic_pub_humidity "nguyen_phuoc_nguyen/humidity" //name topic
#define mqtt_topic_pub_temperature "nguyen_phuoc_nguyen/temperature"  
#define mqtt_topic_pub "nguyen_phuoc_nguyen"  
#define mqtt_topic_sub "nguyen_phuoc_nguyen"
#define mqtt_user "node_mcu_01"    
#define mqtt_pwd "195195"

#define DHTPIN D2
#define DHTTYPE DHT21   // DHT 21 (AM2301)

const uint16_t mqtt_port = 1883; //Port CloudMQTT

WiFiClient espClient;
PubSubClient client(espClient);

DHT dht(DHTPIN, DHTTYPE);

long lastMsg = 0;
char msg[50];
int value = 0;

void setup()
{
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port); 
  client.setCallback(callback);

  dht.begin();
}


void setup_wifi() 
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
// Hàm call back để nhận dữ liệu
void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) 
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
// Hàm reconnect thực hiện kết nối lại khi mất kết nối với MQTT Broker
void reconnect()
{
  // Chờ tới khi kết nối
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Thực hiện kết nối với mqtt user và pass
    if (client.connect("ESP8266Client",mqtt_user, mqtt_pwd)) 
    {
      Serial.println("connected");
      // Khi kết nối sẽ publish thông báo
      client.publish(mqtt_topic_pub, "ESP_reconnected");
      // ... và nhận lại thông tin này
      client.subscribe(mqtt_topic_sub);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Đợi 5s
      delay(5000);
    }
  }
}
void loop() {
  // Kiểm tra kết nối
  if (!client.connected()) 
  {
    reconnect();
  }
  client.loop();
  // Sau mỗi 2s sẽ thực hiện publish dòng hello world lên MQTT broker
  long now = millis();
  if (now - lastMsg > 2000) 
  {
    lastMsg = now;
    float h = dht.readHumidity();
    float t = dht.readTemperature();
//    float h = 80.22;
//    float t = 22.22;
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    static char temperature_char_valua[7];
    dtostrf(h,6,2,temperature_char_valua);
    static char Humidity_char_valua[7];
    dtostrf(t,6,2,Humidity_char_valua);

 //   client.publish(mqtt_topic_pub_temperature,"Temerature value");
    client.publish(mqtt_topic_pub_temperature,temperature_char_valua);  
     
  //  client.publish(mqtt_topic_pub_humidity,"Humidity value");
    client.publish(mqtt_topic_pub_humidity,Humidity_char_valua);
    
 
//    ++value;
//    snprintf (msg, 75, "hello world #%ld", value);
//    Serial.print("Publish message: ");
//    Serial.println(msg);
//    client.publish(mqtt_topic_pub, msg);
    
  }
}
