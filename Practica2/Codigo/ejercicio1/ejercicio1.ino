#ifdef __cplusplus
extern "C" {
  #endif
  uint8_t temprature_sens_read();
  #ifdef __cplusplus
}
#endif

void setup() {
  // put your setup code here, to run once:
  pinMode(2, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(2,HIGH);
  Serial.println("Led prendido");
  delay(2500);
  digitalWrite(2,LOW);
  Serial.println("Led apagado");
  delay(2500);
  Serial.println("la temperatura interna del ESP32 es: ");
  Serial.println(((temprature_sens_read() - 32 ) / 1.8));
  Serial.println("");
}
