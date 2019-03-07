#include <Servo.h> 
#define LEFT_SERVO_PIN   3 
#define RIGHT_SERVO_PIN  4 
#define LEFT_SNS         5
#define CENT_SNS         6
#define RIGHT_SNS        7
Servo leftServo;
Servo rightServo;

int lastLeft, lastCent, lastRight;

void setup() {
  Serial.begin(9600);
  
  leftServo.attach(LEFT_SERVO_PIN);
  leftServo.write(90);
  rightServo.attach(RIGHT_SERVO_PIN);
  rightServo.write(90);

  pinMode(LEFT_SNS, INPUT);
  pinMode(CENT_SNS, INPUT);
  pinMode(RIGHT_SNS, INPUT);

  lastLeft = 0;
  lastCent = 0;
  lastRight = 0;
}

void loop() {
  int leftRead = digitalRead(LEFT_SNS);
  int centRead = digitalRead(CENT_SNS);
  int rightRead = digitalRead(RIGHT_SNS);

  String leftSns = "Left Sns " + String(leftRead);
  String centSns = ", Cent Sns " + String(centRead);
  String rightSns = ", Right Sns " + String(rightRead);
  Serial.print(leftSns + centSns + rightSns + "\n");

  if (leftRead == 0 && centRead == 0 && rightRead == 0) {
      leftRead = lastLeft;
      centRead = lastCent;
      rightRead = lastRight;
  }

  if (leftRead) {
    sharpRight();
    lastLeft = 1;
    lastCent = 0;
    lastRight = 0;
  }
  else if (centRead) {
    forward();
    lastLeft = 0;
    lastCent = 1;
    lastRight = 0;
  }
  else if (rightRead) {
    easyLeft();
    lastLeft = 0;
    lastCent = 0;
    lastRight = 1;
  }
  else {
    // idle to give my head a break 
    leftServo.write(90);
    rightServo.write(90);
  }
}

void sharpLeft() {
  leftServo.write(90);
  rightServo.write(0);
  Serial.print("Sharp left turn");
}

void easyLeft() {
  leftServo.write(95);
  rightServo.write(0);
  Serial.print("Easy left turn");
}

void forward() {
  leftServo.write(110);
  rightServo.write(70);
  Serial.print("Straight forward, full speed");
}

void sharpRight() {
  leftServo.write(145);
  rightServo.write(80);
  Serial.print("Sharp Right turn");
}

void easyRight() {
  leftServo.write(180);
  rightServo.write(80);
  Serial.print("Easy Right turn");
}
