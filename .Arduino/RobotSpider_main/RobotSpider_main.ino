#include <string.h>
#include <Servo.h>
#include <Wire.h>

#define IDLE -1
#define MOVE 1

#define BACKWARD 0
#define FORWARD 1
#define LEFT 2
#define RIGHT 3

#define UP 2
#define DOWN 3
//============================================================
//                          STRUCTS
//============================================================
//servo struct
struct MyServo{
    int port;
    int pos;
    
    Servo serv;
  };

struct Leg{
    MyServo parts[2];
  };

int currentMode = IDLE;
//============================================================
//                          PROTOTYPES
//============================================================
  void initLegs();
  void legCalibration(Leg lg);
  void moveServ(MyServo my_servo, int new_angle);
  void moveLeg(Leg* lg, int for_back_up_down);
  
  void leftMove(Leg* legs);
  void rightMove(Leg* legs);
  void frontMove(Leg* legs);
  void backMove(Leg* legs);
  
  void allLegCalib(Leg* legs, int numLegs);

  void receiveEvent(int howMany);
//============================================================
//                        GLOBAL_DATA
//============================================================
// electron parts
Leg legs_dt[4];

// speed info
int legsDelay = 0.0;
int moveDelay = 0.05;
int angleSpeed = 1;

int steps = 0;
//============================================================
//                      MAIN_FUNCTIONS
//============================================================
void setup() {
  // init ports  
  Wire.begin(8);
  Wire.onReceive(receiveEvent);
  Serial.begin(115200);
    
  // init and calib leg
  initLegs(legs_dt, 4);
  allLegCalib(legs_dt, 4);

  delay(10000);
}

// INFINITY LOOP
void loop() {
  // test
  // rightMove(legs_dt);
  // leftMove(legs_dt);
  // frontMove(legs_dt);
  // backMove(legs_dt);

  // main code
  if(currentMode == FORWARD)
    frontMove(legs_dt);
  if(currentMode == BACKWARD)
    backMove(legs_dt);
  if(currentMode == RIGHT)
    rightMove(legs_dt);
  if(currentMode == LEFT)
    leftMove(legs_dt);

  if(currentMode != IDLE)
    if(steps == 3){
      currentMode = IDLE;
      steps = 0;
    }
    else
      ++steps;
}
//============================================================
//                      OTHER_FUNCTIONS
//============================================================
// initialyze legs
void initLegs(Leg* legs, int numLegs){
  int start_port = 22;
  int increment = 0;

  for(int i = 0; i < numLegs; ++i){
    legs[i].parts[0].serv.attach(start_port + increment);
    legs[i].parts[0].port = start_port + increment;
    legs[i].parts[0].pos = 0;

    legs[i].parts[1].serv.attach(start_port + increment + 1);
    legs[i].parts[1].port = start_port + increment + 1;
    legs[i].parts[1].pos = 90;

    increment += 2;
  }
}

// calibrates the legs (TEST)
void legCalibration(Leg lg){
  lg.parts[0].serv.write(0);
  lg.parts[1].serv.write(90);
}

// calibrates all legs
void allLegCalib(Leg* legs, int numLegs){
  for(int i = 0; i < numLegs; ++i)
      legCalibration(legs[i]);
}

// move leg function
void moveServ(MyServo* my_servo, int new_angle){
  int angle = (my_servo->pos - new_angle) / angleSpeed;
  int i = angleSpeed;

  while(i > 0){
    my_servo->serv.write(new_angle + (i * angle));
    delay(legsDelay * 1000);
    --i;
  }

  my_servo->serv.write(new_angle);
  my_servo->pos = new_angle;
  delay(legsDelay * 1000);
}

// legs movement logic
void moveLeg(Leg* lg, int for_back_up_down){
  // if forward
  if(for_back_up_down == BACKWARD){
      moveServ(&lg->parts[1], 135);
      delay(moveDelay * 1000);
    }
  if(for_back_up_down == FORWARD){ 
      moveServ(&lg->parts[1], 65);
      delay(moveDelay * 1000);
    }
   if(for_back_up_down == DOWN){
      moveServ(&lg->parts[0], 10);
      delay(moveDelay * 1000);
    }
   if(for_back_up_down == UP){
      moveServ(&lg->parts[0], 40);
      delay(moveDelay * 1000);
   }
}

