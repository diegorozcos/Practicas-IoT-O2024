/************** 
 * Include Libraries 
 **************/ 
#include <WiFi.h> 
#include <PubSubClient.h> 
#include "DHT.h" 

/************** 
 * Define Constants 
 **************/ 
#define WIFISSID "IoT" // WIFI SSID aquí  
#define PASSWORD "1t3s0IoT23" // WIFI pwd  
#define TOKEN "BBUS-Lgi0XP30T14TQiFUO39ZtLGhKWs7eh" // Ubidots TOKEN  
#define MQTT_CLIENT_NAME "proyecto-iot" // ID único para el cliente MQTT  
#define VARIABLE_LABEL_temp "temperatura" // Variable Temperatura 
#define VARIABLE_LABEL_hum "humedad" // Variable Humedad 
#define VARIABLE_LABEL_dist "distancia" // Variable Distancia 
#define VARIABLE_LABEL_ldr "luz" // Variable Luz 
#define VARIABLE_LABEL_count "contador" // Variable Contador para el sensor ultrasónico
#define DEVICE_LABEL "proyecto-iotO2024" // Nombre del dispositivo en Ubidots
 
#define pin1 15 // Pin del sensor DHT11
DHT dht1(pin1, DHT11); // Sensor de temperatura y humedad 
#define TRIG_PIN 4 // Pin TRIG del sensor ultrasónico HC-SR04
#define ECHO_PIN 5 // Pin ECHO del sensor ultrasónico HC-SR04
#define LDR_PIN 34 // Pin para el sensor LDR

char mqttBroker[] = "industrial.api.ubidots.com"; 
char payload[200]; 
char topic[150]; 

// Espacio para almacenar los valores de los sensores
char str_temp[10]; 
char str_hum[10]; 
char str_dist[10]; 
char str_ldr[10]; 
char str_count[10]; 

// Variables globales
WiFiClient ubidots; 
PubSubClient client(ubidots); 
int counter = 0;  // Contador para el sensor ultrasónico

/************** 
 * Funciones auxiliares 
 **************/ 
void callback(char* topic, byte* payload, unsigned int length) { 
  char p[length + 1]; 
  memcpy(p, payload, length); 
  p[length] = NULL; 
  String message(p); 
  Serial.write(payload, length); 
  Serial.println(topic); 
} 
 
void reconnect() { 
  while (!client.connected()) { 
    Serial.println("Attempting MQTT connection..."); 
    if (client.connect(MQTT_CLIENT_NAME, TOKEN, "")) { 
      Serial.println("Connected"); 
    } else { 
      Serial.print("Failed, rc="); 
      Serial.print(client.state()); 
      Serial.println(" try again in 2 seconds"); 
      delay(2000); 
    } 
  } 
} 

float getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = (duration * 0.0343) / 2; // Calcula la distancia en cm
  return distance;
}

int getLDRValue() {
  int ldrValue = analogRead(LDR_PIN);
  return ldrValue;
}

/************** 
 * Main Functions 
 **************/ 
void setup() { 
  Serial.begin(115200); 
  WiFi.begin(WIFISSID, PASSWORD); 
   
  Serial.println(); 
  Serial.print("Wait for WiFi..."); 
   
  while (WiFi.status() != WL_CONNECTED) { 
    Serial.print("."); 
    delay(500); 
  } 
   
  Serial.println(""); 
  Serial.println("WiFi Connected"); 
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP()); 
  client.setServer(mqttBroker, 1883); 
  client.setCallback(callback);   
 
  // Configuración de sensores
  dht1.begin(); 
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);
} 
 
void loop() { 
  if (!client.connected()) { 
    reconnect(); 
  } 
  Serial.println(); 
  Serial.println("A continuación los datos de los sensores:");
  Serial.println();

  // Publica en el topic de temperatura 
  sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL); 
  sprintf(payload, "%s", ""); 
  sprintf(payload, "{\"%s\":", VARIABLE_LABEL_temp); 
  float t1 = dht1.readTemperature(); 
  Serial.print("  1. La temperatura detectada en el sensor es de: ");
  Serial.println(t1); 
  dtostrf(t1, 4, 2, str_temp); 
  sprintf(payload, "%s {\"value\": %s", payload, str_temp); 
  sprintf(payload, "%s } }", payload); 
  client.publish(topic, payload); 

  Serial.println();

  // Publica en el topic de humedad 
  sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL); 
  sprintf(payload, "%s", ""); 
  sprintf(payload, "{\"%s\":", VARIABLE_LABEL_hum); 
  float h1 = dht1.readHumidity(); 
  Serial.print("  2. La humedad detectada por el sensor es de: ");
  Serial.println(h1); 
  dtostrf(h1, 4, 2, str_hum); 
  sprintf(payload, "%s {\"value\": %s", payload, str_hum); 
  sprintf(payload, "%s } }", payload); 
  client.publish(topic, payload); 

  Serial.println();

  // Publica en el topic de distancia 
  sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL); 
  sprintf(payload, "%s", ""); 
  sprintf(payload, "{\"%s\":", VARIABLE_LABEL_dist); 
  float distance = getDistance(); 
  Serial.print("  3. La distancia detectada por el sensor HC-SR04 es: ");
  Serial.println(distance); 
  dtostrf(distance, 4, 2, str_dist); 
  sprintf(payload, "%s {\"value\": %s", payload, str_dist); 
  sprintf(payload, "%s } }", payload); 
  client.publish(topic, payload); 

  Serial.println();

  // Contador basado en el sensor ultrasónico
  if (distance <= 200) {  // Si el objeto está a 2 metros o menos
    counter++;  // Incrementa el contador
    Serial.print("Objeto detectado a menos de 2 metros. Contador incrementado a: ");
    Serial.println(counter);
  }

  // Publica en el topic de contador 
  sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL); 
  sprintf(payload, "%s", ""); 
  sprintf(payload, "{\"%s\":", VARIABLE_LABEL_count); 
  sprintf(payload, "%s {\"value\": %d", payload, counter); 
  sprintf(payload, "%s } }", payload); 
  Serial.println("  Enviando Contador a Ubidots via MQTT....");  
  client.publish(topic, payload); 

  Serial.println();

  // Publica en el topic de luz 
  sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL); 
  sprintf(payload, "%s", ""); 
  sprintf(payload, "{\"%s\":", VARIABLE_LABEL_ldr); 
  int ldrValue = getLDRValue(); 
  Serial.print("  4. El valor de luz detectado por el LDR es: ");
  Serial.println(ldrValue); 
  sprintf(payload, "%s {\"value\": %d", payload, ldrValue); 
  sprintf(payload, "%s } }", payload); 
  client.publish(topic, payload); 

  client.loop();
  
  Serial.println();
  Serial.println("Esperaré 12 seg para leer nuevamente los sensores.....");
  Serial.println();
  Serial.println(".......................................................");
  delay(12000); 
}
