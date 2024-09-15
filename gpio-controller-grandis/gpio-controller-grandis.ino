
const int circlePin = 2;
const int trianglePin = 3;
const int xPin = 4;
const int squarePin = 5;
const int r1Pin = 6;
const int r2Pin = 7;
const int l1Pin = 8;
const int l2Pin = 9;
const int dpadRightPin = 10;
const int dpadDownPin = 11;
const int dpadUpPin = 12;
const int dpadLeftPin = 13;

const int joystick1XPin = A0;
const int joystick1YPin = A1;
const int joystick1ButtonPin = A2;
const int joystick2XPin = A3;
const int joystick2YPin = A4;
const int joystick2ButtonPin = A5;

void setup() {
  pinMode(circlePin, INPUT_PULLUP);
  pinMode(trianglePin, INPUT_PULLUP);
  pinMode(xPin, INPUT_PULLUP);
  pinMode(squarePin, INPUT_PULLUP);
  pinMode(r1Pin, INPUT_PULLUP);
  pinMode(r2Pin, INPUT_PULLUP);
  pinMode(l1Pin, INPUT_PULLUP);
  pinMode(l2Pin, INPUT_PULLUP);
  pinMode(dpadRightPin, INPUT_PULLUP);
  pinMode(dpadDownPin, INPUT_PULLUP);
  pinMode(dpadUpPin, INPUT_PULLUP);
  pinMode(dpadLeftPin, INPUT_PULLUP);

  pinMode(joystick1ButtonPin, INPUT_PULLUP);
  pinMode(joystick2ButtonPin, INPUT_PULLUP);

  Serial.begin(9600);
}

void loop() {
  int circleState = digitalRead(circlePin);
  int triangleState = digitalRead(trianglePin);
  int xState = digitalRead(xPin);
  int squareState = digitalRead(squarePin);
  int r1State = digitalRead(r1Pin);
  int r2State = digitalRead(r2Pin);
  int l1State = digitalRead(l1Pin);
  int l2State = digitalRead(l2Pin);
  int dpadRightState = digitalRead(dpadRightPin);
  int dpadDownState = digitalRead(dpadDownPin);
  int dpadUpState = digitalRead(dpadUpPin);
  int dpadLeftState = digitalRead(dpadLeftPin);
  int joystick1ButtonState = digitalRead(joystick1ButtonPin);
  int joystick2ButtonState = digitalRead(joystick2ButtonPin);

  int joystick1XValue = analogRead(joystick1XPin);
  int joystick1YValue = analogRead(joystick1YPin);
  int joystick2XValue = analogRead(joystick2XPin);
  int joystick2YValue = analogRead(joystick2YPin);

  Serial.print(circleState == LOW ? "0" : "1");
  Serial.print(" ");
  Serial.print(triangleState == LOW ? "0" : "1");
  Serial.print(" ");
  Serial.print(xState == LOW ? "0" : "1");
  Serial.print(" ");
  Serial.print(squareState == LOW ? "0" : "1");
  Serial.print(" ");
  Serial.print(r1State == LOW ? "0" : "1");
  Serial.print(" ");
  Serial.print(r2State == LOW ? "0" : "1");
  Serial.print(" ");
  Serial.print(l1State == LOW ? "0" : "1");
  Serial.print(" ");
  Serial.print(l2State == LOW ? "0" : "1");
  Serial.print(" ");
  Serial.print(dpadRightState == LOW ? "0" : "1");
  Serial.print(" ");
  Serial.print(dpadDownState == LOW ? "0" : "1");
  Serial.print(" ");
  Serial.print(dpadUpState == LOW ? "0" : "1");
  Serial.print(" ");
  Serial.print(dpadLeftState == LOW ? "0" : "1");
  Serial.print(" ");
  Serial.print(joystick1XValue);
  Serial.print(" ");
  Serial.print(joystick1YValue);
  Serial.print(" ");
  Serial.print(joystick1ButtonState == LOW ? "0" : "1");
  Serial.print(" ");
  Serial.print(joystick2XValue);
  Serial.print(" ");
  Serial.print(joystick2YValue);
  Serial.print(" ");
  Serial.println(joystick2ButtonState == LOW ? "0" : "1");

  delay(200);
}