void rightMove(Leg* legs){
  // first part
  moveLeg(&legs[0], UP);
  moveLeg(&legs[0], FORWARD);
  moveLeg(&legs[3], UP);
  moveLeg(&legs[3], BACKWARD);
  delay(200);
  moveLeg(&legs[0], DOWN);
  moveLeg(&legs[3], DOWN);
  delay(200);
  
  moveLeg(&legs[0], BACKWARD);
  moveLeg(&legs[3], FORWARD);
  delay(300);

  // second part
  moveLeg(&legs[1], UP);
  moveLeg(&legs[1], FORWARD);
  moveLeg(&legs[2], UP);
  moveLeg(&legs[2], BACKWARD);
  delay(200);
  moveLeg(&legs[1], DOWN);
  moveLeg(&legs[2], DOWN);
  delay(200);
  
  moveLeg(&legs[1], BACKWARD);
  moveLeg(&legs[2], FORWARD);
  delay(300);
}

void leftMove(Leg* legs){
  // first part
  moveLeg(&legs[0], UP);
  moveLeg(&legs[0], BACKWARD);
  moveLeg(&legs[3], UP);
  moveLeg(&legs[3], FORWARD);
  delay(200);
  moveLeg(&legs[0], DOWN);
  moveLeg(&legs[3], DOWN);
  delay(200);

  moveLeg(&legs[0], FORWARD);
  moveLeg(&legs[3], BACKWARD);
  delay(300);

  // second part
  moveLeg(&legs[1], UP);
  moveLeg(&legs[1], BACKWARD);
  moveLeg(&legs[2], UP);
  moveLeg(&legs[2], FORWARD);
  delay(200);
  moveLeg(&legs[1], DOWN);
  moveLeg(&legs[2], DOWN);
  delay(200);

  moveLeg(&legs[1], FORWARD);
  moveLeg(&legs[2], BACKWARD);
  delay(300);
  }
  
void frontMove(Leg* legs){
  // second part
  moveLeg(&legs[1], UP);
  moveLeg(&legs[1], BACKWARD);
  moveLeg(&legs[2], UP);
  moveLeg(&legs[2], FORWARD);
  delay(200);
  moveLeg(&legs[1], DOWN);
  moveLeg(&legs[2], DOWN);
  delay(200);
  
  moveLeg(&legs[1], FORWARD);
  moveLeg(&legs[2], BACKWARD);
  delay(300);

  // first part
  moveLeg(&legs[0], UP);
  moveLeg(&legs[0], FORWARD);
  moveLeg(&legs[3], UP);
  moveLeg(&legs[3], BACKWARD);
  delay(200);
  moveLeg(&legs[0], DOWN);
  moveLeg(&legs[3], DOWN);
  delay(200);
  
  moveLeg(&legs[0], BACKWARD);
  moveLeg(&legs[3], FORWARD);
  delay(300);
  }

 void backMove(Leg* legs){
  // second part
  moveLeg(&legs[1], UP);
  moveLeg(&legs[1], FORWARD);
  moveLeg(&legs[2], UP);
  moveLeg(&legs[2], BACKWARD);
  delay(200);
  moveLeg(&legs[1], DOWN);
  moveLeg(&legs[2], DOWN);
  delay(200);
  
  moveLeg(&legs[1], BACKWARD);
  moveLeg(&legs[2], FORWARD);
  delay(300);

  // first part
  moveLeg(&legs[0], UP);
  moveLeg(&legs[0], BACKWARD);
  moveLeg(&legs[3], UP);
  moveLeg(&legs[3], FORWARD);
  delay(200);
  moveLeg(&legs[0], DOWN);
  moveLeg(&legs[3], DOWN);
  delay(200);
  
  moveLeg(&legs[0], FORWARD);
  moveLeg(&legs[3], BACKWARD);
  delay(300);
  }
//============================================================
//                      WIFI_FUNCTIONS
//============================================================
// read info
void receiveEvent(int howMany) {
  String result = ""; 
 
 while (0 <Wire.available()) {
    char c = Wire.read();      /* получаем байт как символ*/
    result += c;
    Serial.print(c);           /* выводим символ в серийный порт */
  }
 Serial.println();             /* переходим на новую строку */

  if(result.indexOf('0') != -1)
    currentMode = BACKWARD; 
  if(result.indexOf('1') != -1)
    currentMode = FORWARD; 
  if(result.indexOf('2') != -1)
    currentMode = LEFT; 
  if(result.indexOf('3') != -1)
    currentMode = RIGHT; 

  Serial.println(currentMode);
}
