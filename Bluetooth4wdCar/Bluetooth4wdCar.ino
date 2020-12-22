
#define enA 2
#define enB 3

#define in1 7
#define in2 6
#define in3 5
#define in4 4

void setup() {
  pinMode(enA,OUTPUT);
  pinMode(enB,OUTPUT);
  pinMode(in1,OUTPUT);
  pinMode(in2,OUTPUT);
  pinMode(in3,OUTPUT);
  pinMode(in4,OUTPUT);
  digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
}

void loop() {
  analogWrite(enB,0);
  analogWrite(enA,0);
  delay(5000);
  analogWrite(enB,128);
  analogWrite(enA,128);
  delay(5000);
  analogWrite(enB,255);
  analogWrite(enA,255);
  delay(5000);
}
