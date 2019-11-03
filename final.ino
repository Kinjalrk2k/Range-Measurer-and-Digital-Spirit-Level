#include <LiquidCrystal.h>
#include <Wire.h>
#include <MPU6050.h>

const int MPU_addr=0x68;
int16_t axis_X,axis_Y,axis_Z;
int minVal=265;
int maxVal=402;
double x;
double y;
double z;

float error = 0;

/*  PINS  */
//  for Ultrasonic
const int trigPin = 3;
const int echoPin = 4;

/*  for LCD */
const int reg = 0;
const int ebl = 1;
const int D4 = 8;
const int D5 = 9;
const int D6 = 10;
const int D7 = 11;

const int swt = 13;

// initialize the LCD library with the numbers of the interface pins
LiquidCrystal lcd(reg, ebl, D4, D5, D6, D7); // REGISTER SELECT PIN, ENABLE PIN, D4 PIN, D5 PIN, D6 PIN, D7 PIN

//  initialize a MPU6050 object for the gyro
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
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);
  axis_X=Wire.read()<<8|Wire.read();
  axis_Y=Wire.read()<<8|Wire.read();
  axis_Z=Wire.read()<<8|Wire.read();
  int xAng = map(axis_X,minVal,maxVal,-90,90);
  int yAng = map(axis_Y,minVal,maxVal,-90,90);
  int zAng = map(axis_Z,minVal,maxVal,-90,90);
  x= RAD_TO_DEG * (atan2(-yAng, -zAng)+PI);
  y= RAD_TO_DEG * (atan2(-xAng, -zAng)+PI);
  z= RAD_TO_DEG * (atan2(-yAng, -xAng)+PI);
  return abs((360-z));  
}

void callibrate_gyro(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Callibrating");
  lcd.setCursor(0, 1);
  lcd.print("Please Wait!");
  delay(5000);
  error = calc_angle();
}

void display_lcd(float dist, float ang){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dist: ");
  lcd.print(dist);
  lcd.print(" cm");
  lcd.setCursor(0, 1);
  lcd.print("Angle: ");
  lcd.print(abs(ang));
  lcd.print((char)223); //  printing the degree symbol

  Serial.print("Distance: ");
  Serial.print(dist);
  Serial.print("\tAngle: ");
  Serial.println(abs(ang));
//  Serial.println((char)223);  //  printing the degree symbol

  delay(750);
  lcd.setCursor(0, 0);
}

void setup() {
  // put your setup code here, to run once:

  lcd.begin(8, 2);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Initializing");
  lcd.setCursor(0, 1);
  lcd.print("Please Wait!");
  
//  Initialize MPU6050
//  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
//  {
//    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
//    delay(500);
//  }
//  
//  // Calibrate gyroscope. The calibration must be at rest.
//  // If you don't want calibrate, comment this line.
//  mpu.calibrateGyro();
//
//  // Set threshold sensivty. Default 3.
//  // If you don't want use threshold, comment this line or set 0.
//  mpu.setThreshold(3);

  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(swt, INPUT);
//  Serial.begin(9600); // Starts the serial communication (cannot be used with LCD all together)

  
  callibrate_gyro();
}

void loop() {
  // put your main code here, to run repeatedly:

  if(digitalRead(swt) == 1){
    callibrate_gyro();
  }

//  Serial.print(digitalRead(swt));
  
  float dist = calc_dist();
  float ang = calc_angle();

  display_lcd(dist, ang - error);
}
