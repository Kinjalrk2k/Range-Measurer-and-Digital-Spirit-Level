#include <LiquidCrystal.h>
#include <Wire.h>
#include <MPU6050.h>

/*  PINS  */
//  for Ultrasonic
const int trigPin = 9;
const int echoPin = 10;

//  for LCD
const int reg = 0;
const int ebl = 1;
const int D4 = 2;
const int D5 = 3;
const int D6 = 4;
const int D7 = 5;

// initialize the library with the numbers of the interface pins

LiquidCrystal lcd(0, 1, 2, 3, 4, 5); /// REGISTER SELECT PIN,ENABLE PIN,D4 PIN,D5 PIN, D6 PIN, D7 PIN

MPU6050 mpu;

// Timers
unsigned long timer = 0;
float timeStep = 0.01;

// Pitch, Roll and Yaw values
float pitch = 0;
float roll = 0;
float yaw = 0;

float calc_dist(){
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  float duration = pulseIn(echoPin, HIGH);
  
  // Calculating the distance
  float distance = duration*0.034/2;

  return distance;
}

float calc_angle(){
  timer = millis();

  // Read normalized values
  Vector norm = mpu.readNormalizeGyro();

  // Calculate Pitch, Roll and Yaw
  pitch = pitch + norm.YAxis * timeStep;
  roll = roll + norm.XAxis * timeStep;
  yaw = yaw + norm.ZAxis * timeStep;

  delay((timeStep*1000) - (millis() - timer));
  return pitch;  
}

void display_lcd(float dist, float ang){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dist: ");
  lcd.print(dist);
  lcd.print(" cm");
  lcd.setCursor(0, 1);
  lcd.print("Angle: ");
  lcd.print(ang);
  lcd.print((char)223);

  Serial.print("Distance: ");
  Serial.print(dist);
  Serial.print("\tAngle: ");
  Serial.println(ang);

  delay(750);
  lcd.setCursor(0, 0);
}

void setup() {
  // put your setup code here, to run once:

  lcd.begin(8, 2);
  
//  Initialize MPU6050
  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }
  
  // Calibrate gyroscope. The calibration must be at rest.
  // If you don't want calibrate, comment this line.
  mpu.calibrateGyro();

  // Set threshold sensivty. Default 3.
  // If you don't want use threshold, comment this line or set 0.
  mpu.setThreshold(3);

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
//  Serial.begin(9600); // Starts the serial communication
}

void loop() {
  // put your main code here, to run repeatedly:
  float dist = calc_dist();
  float ang = calc_angle();

  display_lcd(dist, ang);
 
  
}
