
#define enA 2
#define enB 3

#define in1 7
#define in2 6
#define in3 5
#define in4 4

#define buzzer 8
#define tailLights 39

#define trigPin1 40
#define echoPin1 41
#define trigPin2 36
#define echoPin2 37

#define  signalingRight A14
#define  signalingLeft A15

#define lights 38

#define photocell A8

char command = 'P';
bool tailLights_state = 1;

int signalingRight_state = -1;
int signalingLeft_state = -1;
int x; //signaling pwm

float duration;
float cm1 = 2000; 
float cm2 = 2000; 

bool mode = true; // If mode is false the car motion is continuous, the car the car will only stop when the break button is presse
bool V_forward = false;
bool V_back = false;
bool V_right = false;
bool V_left = false;

volatile int count;
volatile float mps;

int mspeed = 255;
int newspeed;

int photocellReading;

void setup() {
  Serial3.begin(9600);
  Serial3.setTimeout(30);
  Serial.begin(9600);
  pinMode(enA,OUTPUT);
  pinMode(enB,OUTPUT);
  pinMode(in1,OUTPUT);
  pinMode(in2,OUTPUT);
  pinMode(in3,OUTPUT);
  pinMode(in4,OUTPUT);

  pinMode(buzzer,OUTPUT);
  pinMode(tailLights, OUTPUT);

  pinMode(signalingLeft,OUTPUT);
  pinMode(signalingRight,OUTPUT);

  pinMode(trigPin1,OUTPUT);
  pinMode(echoPin1,INPUT);
  pinMode(trigPin2,OUTPUT);
  pinMode(echoPin2,INPUT);

  //1 second counter
  cli();
  TCCR1A = 0;
  TCCR1B = 0; 

  OCR1A = 15624;
  TCCR1B |= (1 << WGM12);
  
  TCCR1B |= (1 << CS10);
  TCCR1B |= (1 << CS12); 
  
  TIMSK1 |= (1 << OCIE1A); 
  sei();

   attachInterrupt(digitalPinToInterrupt(18), counter, RISING);
}


void loop() {
  if(Serial3.available() > 0){ 
    command = Serial3.read();
     switch(command){
      case 'P':  
      sstop();
      V_forward = false;
      V_back = false;
      break;
     case 'M':  
      mode = true;
      sstop();
      V_forward = false;
      V_back = false;
     break;
     case 'N':  
      mode = false;
      sstop();
      V_forward = false;
      V_back = false;
      break;
    case 'W':  
      if(mode) forward(mspeed);
      else 
      {
        V_forward = true;
        V_back = false;
      }   
      tailLights_state = 0;
      break;
    case 'S':  
       if(mode) back(mspeed);
       else 
       {
        V_back = true;
        V_forward = false;
       }
       tailLights_state = 0;
      break;
    case 'A':
      if(mode) left(mspeed);
      else 
      {
        V_left = true;
      }
      tailLights_state = 0;
      break;
    case 'D':
      if(mode) right(mspeed);
      else 
      {
        V_right = true;
      }
      tailLights_state = 0;
      break; 
      case 'B':
      if(mode) sstop();
      else
      {
        V_right = false;
        V_left = false;
      }
      tailLights_state = 1;
      break;
      case 'X':
      signalingRight_state = signalingRight_state * -1;
      break; 
      case 'Z':
      signalingLeft_state = signalingLeft_state * -1;
      break;  
      case 'V':
      while(Serial3.available()<3);
      char s = Serial3.read();
      char z = Serial3.read();
      char u = Serial3.read();
      newspeed = ((int)s-48)*100+((int)z-48)*10+((int)u-48);
      if(newspeed>=100 &&  newspeed<=255)
      {
        mspeed=newspeed;
      }
      Serial.println(mspeed);
      break; 
    }

    if(command == 'H') tone(buzzer,450);
    if(command == 'J') noTone(buzzer);
  }

  if(!mode)
  {
    if(V_forward && V_left == 0 && V_right==0) forward(mspeed);
    if(V_back && V_left == 0 && V_right==0) back(mspeed);
    if(V_left) 
    {
      digitalWrite(in3,LOW);
      digitalWrite(in4,LOW);
    }
    if(V_right) 
    {
      digitalWrite(in1,LOW);
      digitalWrite(in2,LOW);
    }
  }

  photocellReading = analogRead(photocell);
  if(photocellReading <500)  analogWrite(lights, 255);
  else  analogWrite(lights, 0);
  

  if(tailLights_state == 1) analogWrite(tailLights, 255);
  else  analogWrite(tailLights, 0);

  if(signalingRight_state == 1) analogWrite(signalingRight,x);
  else analogWrite(signalingRight,0);
  if(signalingLeft_state == 1) analogWrite(signalingLeft,x);
  else analogWrite(signalingLeft,0);
  x+=5;
  if(x==255) x=0;

  if(command == 'S'){ // if direction is back ultrasonic sensor from the back is activated
    ultrasonic2();
  }
  else {
    ultrasonic1();
  }

  if(cm1 <= 15 && command == 'W') 
    {
      sstop();
      V_forward = false;
      V_back = false;
      tailLights_state = 1;
    }
   else if(cm2 <= 15 && command == 'S')
   {
      sstop();
      tailLights_state = 1;
   }
}

void counter()
{
  count++;
}


void ultrasonic2()
{
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigPin2, LOW);
  

  duration = pulseIn(echoPin2, HIGH); 
  cm2 = microsecondsToCentimeters(duration);
  
    Serial.print(cm2);
    Serial.println("        cm2");
 
}
void ultrasonic1()
{
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigPin1, LOW);

  duration = pulseIn(echoPin1, HIGH); 
  cm1 = microsecondsToCentimeters(duration);
 
}
float microsecondsToCentimeters(float microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the object we
  // take half of the distance travelled.
  return microseconds / 29 / 2;
}

void back(int speed)
{
    digitalWrite(in3,LOW);
    digitalWrite(in4,HIGH);
    digitalWrite(in1,HIGH);
    digitalWrite(in2,LOW);
    analogWrite(enB,speed);
    analogWrite(enA,speed);
}

void forward(int speed)
{
    digitalWrite(in3,HIGH);
    digitalWrite(in4,LOW);
    digitalWrite(in1,LOW);
    digitalWrite(in2,HIGH);
    analogWrite(enB,speed);
    analogWrite(enA,speed);
}

void left(int speed)
{
    digitalWrite(in3,LOW);
    digitalWrite(in4,HIGH);
    digitalWrite(in1,LOW);
    digitalWrite(in2,HIGH);
    analogWrite(enB,speed);
    analogWrite(enA,speed);
}

void right(int speed)
{
    digitalWrite(in3,HIGH);
    digitalWrite(in4,LOW);
    digitalWrite(in1,HIGH);
    digitalWrite(in2,LOW);
    analogWrite(enB,speed);
    analogWrite(enA,speed);
}

void sstop()
{
    digitalWrite(in3,LOW);
    digitalWrite(in4,LOW);
    digitalWrite(in1,LOW);
    digitalWrite(in2,LOW);
}

ISR(TIMER1_COMPA_vect) // send to mobile at an interval of one second
{
  if(cm1<=200)
  {
   Serial3.print(cm1);
   Serial3.print(" cm    ");
  }
  else
  {
    Serial3.println("Out of range");
  }
  
  count=count/20;
  Serial3.print(count);
  Serial3.print(" rps ");
  
  mps=count*10/100; // rotation per second * wheel circumference (cm) 
  Serial3.print(mps);
  Serial3.print(" mps ");
  count=0;
} 
