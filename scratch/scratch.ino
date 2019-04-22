#define PIN1 47
#define PIN2 45

bool state = false;
unsigned long t; 

void setup() {
  // put your setup code here, to run once:
//  pinMode(PIN1, INPUT_PULLUP);
//  pinMode(PIN2, INPUT_PULLUP);
  pinMode(53, OUTPUT);
  pinMode(A15, INPUT);
  Serial.begin(115200);
//  Serial.println("listening for limit switches");
}

void loop() {
  // put your main code here, to run repeatedly:
//  Serial.print("Pin1:" + String(digitalRead(PIN1)) + " ");
//  Serial.print("Pin2:" + String(digitalRead(PIN2)) + " \n");
//  delay(500);
  if (millis() - t > 500)
  {
    digitalWrite(53, state ? HIGH : LOW);
    state = !state;
    t = millis();
  }
  Serial.println(analogRead(A15));
}
