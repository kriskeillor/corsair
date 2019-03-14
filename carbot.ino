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
  if (leftRead == 0 && centRead == 1 && rightRead == 0) {
    forward();
  }
   if (leftRead==1 && centRead==0 && rightRead==0) {
    easyLeft();
  }

   if (leftRead==0 && centRead==0 && rightRead==1) {
    easyRight();
  }
  if (leftRead == 1 && centRead == 1 && rightRead == 0) {
    sharpLeft();
  }
  
  if (leftRead==0 && centRead==1 && rightRead==1)
    sharpRight();

  if (leftRead==1 && centRead==0 && rightRead==1) {
    leftRightBlack();
  }
  if (leftRead==1 && centRead==1 && rightRead==1) {
    trackFind();
  }
  
  //just added this to track last reads 
  lastLeft = leftRead;
  lastCent = centRead;
  lastRight = rightRead;
}

void sharpLeft() {
  leftServo.write(90);
  rightServo.write(0);
  Serial.print("Sharp left turn");
}

void easyLeft() {
  leftServo.write(95);
  rightServo.write(5);
  Serial.print("Easy left turn");
}

void forward() {
  leftServo.write(180);
  rightServo.write(0);
  Serial.print("Straight forward, full speed");
}

void sharpRight() {
  leftServo.write(190);
  rightServo.write(85);
  Serial.print("Sharp Right turn");
}
void trackFind() {
  leftServo.write(145);
  rightServo.write(90);
  Serial.print("Track Found");
}
void leftRightBlack() {
  leftServo.write(80);
  rightServo.write(100);
  Serial.print("Wierd shits happening, slowing down");
}
void easyRight() {
  leftServo.write(175);
  rightServo.write(90);
  Serial.print("Easy Right turn");
}
