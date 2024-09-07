
// Pin assignments
const int button1Pin = 4; // Button 1
const int button2Pin = 5; // Button 2
const int button3Pin = 6; // Button 3
const int button4Pin = 7; // Button 4

const int joystickXPin = A0;     // Joystick X axis (Analog)
const int joystickYPin = A1;     // Joystick Y axis (Analog)
const int joystickButtonPin = 2; // Joystick button (Digital)

// Variables to store button states
int button1State = 0;
int button2State = 0;
int button3State = 0;
int button4State = 0;

int joystickButtonState = 0; // Joystick button state
int joystickXValue = 0;      // Joystick X axis value
int joystickYValue = 0;      // Joystick Y axis value

void setup() {
  // Set button pins as input
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(button3Pin, INPUT_PULLUP);
  pinMode(button4Pin, INPUT_PULLUP);

  // Set joystick button pin as input
  pinMode(joystickButtonPin, INPUT_PULLUP);

  // Initialize serial communication for debugging or sending data
  Serial.begin(9600);
}

void loop() {
  // Read button states (LOW = pressed)
  button1State = digitalRead(button1Pin);
  button2State = digitalRead(button2Pin);
  button3State = digitalRead(button3Pin);
  button4State = digitalRead(button4Pin);

  // Read joystick button state (LOW = pressed)
  joystickButtonState = digitalRead(joystickButtonPin);

  // Read joystick X and Y values (0 to 1023 range)
  joystickXValue = analogRead(joystickXPin);
  joystickYValue = analogRead(joystickYPin);

  // Print data in a consistent format
  Serial.print(button1State == LOW ? "0" : "1");
  Serial.print(" ");
  Serial.print(button2State == LOW ? "0" : "1");
  Serial.print(" ");
  Serial.print(button3State == LOW ? "0" : "1");
  Serial.print(" ");
  Serial.print(button4State == LOW ? "0" : "1");
  Serial.print(" ");
  Serial.print(joystickXValue);
  Serial.print(" ");
  Serial.print(joystickYValue);
  Serial.print(" ");
  Serial.println(joystickButtonState == LOW ? "0" : "1");

  // Add a short delay to avoid flooding the serial output
  delay(100);
}
