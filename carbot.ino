#include <Servo.h> 
#define LEFT_SERVO_PIN   3 
#define RIGHT_SERVO_PIN  4 
#define LEFT_SNS         5
#define CENT_SNS         6
#define RIGHT_SNS        7
Servo leftServo;
Servo rightServo;

float leftSns, centSns, rightSns;
bool onTrack;

void setup() {
  Serial.begin(9600);
  
  leftServo.attach(LEFT_SERVO_PIN);
  leftServo.write(90);
  rightServo.attach(RIGHT_SERVO_PIN);
  rightServo.write(90);

  pinMode(LEFT_SNS, INPUT);
  pinMode(CENT_SNS, INPUT);
  pinMode(RIGHT_SNS, INPUT);

  leftSns = 0.0;
  centSns = 0.0;
  rightSns = 0.0;
  onTrack = true;
}

void loop() {
  float snsScaler = 0.975f;
  int leftRead = digitalRead(LEFT_SNS);
  int centRead = digitalRead(CENT_SNS);
  int rightRead = digitalRead(RIGHT_SNS);
  
  String leftMsg = "Readout " + String(leftRead);
  String centMsg = String(centRead);
  String rightMsg = String(rightRead);
  String snsMsg = leftMsg + centMsg + rightMsg + "; ";
  
  if ((leftRead == 0 && centRead == 0 && rightRead == 0) or (leftRead == 1 && centRead == 0 && rightRead == 1)) {
    //if we are detecting a split or a wall, don't update SNS data 
    onTrack = false;
    snsScaler = 1.0f;
  }
  else {
    //average the last & current readout
    leftSns = (smoothstep(0, 1, leftSns) + (float)leftRead) / 2;
    centSns = (smoothstep(0, 1, centSns) + (float)centRead) / 2;
    rightSns = (smoothstep(0, 1, rightSns) + (float)rightRead) / 2;
    onTrack = true;
  }

  String leftMem = "Memory " + String(leftSns) + "/";
  String centMem = String(centSns) + "/";
  String rightMem = String(rightSns) + "; ";
  String memMsg = leftMem + centMem + rightMem;

  String revMsg = "";

  // basic idea is we get half throttle
  float leftScale, rightScale;
  if (leftSns > rightSns) {
    leftScale = (centSns * 3 - rightSns) / 4;
    rightScale = (centSns * 3 + leftSns) / 4;
  }
  else {
    leftScale = (centSns * 3 + rightSns) / 4;
    rightScale = (centSns * 3 - leftSns) / 4;
  }
/*  float leftScale = (centSns * 4 + rightSns * 3 - leftSns) / 8;
  float leftRev = 90 + 90 * smoothstep(0, 1, leftScale);
  float rightScale = (centSns * 4 - rightSns + leftSns * 3) / 8;
  float rightRev = 90 - 90 * smoothstep(0, 1, rightScale);*/
  
  //float leftScale = (centSns * 4 + rightSns * 3 - leftSns) / 8;
  float leftRev = 90 + 90 * smoothstep(0, 1, leftScale);
  //float rightScale = (centSns * 4 - rightSns + leftSns * 3) / 8;
  float rightRev = 90 - 90 * smoothstep(0, 1, rightScale);
  leftServo.write(leftRev);
  rightServo.write(rightRev);
  
  revMsg = "Revs L" + String(leftRev) + " / R" + String(rightRev);
  Serial.print(snsMsg + memMsg + revMsg + "\n");
  
  //wash out old sns data 
  leftSns *= snsScaler;
  centSns *= snsScaler;
  rightSns *= snsScaler;
}

float smoothstep(float edge0, float edge1, float x) {
  // Scale, bias and saturate x to 0..1 range
  x = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0); 
  // Evaluate polynomial
  return x * x * (3 - 2 * x);
}

float clamp(float x, float lowerlimit, float upperlimit) {
  if (x < lowerlimit)
    x = lowerlimit;
  if (x > upperlimit)
    x = upperlimit;
  return x;
}
