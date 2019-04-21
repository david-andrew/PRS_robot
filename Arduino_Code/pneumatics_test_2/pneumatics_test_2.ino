void setup() {
  // put your setup code here, to run once:
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(8, HIGH);
  digitalWrite(9, LOW);

  Serial.begin(9600);
  Serial.println("GO!");
}

void loop() {
  // put your main code here, to run repeatedly:

}
