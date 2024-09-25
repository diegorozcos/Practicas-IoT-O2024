// Prácticas materia IoT - Práctica realizada por Diego Arturo Orozco Sánchez y Luis Gerardo Padilla Torres
#include "DHT.h" // Se incluye la librería DHT para trabajar con sensores de temperatura y humedad.

// Definimos el pin y el tipo de sensor.
#define DHTPIN 15  // Pin digital al que está conectado el sensor.
#define DHTTYPE DHT11  // Definimos el tipo de sensor (DHT11).

// Creamos una instancia del sensor DHT usando el pin y tipo definidos.
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);  // Iniciamos la comunicación serial a 9600 baudios.
  Serial.println(F("DHT11 test!"));  // Imprimimos un mensaje inicial en el monitor serial para indicar que el sensor se está probando.

  dht.begin();  // Iniciamos el sensor DHT.
}

void loop() {
  delay(2000);  // Pausamos 2 segundos entre lecturas porque el sensor es lento.
  
  // Leemos la humedad desde el sensor.
  float h = dht.readHumidity();
  
  // Leemos la temperatura desde el sensor.
  float t = dht.readTemperature();

  // Verificamos si las lecturas son válidas. Si no lo son, se imprime un mensaje de error.
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));  // Si la lectura falla, imprimimos un mensaje de error.
    return;  // Salimos del loop sin continuar.
  }

  // Si las lecturas son correctas, imprimimos los valores de humedad y temperatura.
  Serial.print(F("Humedad: "));  // Imprimimos el texto "Humedad".
  Serial.print(h);  // Imprimimos el valor de la humedad.
  Serial.print(F("% Temperatura: "));  // Imprimimos el texto "Temperatura".
  Serial.print(t);  // Imprimimos el valor de la temperatura.
  Serial.println(F("°C "));  // Imprimimos el símbolo de grados Celsius y hacemos un salto de línea.
}
