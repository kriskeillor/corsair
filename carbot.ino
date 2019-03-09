#include <Servo.h> 
#define LEFT_SERVO_PIN   3 
#define RIGHT_SERVO_PIN  4 
#define LEFT_SNS         5
#define CENT_SNS         6
#define RIGHT_SNS        7
Servo leftServo;
Servo rightServo;

float leftMem, centMem, rightMem;
int leftLast, centLast, rightLast;
bool offCourse;

void setup() {
  Serial.begin(9600);
  offCourse = false;
  
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
  // read sns
  int leftRead = digitalRead(LEFT_SNS);
  int centRead = digitalRead(CENT_SNS);
  int rightRead = digitalRead(RIGHT_SNS);

  // original sns readout 
  String snsRead = String(leftRead) + String(centRead) + String(rightRead);

  // if we're off track ... 
  if (leftRead == 0 && centRead == 0 && rightRead == 0) 
  {
    // we're only off course if we're veering off the left/right 
    // drifting from the center is fine 
    if (centLast == 0)
      offCourse = true;
    
    // replace the current sns readout with last update's readout   
    leftRead = leftLast;
    centRead = centLast;
    rightRead = rightLast;
    // interpolate sns memory, buffing up any sns that has both a semi-strong memory state and a hit last update 
    leftMem = smoothstep(0, 1, leftMem + 0.25 * leftRead);
    centMem = smoothstep(0, 1, centMem + 0.25 * centRead);
    rightMem = smoothstep(0, 1, rightMem + 0.25 * rightRead);
  } else {
    // interpolate sns memory 
    leftMem = smoothstep(0, 1, leftMem * 0.8 + (float)leftRead * 0.2);
    centMem = smoothstep(0, 1, centMem * 0.8 + (float)centRead * 0.2);
    rightMem = smoothstep(0, 1, rightMem * 0.8 + rightRead * 0.2);
    offCourse = false;
  }
  
  // print sns data 
  String snsTaken = "Readout " + snsRead + "->" + String(leftRead) + String(centRead) + String(rightRead) + "; ";
  String memMsg = "Memory " + String(leftMem) + "/" + String(centMem) + "/" + String(rightMem) + "; ";
  Serial.print(snsTaken + memMsg);
  
  // steering block follows 
  if (leftRead == 0 && centRead == 1 && rightRead == 0) 
    forward();
  
  if (leftRead==1 && centRead==0 && rightRead==0) {
    if (!offCourse)
      easyLeft();
    else 
      sharpLeft();
  }
  
  if (leftRead==0 && centRead==0 && rightRead==1) { 
    if (!offCourse)
      easyRight();
    else
      sharpRight();
  }
  
  if (leftRead == 1 && centRead == 1 && rightRead == 0) 
    sharpLeft();
  
  if (leftRead==0 && centRead==1 && rightRead==1)
    sharpRight();
  
  if (leftRead==1 && centRead==0 && rightRead==1) 
    easyLeft();
    //findTrackLeft();
  
  if (leftRead==1 && centRead==1 && rightRead==1) 
    easyLeft();
    //findTrackLeft();

  // store current sns data for next update 
  leftLast = leftRead;
  centLast = centRead;
  rightLast = rightRead;
}

void sharpLeft() {
  writeRevs(120 - 40 * leftMem, 0);
}

void easyLeft() {
  float leftRevs = 105 + 15 * rightMem;
  float rightRevs = 60 - 60 * leftMem;
  writeRevs(leftRevs, rightRevs);
}

void forward() {
  float leftRevs = 140 + 40 * rightMem + 20 * centMem - 10 * leftMem;
  float rightRevs = 40 - 40 * leftMem - 20 * centMem + 10 * rightMem;
  writeRevs(leftRevs, rightRevs);
}

void sharpRight() {
  writeRevs(180, 60 + 40 * rightMem);
}

void easyRight() {
  float leftRevs = 120 + 60 * rightMem;
  float rightRevs = 75 - 15 * leftMem;
  writeRevs(leftRevs, rightRevs);
}

void findTrackLeft() {
  writeRevs(105, 60);
}

void findTrackRight() {
  writeRevs(120, 75);
}

void reverse() {
  writeRevs(80, 100);
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
