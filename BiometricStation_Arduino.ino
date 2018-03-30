void setup() {
Serial.begin(115200);  
Serial.println("Hello World");

}

void loop() {
  
int pubg = 1234;
double level = 12.33;

Serial.println(String("[") + level + String("|") + pubg + String("]"));
delay(2000);

}
