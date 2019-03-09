#include <Servo.h> 
#define LEFT_SERVO_PIN   3 
#define RIGHT_SERVO_PIN  4 
#define LEFT_SNS         5
#define CENT_SNS         6
#define RIGHT_SNS        7
Servo leftServo;
Servo rightServo;

float leftMem, centMem, rightMem;

void setup() {
  Serial.begin(9600);
  
  leftServo.attach(LEFT_SERVO_PIN);
  leftServo.write(90);
  rightServo.attach(RIGHT_SERVO_PIN);
  rightServo.write(90);

  pinMode(LEFT_SNS, INPUT);
  pinMode(CENT_SNS, INPUT);
  pinMode(RIGHT_SNS, INPUT);

  leftMem = 0.0;
  centMem = 0.0;
  rightMem = 0.0;
}

void loop() {
  int leftRead = digitalRead(LEFT_SNS);
  int centRead = digitalRead(CENT_SNS);
  int rightRead = digitalRead(RIGHT_SNS);

  if (leftRead == 0 && centRead == 0 && rightRead == 0) 
  {
    // if we're off track, buff up the last sns data
    if (leftMem > 0.25) leftRead = 1;
    if (centMem > 0.75) centRead = 1;
    if (rightMem > 0.25) rightRead = 1;
  } else {
    leftMem = (smoothstep(0, 1, leftMem) * 4 + (float)leftRead) / 5;
    centMem = (smoothstep(0, 1, centMem) * 4 + (float)centRead) / 5;
    rightMem = (smoothstep(0, 1, rightMem) * 4 + (float)rightRead) / 5;
  }
  
  String snsRead = ("Readout " + String(leftRead) + String(centRead) + String(rightRead) + "; ");
  
  String leftMem = "Memory " + String(leftMem) + "/";
  String centMem = String(centMem) + "/";
  String rightMem = String(rightMem) + "; ";
  String memMsg = leftMem + centMem + rightMem;
  Serial.print(snsRead + memMsg);
  
  if (leftRead == 0 && centRead == 0 && rightRead == 0) {
    // if we're STILL off track, reverse 
    trackFind();
  }

  // steering block follows 
  if (leftRead == 0 && centRead == 1 && rightRead == 0) 
    forward();
  
  if (leftRead==1 && centRead==0 && rightRead==0) 
    easyLeft();

  if (leftRead==0 && centRead==0 && rightRead==1) 
    easyRight();
    
  if (leftRead == 1 && centRead == 1 && rightRead == 0) 
    sharpLeft();
  
  if (leftRead==0 && centRead==1 && rightRead==1)
    sharpRight();

  if (leftRead==1 && centRead==0 && rightRead==1) 
    trackFindLeft();
  
  if (leftRead==1 && centRead==1 && rightRead==1) 
    trackFindLeft();
}

void sharpLeft() {
  writeRevs(90, 0);
}

void easyLeft() {
  float leftRevs = 95 + 10 * rightMem;
  float rightRevs = 75 - 20 * leftMem;
  writeRevs(leftRevs, rightRevs);
}

void forward() {
  float leftRevs = 100 + 20 * leftMem;
  float rightRevs = 80 - 20 * rightMem;
  writeRevs(leftRevs, rightRevs);
}

void sharpRight() {
  writeRevs(180, 85);
}
void trackFind() {
  writeRevs(95, 60);
}

void trackFindLeft() {
  writeRevs(135, 85);
}
void Reverse() {
  writeRevs(80, 100);
}
void easyRight() {
  float leftRevs = 105 + 20 * rightMem;
  float rightRevs = 85 - 10 * leftMem;
  writeRevs(leftRevs, rightRevs);
}

void writeRevs(float L, float R) {
  leftServo.write(L);
  rightServo.write(R);
  Serial.print("Revs L" + String(L) + " / R" + String(R) + "\n");
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
