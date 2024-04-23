#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#include "RTClib.h"
#include <DHT.h>
#include <DHT_U.h>
#include <IRremote.h>
RTC_DS3231 RTC;

// defines pins numbers
const int trigPin = 11;
const int echoPin = 10;

// defines variables
long duration;
int distance1 = 0;
int distance2 = 0;
double Speed = 0;
int distance = 0;

// RGB Define
int redPin = 4;
int greenPin = 5;
int bluePin = 6;

// IR
IRrecv irrecv(8);
decode_results results;
#define ir0 16738455
#define ir1 16724175
#define ir2 16718055

// Mode Set
volatile int mode = 0;

// Joystick
int XValue = 0;
int YValue = 0;

// Motor
#include <Stepper.h>
const int stepsPerRevolution = 2048;
Stepper myStepper = Stepper(stepsPerRevolution, 31, 35, 33, 37);

// DHT
#define DHTPIN 26  // DHT Pin

#define DHTTYPE DHT11  // DHT 11

DHT_Unified dht(DHTPIN, DHTTYPE);

// Servo
#include <Servo.h>
Servo myServo;  //ประกาศตัวแปรแทน Servo


// Buzzer
#define BUZZERPIN 22

// Water Level
#define WATERPIN A3
int waterVal = 0;

void setup() {
  lcd.begin();
  pinMode(trigPin, OUTPUT);  // Sets the trigPin as an Output
  pinMode(echoPin, INPUT);   // Sets the echoPin as an Input
  pinMode(7, OUTPUT);
  // lcd.backlight();
  Serial.begin(9600);  // Starts the serial communication
  // Time Setup
  RTC.begin();
  RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // DHT
  dht.begin();

  // RGB Led
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  setColor(0, 255, 0);

  // IR
  irrecv.enableIRIn();

  // Interrupt
  attachInterrupt(digitalPinToInterrupt(3), inter, RISING);

  // Motor Setup
  myStepper.setSpeed(5);

  // Servo
  myServo.attach(13);  // กำหนดขา 9 ควบคุม Servo

  // Buzzer
  pinMode(BUZZERPIN, OUTPUT);

  // Water Level
  pinMode(WATERPIN, INPUT);

  // JOY Stick
  pinMode(53, INPUT_PULLUP);
}

void loop() {
  int servoValue;
  DateTime now = RTC.now();  // Set Time to current time
  //calculating Speed
  distance1 = ultrasonicRead();  //calls ultrasoninicRead() function below

  delay(1000);  //giving a time gap of 1 sec

  distance2 = ultrasonicRead();  //calls ultrasoninicRead() function below

  //formula change in distance divided by change in time
  Speed = (distance2 - distance1) / 1.0;  //as the time gap is 1 sec we divide it by 1.

  //Displaying Speed
  Serial.print("Speed in cm/s  :");
  Serial.println(Speed);
  // lcd.setCursor(0, 1);
  // lcd.print("Speed  cm/s ");
  // lcd.print(Speed);

  // Display Time
  lcd.setCursor(0, 0);
  print2digits(now.hour());
  lcd.print(':');
  print2digits(now.minute());
  lcd.print(':');
  print2digits(now.second());
  lcd.print(" ");

  // Get Temp
  sensors_event_t event;
  // Get temperature event and print its value.
  dht.temperature().getEvent(&event);

  // Displaying Temp
  lcd.print(event.temperature);
  lcd.print((char)223);
  lcd.print("C");

  // IR
  if (irrecv.decode(&results)) {
    Serial.print("รหัสปุ่มกด: ");
    Serial.println(results.value);
    lcd.setCursor(0, 1);
    lcd.print("                ");
    if (results.value == ir0) {
      mode = 0;
    }
    if (results.value == ir1) {
      mode = 1;
    }
    if (results.value == ir2) {
      mode = 2;
    }
    irrecv.resume();  // Receive the next value
  }

  // Water Level
  waterVal = analogRead(WATERPIN);
  Serial.println(waterVal);

  if (waterVal > 400) {
    mode = 4;
  }

  // Joystick
  XValue = analogRead(A0);
  YValue = analogRead(A1);
  int SWValue = digitalRead(53);

  Serial.print("XValue: ");
  Serial.print(XValue);
  Serial.print("\t");
  Serial.print("YValue: ");
  Serial.print(YValue);
  Serial.print("\n");

  if (SWValue == 0) {
    myServo.write(90);
  }
  int servoPos = map(analogRead(A1), 0, 1023, 0, 180);  // Map joystick Y-axis value to servo position (0-180)

  while (XValue > 600) {
    XValue = analogRead(A0);
    myStepper.step(16);
  }

  while (XValue < 400) {
    XValue = analogRead(A0);
    myStepper.step(-16);
  }

  if (YValue < 480) {
    myServo.write(servoPos);
  }
  if (YValue > 540) {
    myServo.write(servoPos);
  }

  if (mode == 0) {

    lcd.setCursor(0, 1);
    lcd.print("Mode 0 : Auto");
    // LED indicator
    if (distance >= 20) {
      // No Traffic At al
      setColor(0, 255, 0);
    }
    if (distance < 20 && Speed < 3) {
      // Heavy Traffic
      setColor(255, 0, 0);
    }
    if (distance < 20 && Speed > 3 && Speed < 12) {
      // Moderate Traffic
      setColor(229, 83, 0);
    }
    if (Speed >= 12) {
      // Low Traffic
      setColor(0, 255, 0);
    }
  }
  if (mode == 1) {
    lcd.setCursor(0, 1);
    lcd.print("Mode 1 : Green");
    setColor(0, 255, 0);
  }

  if (mode == 2) {
    lcd.setCursor(0, 1);
    lcd.print("Mode 2 : Red");
    setColor(255, 0, 0);
  }

  if (mode == 4) {
    Serial.println("Interrupt");
    lcd.setCursor(0, 1);
    lcd.print("Mode 4 : Error");
    setColor(0, 0, 0);
    digitalWrite(BUZZERPIN,HIGH);
    delay(500);
    digitalWrite(BUZZERPIN,LOW);
  }
}

float ultrasonicRead() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  //calculating distance
  distance = duration * 0.034 / 2;

  // Prints the distance on the Serial Monitor
  Serial.print("Distance in cm : ");
  Serial.println(distance);
  // lcd.setCursor(0, 0);
  // lcd.print("Dist. in cm ");
  // lcd.print(distance);
  // lcd.print("   ");
  return distance;
}

void print2digits(int number) {
  if (number >= 0 && number < 10) { lcd.print('0'); }
  lcd.print(number, DEC);
}


void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
}

void inter() {
  mode = 4;
}