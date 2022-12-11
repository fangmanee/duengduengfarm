const int potPin = 2;
int potValue = 0;

void setup() {
  pinMode(potPin,INPUT);
  Serial.begin(9600);
  delay(1000);
}
void loop() {
  potValue = analogRead(potPin); // อ่านค่า Analog จากขา 34
  Serial.println(potValue);
  delay(500);
}
