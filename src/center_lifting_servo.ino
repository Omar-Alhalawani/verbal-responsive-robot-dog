#include <Servo.h>

Servo liftServo;

void setup() {
  Serial.begin(9600);
  liftServo.attach(11);  // Change this to your lift servo's pin
  Serial.println("Enter an angle between 0 and 180:");
}

void loop() {
  
  liftServo.write(20);
    
}
