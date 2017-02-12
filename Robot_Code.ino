//#include <Array.h>

#include <SoftwareSerial.h>
#include <SPI.h>
#include <Pixy.h>
#include <NewPing.h>
#include <Math.h>
#include <Time.h>
#include <SD.h>

#define X_CENTER 160L

int wheel_pwm[] = {2,3,4}; //pins on motor drive to control speed
int wheel_direction[] = {5,6,7}; //pins to control wheel direction
double wheel_angles[] = {270, 30, 150}; //wheel angles are defined
double wheel_speed[3]; //speed of each wheel is stored in this array
double max_speed = 70; //maximum speed
double max_speedR = 40; //rotate speed

//create a new instance of Pixy
Pixy pixy;

double cm[2]; //sensed distance of sonar sensor : 2 represents array(2 sonar sensors)

int trigger[6] = {30, 32, 46, 36, 38, 40}; //trigger pins assignment  (sends)   
int echo[6] = {31, 33, 47, 37, 39, 41}; //echo pins (receives)

int colliDistance_cm = 25; //distances to define collision in cm/in
int colliDistance_in = 12;

//set up the sonar sensors for NewPing Library
NewPing sonar[6] =
{
  NewPing(trigger[0], echo[0], 500), //500 represents type of sensor
  NewPing(trigger[1], echo[1], 500),
  NewPing(trigger[2], echo[2], 500),
  NewPing(trigger[3], echo[3], 500),
  NewPing(trigger[4], echo[4], 500),
  NewPing(trigger[5], echo[5], 500),
};

int IR_proximity_pins[] = {A0, A1, A2, A3}; // pins for the proximity detection
int IR_proximity_state[4];              // state of IR pins

double solar[9]; //solar vectors are stored here
double solar_angles[] = {290, 330, 10, 50, 90, 130, 170, 210, 250}; //solar angles defined                                       
int solar_pins[] = {A5, A7, A8, A9, A10, A12, A13, A14, A15}; // solar pins
//analog pins assigned to solar panels

boolean light1_state = false;
boolean light2_state = false;
boolean water_state = false;
boolean CO2_state = false;

const int motor_relay = 8;   //does not want relay to change.

void setup() {     //does not return anything
  Serial.begin(9600);  // begin serial to computer

  for (int i = 0; i < 3; i++) {
    pinMode(wheel_direction[i], OUTPUT); //wheel directions are OUTPUT
  }
  for (int i = 0; i < 4; i++) { //i++ for adding
    pinMode(IR_proximity_pins[i], INPUT);
  }
  pinMode(motor_relay, OUTPUT);
  digitalWrite(motor_relay, HIGH);

  Serial3.begin(9600);    // begin serial to BASE ARDUINO
  Serial1.begin(9600);    // begin serial to SD card

  pixy.init();  // initialize pixy camera library functions
  delay(100);  //delay of .1 seconds so that you do not compile errors
  // find_red2();
  // setTime(7,0,0,1,1,1);    // (hr,min,sec,day,month,yr)

}
/*
  #include <SoftwareSerial.h>

  SoftwareSerial mySerial(0, 1); // RX, TX

  void setup()
  {
  // Open serial communications and wait for port to open:
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  Serial.println("Goodnight moon!");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(4800);
  mySerial.println("Hello, world?");
  }

  void loop() // run over and over
  {
  if (mySerial.available())
     Serial.write(mySerial.read());
  if (Serial.available())
     mySerial.write(Serial.read());
  }
*/
void loop() {
 // motospd(5,5,5);
//Pixy_Align();
//updateSonar();
sonarInfoDisplay();
//Do_Thingy();
//Go_To_Object();
//wheel_speeds(40,270);
//motospd(0,0,40);
  delay(1000);
}
void Do_Thingy(){
  boolean aligned = false;
  Serial.print(Am_I_Close());
  if(!aligned){
    Pixy_Align();
    aligned = true;
  }
  else if (!Am_I_Close()){
    Go_To_Object();
  }
  else{
    return;
  }
}
void Go_To_Object(){
  if(Am_I_Close()){
    motospd(0,0,0);
  }
  else{
    motospd(0,40,-40);
  }
  delay(100);
    
}
boolean Am_I_Close(){
   unsigned int uS = sonar[2].ping();
    cm[1] = sonar[1].convert_cm(uS);
    cm[2] = sonar[2].convert_cm(uS);
    Serial.print(cm[1]);
    Serial.print(cm[2]);
    if (cm[1] <=30 && cm[1] != 0 || cm[2] <= 30 && cm[2] != 0){
      return true;
    }
    else{
      return false;
    }
    
}
void Pixy_Align()
{
  Serial.println("-------------------------------------------------");
  Serial.println("-------------------------------------------------");
  get_coordinates();
  int rotation_update_delay = 10;
  int tolerance = 15;
  int blocks = pixy.getBlocks(17);
  if (pixy.blocks[0].x < (160 - tolerance) )
  {
    Serial.println("Rotating CoutnerClockwise...");
    motospd(20, 20, 20);
    delay(rotation_update_delay);
    Pixy_Align();
  }
  else if (pixy.blocks[0].x > (160 + tolerance) )
  {
    Serial.println("Rotating Clockwise...");
    motospd(-20, -20, -20);
    delay(rotation_update_delay);
    Pixy_Align();
  }
  else {
//    Pixy_finetune();
 Serial.println("pixy has been aligned");
    motospd(0, 0, 0);
    return;
  }


}

void Pixy_finetune() {

  Serial.println("------------||FINE TUNE IN PROGRESS||-----------");
  Serial.println("-------------------------------------------------");
  get_coordinates();
  int rotation_update_delay = 10;
  int tolerance = 5;
  int spd = 10;
  int blocks = pixy.getBlocks(17);
  if (pixy.blocks[0].x < (160 - tolerance) )
  {
    Serial.println("Rotating CoutnerClockwise...");
    motospd(spd, spd, spd);
    delay(rotation_update_delay);
    Pixy_finetune();
  }
  else if (pixy.blocks[0].x > (160 + tolerance) )
  {
    Serial.println("Rotating Clockwise...");
    motospd(-1 * spd, -1 * spd, -1 * spd);
    delay(rotation_update_delay);
    Pixy_finetune();
  }
  else {
    Serial.println("pixy has been aligned");
    motospd(0, 0, 0);
    return;
  }

}

void get_coordinates()
{

  int blocks = pixy.getBlocks(17);
  for (int k = 0; k < blocks; k++) {
    if (pixy.blocks[k].signature == 1) {
      Serial.print("The X coordinate is:");
      Serial.print(pixy.blocks[k].x);
      Serial.println();
      Serial.print("The Y coordinate is:");
      Serial.print(pixy.blocks[k].y);
      Serial.println();

    }
  }

}
void wheel_speeds(float tot_spd, float ang){   //Input speed and direction you want to go from 0 to 360
  float results[4]; //establish the results arrray for relative angle.
  Relative_ang(results,ang); //returns relative angle from -30 to 30 and wheels. First two wheels are main wheels, last wheel compensates for directional movement

  float Motor_2 = abs(((((-1/sqrt(3))*tan((results[0]/180)*M_PI))-1)*(tot_spd/(1/cos((results[0]/180)*M_PI))))); //maths
  float Motor_1 = abs(((((1/sqrt(3))*tan((results[0]/180)*M_PI))-1)*(tot_spd/(1/cos((results[0]/180)*M_PI)))));  //more maths

  float Motor_Comp = .0018*pow(results[0],3) - 0.1019*pow(results[0],2) + 2.446*results[0] + 0.8571; //equaiton for compensator found by experiment and excel curve fit



//----------------------------------------------------------------------------------------
//READ ME!!!!!!!
//
//What is done below may look confusing, and it is to an extent. This method was done in order to not have a bunch of if/else statements
//
// The general idea: Relative_ang() gives values to the array called results. results is an array where the first position is the relative angle and the following
// three are motors, with the first two being main working motors and the last being a compensator for the direction
// 
// What we need to do is assign each of these wheels the proper wheel speed. Using the knowledge that the first two motors listed in the results array are the
// main motors, and that the motors are called (+/-)1,2,3.....we can give the proper value to the proper motor. Since motospd(...,...,...) takes in three values that
// are always in order from wheel 1 to 3,  we must assign the wheels from results an array value and then assign the proper speed at that specific array value and 
  // asign it to an array that reorganizes the wheels so motospd can distribute the proper speed to the proper wheel....... this is a shitty explanation pls forgive me
  //
  //
int a = abs(results[1])-1; //since the wheels are labled from 1 to 3, we subtract one to give it a proper position in the motors array below
int b = abs(results[2])-1; // e.g. results[2] = wheel 3.......3-1 = 2.... 'b' will be in the position of motors[2]
int c = abs(results[3])-1;

 float motors[3]; // establish the final wheel speed array to set wheels
 motors[a]= (results[1]/(abs(results[1]))) * Motor_1;  //(results[1]/(abs(results[1]))) is to determine the directon of the wheel rotation (positive or negative)
 motors[b]= (results[2]/(abs(results[2]))) * Motor_2;  
 motors[c]= (results[3]/(abs(results[3]))) * Motor_Comp; // so since 'c' corresponds to the value of the wheel in results[3], the compensator value is assigned.
 // this does not mean motor[c] will always be the thrid value in motors. if the compensator wheel is determined to be wheel 2, then 2-1 = 1, and motors[1] will be assinged
 // the compenator value, which is the second value in the array for this case. 

Serial.println(results[0]);
Serial.println(results[1]);
Serial.println(results[2]);
Serial.println(results[3]);
Serial.println("------------------------------------");
Serial.println("--------------Motors---------------");
Serial.println("------------------------------------");
Serial.println(motors[0]);
Serial.println(motors[1]);
Serial.println(motors[2]);
Serial.println("------------------------------------");
Serial.println("--------------Results---------------");
Serial.println("------------------------------------");

 motospd(motors[0],motors[1],motors[2]); //set all the wheel seeds
}


void Relative_ang(float *ptr, float ang){
//float a,b,c,d;
// float new_config[4]={0,0,0,0};
// float *ptr[4];
 float new_ang;
 
   
    if(ang >= 0 && ang<= 30){
      new_ang = ang;
     //new_config = {new_ang, 3,2,1};
      ptr[0] = new_ang;
      ptr[1] = -3;
      ptr[2] = 2;
      ptr[3] = -1;      
       
    }
     else if(ang > 330 && ang<= 360){
      new_ang = ang;
     //new_config = {new_ang, 3,2,1};
      ptr[0] = new_ang - 360;
      ptr[1] = -3;
      ptr[2] = 2;
      ptr[3] = 1;      
       
    }
    else if (ang > 30 && ang <= 90){
     new_ang = ang - 60;
    //new_config = {new_ang, -2,-1,3};
     ptr[0] = new_ang;
      ptr[1] = 2;
      ptr[2] = -1;
      ptr[3] = -1*(new_ang/abs(new_ang))*3;
    }
    else if (ang > 90 && ang <= 150){
      new_ang = ang - 120;
     //new_config = {new_ang, 3,1,2};
      ptr[0] = new_ang;
      ptr[1] = 3;
      ptr[2] = -1;
      ptr[3] = -1*(new_ang/abs(new_ang))*2;
    }
    else if(ang > 150 && ang<= 210){
      new_ang = ang - 180;
     // new_config = {new_ang, -3,-2,1};
      ptr[0] = new_ang;
      ptr[1] = 3;
      ptr[2] = -2;
      ptr[3] = -1*(new_ang/abs(new_ang))*1;
    }
    else if(ang > 210 && ang<= 270){
      new_ang = ang - 240;
     // new_config = {new_ang, 2,1,3};
      ptr[0] = new_ang;
      ptr[1] = -2;
      ptr[2] = 1;
      ptr[3] = -1*(new_ang/abs(new_ang))*3;
    }
    else if(ang > 270 && ang<= 330){
      new_ang = ang - 300;
     // new_config = {new_ang, -3,-1,2};
      ptr[0] = new_ang;
      ptr[1] = -3;
      ptr[2] = 1;
      ptr[3] = -1*(new_ang/abs(new_ang))*2;
     }
 
 
 return 0;
          
 
}

//float Relative_ang(float ang){
// //float a,b,c,d;
// float new_config[4]={0,0,0,0};
// float *ptr[4];
//  float new_ang;
//  
//    
//     if((ang > 330 && ang<= 360) || (ang >= 0 && ang<= 30)){
//       new_ang = ang;
//      new_config = {new_ang, 3,2,1};
//        a= new_ang;
//        
//        
//     }
//     else if (ang > 30 && ang <= 90){
//      new_ang = ang + 300;
//     new_config = {new_ang, -2,-1,3};
//     }
//     else if (ang > 90 && ang <= 150){
//       new_ang = ang + 240;
//      new_config = {new_ang, 3,1,2};
//     }
//     else if(ang > 150 && ang<= 210){
//       new_ang = ang + 180;
//      new_config = {new_ang, -3,-2,1};
//     }
//     else if(ang > 210 && ang<= 270){
//       new_ang = ang + 120;
//      new_config = {new_ang, 2,1,3};
//     }
//     else if(ang > 270 && ang<= 330){
//       new_ang = ang + 60;
//      new_config = {new_ang, -3,-1,2};
//      }
//  
//  for(int i = 0; i < 4; i++){
//    ptr[i] = &new_config[i]; // assign the address of integer.
//  }
//  return **ptr;
//           
//  
//}



void Lets_Move(double ang, int tot_spd, int comp){
float Motor_2;
float Motor_1;
  
  Motor_2 = ((((-1/sqrt(3))*tan((ang/180)*M_PI))-1)*(tot_spd/(1/cos((ang/180)*M_PI))));
  Motor_1 = -1*((((1/sqrt(3))*tan((ang/180)*M_PI))-1)*(tot_spd/(1/cos((ang/180)*M_PI))));
  Serial.println("-----------------------");
  Serial.println("-----------------------");
  Serial.print("Motor 1 Speed:");
  Serial.println(Motor_1);
  Serial.print("Motor 2 Speed:");
  Serial.println(Motor_2);

 motospd(comp,Motor_1,Motor_2); //At Min Speed, set compensator to 18. 
 delay(1000);
}

// Just a mini test cycle to try and visualize each station without having to wait
// the actual time it takes for lighting or getting CO2 or whatever its doing
void test_cycle() {
  delay(2000);   //waits 2 seconds
  getWater(1);   //gets water for a minute
  delay(1000);   //waits 10 seconds
  Serial.println("preparing to measure moisture");
  Serial1.write('M');
  delay(1000);   //waits 1 second
  delay(1000);                  //turn on lights
  Serial3.println("7H2");
  delay(3000); //waits 3 seconds
  light1_state = true;
  getSun();
  Serial3.println("7H1");
  delay(120000);
  Serial3.println("7H2");
  delay(3000);
  light1_state = true;
  getSun();
  Serial3.println("7H1");
  delay(120000);
  getCO2(1);

}

// cycle for the entire day
void day_cycle() {
  getWater(1);
  delay(500);
  Serial.println("preparing to measure moisture");
  Serial1.write('M');

  // measure after getting water, then again 2 full cycles later
  for (int i = 0; i < 7; i++) {
    delay(100);
    mini_cycle();
    delay(100);
    if (i == 2) {
      Serial.println("preparing to measure moisture");
      Serial1.write('M');
    }
    delay(100);
  }
}

//run the full 5 light cycle then get CO2
void mini_cycle() {
  light_cycle();
  delay(2000);
  getCO2(30);
  delay(500);
}

// single light cycle just gets light once then sits
void light_cycle() { //cycling light
  for (int i = 0; i < 5; i++) {
    light_loop();
    delay(1800000);       //no sun for 30 min
    motospd(30, 30, 30);  // after getting in a lit position
    delay(1100);          // rotate a little after each cycle
    motospd(0, 0, 0);     // to try and be more distributed
  }
}

void light_loop() {
  delay(50);                  //turn on lights
  Serial3.println("7H3");
  delay(3000);
  light1_state = true;   //light is on now because it is true
  getSun();
  Serial3.println("7H5");
  delay(300000);          //get sun for 5 min
}

void getWater(int water_time) { //time to get water in minutes
  find_red1();   //indicator for detecting
  delay(1000);
  String msg = "5H";
  msg = msg + water_time;
  Serial3.println(msg);
  water_state = true;
  delay(water_time * 30000);
  Serial3.println("5L");
  delay(2000);
  water_state = false;
  delay(2000);
  backAwayFromRed();
}

void getCO2(int CO2_time) {   // time to sit in CO2 cage in minutes
  find_green1();
  delay(1000);
  String msg = "4H";
  msg = msg + CO2_time;
  Serial3.println(msg);  //writes to arduino
  CO2_state = true;
  delay(60000);
  delay(CO2_time * 60000);
  delay(2000);
  delay(60000);
  CO2_state = false;
  backAwayFromGreen();
}

void find_red1() {   // parent function to call recursive functions and talk to BASE
  // turn on the lights temporarily for tracking
  Serial.println("Searching for red");
  delay(1000);
  Serial3.println("6H3");
  delay(50);
  light2_state = true;

  digitalWrite(motor_relay, LOW); //turn motors on
  delay(250);

  find_red2();       // locate, center, and approach target
  horizontalCenter();// horiztonally center along the wall
  adjustRed();       // adjust distance to wall to line up with water faucet

  digitalWrite(motor_relay, HIGH); //turn motors off
  delay(100);
  Serial3.println("6L");
  delay(1000);
}

void find_red2() {
  motospd(20, 20, 20);    // spin until red is detected
  locate_red();
  Serial.println("red detected");
  /*
    centerObject2(1);    // center with target

    delay(50);

    static int i = 0;  //only can be changed outside the global scope; can't be changed in a for loop
    int j;
    uint16_t blocks;   //allocated integer of 16 bits
    char buf[32];    //character
    int32_t xError;
    blocks = pixy.getBlocks(17);

    int height = pixy.blocks[0].height;;
    Serial.print("height:  ");
    Serial.println(height); //prints the height of the variable height

    double dist_away = 1866.0 / height;    // use height to get distance
    //distance away in inches

    if (dist_away > 100 || dist_away < 0) {      // throw out error when pixy picks up
     motospd(max_speedR,max_speedR,max_speedR); // small background objects
     delay(500);
     motospd(0,0,0);  //not moving
     find_red2();
     return;
    }
    Serial.print("distance away:  ");
    Serial.println(dist_away);

    pixyRun2(dist_away, 1);    //approach target
    delay(10);*/
}

void locate_red() {    //spin and refresh camera until red is detected
  static int i = 0;
  int j;
  uint16_t blocks;
  char buf[32];
  int32_t xError;
  blocks = pixy.getBlocks(17);

  for (int k = 0; k < blocks; k++) {
    Serial.println("for loop started");
    if (pixy.blocks[k].signature == 1) {
      Serial.println("found block");
      delay(500);
      motospd(0, 0, 0);
      delay(5000);
      return;
    }
  }
  delay(50);
  locate_red();
}

void backAwayFromRed() {     // back away from red wall before performing next task
  digitalWrite(motor_relay, LOW); //low means on in this case
  delay(250);

  motospd(0, -max_speed, max_speed);
  delay(2000);
  motospd(0, 0, 0);

  digitalWrite(motor_relay, HIGH);
}

void pixyAlignRed() {     //align straight with the red target
  Serial.println("Aligning with object");
  updateSonar();
  delay(100);
  updateSonar();
  sonarInfoDisplay();

  if (abs(cm[0] - cm[1]) < 2) { //spin left or right based on sonar values
    Serial.println("Aligned");
    return;
  } else if (cm[0] > cm[1]) {
    motospd(40, 10, 10);
    delay(250);
    motospd(0, 0, 0);
    delay(100);
    pixyAlignRed();
  } else {
    motospd(-40, -10, -10);
    delay(250);
    motospd(0, 0, 0);
    delay(100);
    pixyAlignRed();
  }
}

void adjustRed() {   // adjust distance from red to line up with water faucet
  Serial.println("adjusting");
  updateSonar();
  delay(100);
  updateSonar();

  double adjust = 24 - ((cm[0] + cm[1]) / 2.0);
  Serial.print("adjust distance: ");
  Serial.println(adjust);

  if (adjust > 0) {  // move forward or backward based on sonar
    motospd(0, -max_speed, max_speed);
    double adjustTime = adjust / 2.54 / 11 * 1000;
    delay(adjustTime);
    motospd(0, 0, 0);
  } else {
    adjust = -adjust;
    motospd(0, max_speed, -max_speed);
    double adjustTime = adjust / 2.54 / 11 * 1000;
    delay(adjustTime);
    motospd(0, 0, 0);
  }
}

void horizontalCenter() {   // center left or right along the wall to line up with water
  Serial.println("horizontally center/align to object");

  static int i = 0;
  int j;
  uint16_t blocks;
  char buf[32];
  int32_t xError;
  blocks = pixy.getBlocks(17);

  int center = pixy.blocks[0].x;    // use pixy x-axis center function to line it up
  Serial.print("center: ");
  Serial.println(center);

  if (abs(center - 150) < 5) {
    Serial.println("hor. centered with the object - ready for dist. adjust");
    return;
  } else if (center < 150) {
    Serial.println("hor. center - moving left");
    for (int i = 0; i < 3; i++) {
      wheel_speed[i] = max_speed * newCos(wheel_angles[i] - 90);
    }
    motorWrite();
    delay(200);
    motospd(0, 0, 0);
    delay(1000);
    horizontalCenter();
  } else {
    Serial.println("hor. center - moving right");
    for (int i = 0; i < 3; i++) {
      wheel_speed[i] = max_speed * newCos(wheel_angles[i] - 270);
    }
    motorWrite();
    delay(200);
    motospd(0, 0, 0);
    delay(1000);
    horizontalCenter();
  }

}

void find_green1() {   // parent function to call recursive functions and talk to BASE
  delay(1500);           // turn on lights temporarily for color tracking
  Serial3.println("6H5");
  delay(1000);
  light2_state = true;

  digitalWrite(motor_relay, LOW);
  delay(250);

  find_green2();       // locate, center, and approach green
  horizontalCenterGreen();   // hor line up to object before adjust
  adjustGreen();       // adjust distance to green to line up with CO2 cage

  digitalWrite(motor_relay, HIGH);
  delay(100);
  Serial3.println("6L");
  delay(1000);
}

//recursive function to narrow in on the green
void find_green2() {
  motospd(-max_speedR, -max_speedR, -max_speedR);    //spin until green is detected
  delay(300);
  locate_green();
  Serial.println("green detected");


  centerObject2(3);    // center with green

  delay(50);

  static int i = 0;
  int j;
  uint16_t blocks;
  char buf[32];
  int32_t xError;
  blocks = pixy.getBlocks(17);

  int height;
  //height = pixy.blocks[0].height;

  if (blocks == 1) {
    height = pixy.blocks[0].height;
  } else {
    height = pixy.blocks[1].height;
  }

  Serial.print("height:  ");
  Serial.println(height);

  double dist_away = 1800.0 / height;    // use height to find distance away
  //distance away in inches

  if (dist_away > 100 || dist_away < 0) {  //get rid of background noise detection errors
    find_green2();
    return;
  }
  Serial.print("distance away:  ");
  Serial.println(dist_away);

  pixyRun2(dist_away, 3);    // approach target
  delay(10);
}

void locate_green() {  //spin and refresh camera until green is detected
  static int i = 0;
  int j;
  uint16_t blocks;
  char buf[32];
  int32_t xError;
  blocks = pixy.getBlocks(17);

  for (int k = 0; k < blocks; k++) {
    if (pixy.blocks[k].signature == 3) {
      delay(300);
      motospd(0, 0, 0);
      return;
    }
  }
  delay(50);
  locate_green();
}

void centerObject2(int color_num) {    // recursive center with target

  Serial.println("centering");
  delay(50);

  static int i = 0;
  int j;
  uint16_t blocks;
  char buf[32];

  blocks = pixy.getBlocks(17);

  int block_num;
  int error;  //error if you do not input 1 or 0
  if (blocks) {

    if (color_num == 1) {
      block_num = 0;
    } else {
      if (blocks == 1) {
        block_num = 0;
      } else {
        block_num = 1;
      }
    }

    int height = pixy.blocks[block_num].height;
    if (height < 10) {
      if (color_num == 1) {
        find_red2();
      } else {
        find_green2();
      }
      return;
    }

    uint32_t center = pixy.blocks[block_num].x;    // use x-axis center function to align
    error = X_CENTER - center;                     // robot with target
    Serial.print("center: ");
    Serial.print(center);
    Serial.print("   error: ");
    Serial.println(error);
  }

  if (abs(error) < 10) {
    Serial.println("centered");    // <10 is good enough alignment with margin of error
    return;
  }

  int spin_time = abs(error) * 4.5;    // spin time based on size of center error
  Serial.print("Spin Time:  ");
  Serial.println(spin_time);

  if (error >= 0) {
    motospd(max_speedR, max_speedR, max_speedR);
  } else {
    motospd(-max_speedR, -max_speedR, -max_speedR);
  }
  delay(spin_time);
  motospd(0, 0, 0);

  centerObject2(color_num);    //recursive call to improve accuracy
}

void pixyRun2(double dist_away, int color_num) {   //approach target
  if (anyCollision(5)) {  // if theres a collision, move away
    getAwayFromWall();
    if (color_num == 1) {
      find_red2();
      return;
    } else if (color_num == 3) {
      find_green2();
      return;
    }
  }
  if (color_num == 1) {    // distance for red is 15 in
    if (dist_away < 15) {
      pixyAlignRed();
      return;
    }
  } else if (color_num == 3) { //distance for green is 20 in
    if (dist_away < 20) {
      pixyAlignGreen();
      return;
    }
  }

  double approach_time = dist_away / 11 * 1000.0;  // approach time based on distance
  motospd(0, max_speed, -max_speed);
  if (color_num == 1) {
    delay(approach_time / 3.0);    // move part of the way
  } else if (color_num == 3) {
    delay(approach_time / 3.0);
  }
  motospd(0, 0, 0);
  delay(10);

  if (color_num == 3) {  //recursive call to continue process
    delay(50);
    find_green2();
  } else if (color_num == 1) {
    delay(50);
    find_red2();
  }
}

void backAwayFromGreen() {
  digitalWrite(motor_relay, LOW);
  delay(250);

  motospd(0, -max_speed, max_speed);
  delay(4500);
  motospd(0, 0, 0);

  digitalWrite(motor_relay, HIGH);
}

void pixyAlignGreen() { //align robot with green to line up with CO2 cage
  Serial.println("Aligning with object");
  updateSonar();
  sonarInfoDisplay();

  if (abs(cm[0] - cm[1]) < 3) { // move left or right based on sonar
    return;
  } else if (cm[0] > cm[1]) {
    motospd(40, 10, 10);
    delay(300);
    motospd(0, 0, 0);
    delay(100);
    pixyAlignGreen();
  } else {
    motospd(-40, -10, -10);
    delay(300);
    motospd(0, 0, 0);
    delay(100);
    pixyAlignGreen();
  }
}

void horizontalCenterGreen() {   // center left/right along the wall to line up with water
  Serial.println("horizontally center/align to object");

  static int i = 0;
  int j;
  uint16_t blocks;
  char buf[32];
  int32_t xError;
  blocks = pixy.getBlocks(17);

  int center = pixy.blocks[0].x;    // use pixy x-axis center function to line it up
  Serial.print("center: ");
  Serial.println(center);

  if (abs(center - 150) < 5) {
    Serial.println("hor. centered with the object - ready for dist. adjust");
    return;
  } else if (center < 150) {
    Serial.println("hor. center - moving left");
    for (int i = 0; i < 3; i++) {
      wheel_speed[i] = max_speed * newCos(wheel_angles[i] - 90);
    }
    motorWrite();
    delay(200);
    motospd(0, 0, 0);
    delay(1000);
    horizontalCenterGreen();
  } else {
    Serial.println("hor. center - moving right");
    for (int i = 0; i < 3; i++) {
      wheel_speed[i] = max_speed * newCos(wheel_angles[i] - 270);
    }
    motorWrite();
    delay(200);
    motospd(0, 0, 0);
    delay(1000);
    horizontalCenterGreen();
  }
}


void adjustGreen() {   // adjust distance to green to line up with CO2 cage
  updateSonar();

  double adjust = 48 - ((cm[0] + cm[1]) / 2.0);
  if (adjust > 0) {  // move forward or backward based on sonar
    motospd(0, -max_speed, max_speed);
    double adjustTime = adjust / 2.54 / 11 * 1000;
    delay(adjustTime);
    motospd(0, 0, 0);
  } else {
    adjust = -adjust;
    motospd(0, max_speed, -max_speed);
    double adjustTime = adjust / 2.54 / 11 * 1000;
    delay(adjustTime);
    motospd(0, 0, 0);
  }
}

void getSun() {  // find light
  digitalWrite(motor_relay, LOW); //turn on the motor

  updateSolar();
  delay(100);
  solarInfoDisplay();

  if (evenSun()) {  // if its evenly distributed in the sun then its done
    digitalWrite(motor_relay, HIGH);
    return;
  }

  double solar_angle = getSolarAngle();  //get angle of light
  Serial.print("Solar Angle:  ");
  Serial.println(solar_angle);

  delay(2000);

  rotateToLight2(solar_angle);  //rotate "front" of robot towards that solar angle

  solarRun(solar_angle);    // approach light

}

//calculate the solar angle in degree by
//using all 3 solar vectors the outcome
//ranges from 0degree to 360degree
double getSolarAngle() {
  double x = 0;
  double y = 0;
  for (int i = 0; i < 9; i++) {
    x += solar[i] * newCos(solar_angles[i]);
    y += solar[i] * newSin(solar_angles[i]);
  }
  return newAtan(x, y);
}

// see if all the solar sensors are within range of each other
// to see if its evenly distributed
boolean evenSun() {
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 9; j++) {
      if (abs(solar[i] - solar[j]) > 250) {
        return false;
      }
    }
  }
  return true;
}

void rotateToLight(double solar_angle) {   //rotate towards the given angle

  double rotationAngle = solar_angle - 90;
  if (rotationAngle < 0) {
    rotationAngle += 360;
  }

  double spinTime = rotationAngle * 13000 / 360;
  Serial.print("spin time :");
  Serial.println(spinTime);

  motospd(max_speedR, max_speedR, max_speedR);
  delay(spinTime);
  motospd(0, 0, 0);

  if (anyCollision(25)) {
    motospd(0, -max_speed, max_speed);
    delay(500);
    motospd(0, 0, 0);
  }
  delay(50);
}

void rotateToLight2(double solar_angle) {   // rotate to the solar angle
  if (solar_angle >= 90 && solar_angle <= 270) {  // if its in this half of the circle,
    double rotationAngle = solar_angle - 90;      // then rotate CCW

    double spinTime = rotationAngle * 13000 / 360;
    Serial.print("spin time :");
    Serial.println(spinTime);

    motospd(max_speedR, max_speedR, max_speedR);
    delay(spinTime);
    motospd(0, 0, 0);
  } else {               // if its in the other half of the circle, then rotate CW
    double rotationAngle;
    if (solar_angle < 90) {
      rotationAngle = 90 - solar_angle;
    } else {
      rotationAngle = 90 + (360 - solar_angle);
    }
    double spinTime = rotationAngle * 13000 / 360;
    Serial.print("spin time :");
    Serial.println(spinTime);

    motospd(-max_speedR, -max_speedR, -max_speedR);
    delay(spinTime);
    motospd(0, 0, 0);
  }

  if (anyCollision(25)) {  //check for collision before moving forward
    motospd(0, -max_speed, max_speed);        // and move away if needed
    delay(500);
    motospd(0, 0, 0);
  }
  delay(50);
}

void solarRun(double solar_angle) {   // movement in the light

  if (anyCollision(20)) {  //check for collision before moving
    Serial.println("Moving away from wall");
    getAwayFromWall();
  }
  boolean done = false;
  updateSolar();
  solarInfoDisplay();
  for (int i = 0; i < 9; i++) {
    if (solar[i] < 300) {
      Serial.println("Moving towards the light");
      solarRun2();               // solarRun2 moves toward the lighted area
      done = true;               // if some sensors are too low
      break;
    }
  }
  if (!done) {
    Serial.println("Attempting to center in the light");
    solarRun3(solar_angle);  //already in the lighted are, so solarRun3
  }                          //tries to center itself better in the light
}

// move towards the lighted area
void solarRun2() {

  updateSonar();
  if (anyCollision(25)) {
    getAwayFromWall();
  }
  motospd(0, max_speed, -max_speed); // its already facing the right direction
  delay(1000);                     // so it just moves forward
  motospd(0, 0, 0);

  delay(2000);

  getSun();    //recursive call to improve accuracy
}

void solarRun3(double solar_angle) { // centering in the lighted area
  int min = 0;
  for (int i = 1; i < 9; i++) { // find the smallest light sensor
    if (solar[i] < solar[min]) {
      min = i;
    }
  }
  double back_theta = solar_angles[min] - 180; //move away from the smallest one
  for (int i = 0; i < 3; i++) {
    wheel_speed[i] = max_speed * newCos(wheel_angles[i] - back_theta);
  }
  motorWrite();
  delay(1000);
  motospd(0, 0, 0);

  getSun();    // recursive call
}

boolean anyCollision(int distance_cm) { // check for a collision within a certain distance
  updateSonar();                        // using the sonar in the front
  sonarInfoDisplay();
  for (int i = 0; i < 2; i++) {
    if (cm[i] < distance_cm && cm[i] > 1) {
      return true;
    }
  }
  return false;
}

boolean anyCollision2() {   //check for a close range collision using the IR sensors
  updateIRprox();
  delay(50);
  for (int i = 0; i < 4; i++) {
    if (IR_proximity_state[i] < 524) {
      return true;
    }
  }
  return false;
}

void getAwayFromWall() {   //uses the IR sensors to move away from a detected obstacle
  int sensorNum = colliSensorNumFeedback();
  colliFeedbackMotion(sensorNum);
}

//give the number of the sensor which detected the obstacle
int colliSensorNumFeedback() {
  delay(50);
  updateIRprox();
  for (int i = 0; i < 4; i++) {
    if (IR_proximity_state[i] < 300) {
      Serial.print("backward_sensor:");
      Serial.print(i);
      Serial.println( );
      return i;
    }
  }
  Serial.println("No_backward_sensor");
  return -1;
}

// uses the number of the obstacle sensor to move away from that direction
void colliFeedbackMotion(int sensor_num) {
  int num = sensor_num;
  if (num == 0) {
    motospd(-max_speed, 0, max_speed);
    delay(2000); //backward 2s
    motospd(0, 0, 0);
    delay(10);
  } else if (num == 1 || num == 2) {
    motospd(0, -max_speed, max_speed);
    delay(2000); //backward 2s
    motospd(0, 0, 0);
    delay(10);
  } else if (num == 3) {
    motospd(max_speed, -max_speed, 0);
    delay(2000); //backward 2s
    motospd(0, 0, 0);
    delay(10);
  } else {
    return;
  }
}

//writes motor speed values to motor
// used to move in obscure directions
void motorWrite() {
  for (int i = 0; i < 3; i++) {
    if (wheel_speed[i] < 0) {
      digitalWrite(wheel_direction[i], LOW);
    } else {
      digitalWrite(wheel_direction[i], HIGH);
    }
    analogWrite(wheel_pwm[i], abs(wheel_speed[i]));
  }
}

//direct way to assign values to motors
//used for easy angles
void motospd(int sp1, int sp2, int sp3) {
  if (sp1 > 0) {
    digitalWrite(wheel_direction[0], HIGH);
  } else {
    digitalWrite(wheel_direction[0], LOW);
  }
  if (sp2 > 0) {
    digitalWrite(wheel_direction[1], HIGH);
  } else {
    digitalWrite(wheel_direction[1], LOW);
  }
  if (sp3 > 0) {
    digitalWrite(wheel_direction[2], HIGH);
  } else {
    digitalWrite(wheel_direction[2], LOW);
  }
  analogWrite(wheel_pwm[0], abs(sp1));
  analogWrite(wheel_pwm[1], abs(sp2));
  analogWrite(wheel_pwm[2], abs(sp3));
}

//display updated wheel speed values
void speedInfoDisplay() {
  Serial.print("Speed Display: ");
  for (int i = 0; i < 3; i++) {
    Serial.print(wheel_speed[i]);
    Serial.print("|");
  }
  Serial.println(" ");
  Serial.println(" ");
  delay(10);
}

//math tool
//return the cosine value for an angle in degree
double newCos(double phi) {
  return cos(phi * M_PI / 180);
}

//return the sine value for an angle in degree
double newSin(double phi) {
  return sin(phi * M_PI / 180);
}

//return the tan value when you input y and x;
//the outcome ranges from 0 to 360 degrees
double newAtan(double x, double y) {
  double theta = atan(y / x) * 180 / M_PI;
  if (x < 0) {
    return theta + 180;
  } else if (y < 0) {
    return theta + 360;
  } else {
    return theta;
  }
}

//update solar panel values
void updateSolar() {
  for (int i = 0; i < 9; i++) {
    solar[i] = analogRead(solar_pins[i]);
    delay(50);
  }
}

//display updated solar panel values
void solarInfoDisplay() {
  Serial.println("Solar Display: ");
  for (int i = 0; i < 9; i++) {
    Serial.print("Solar Pin ");
    Serial.print(i);
    Serial.print(" : ");
    Serial.print(solar[i]);
    Serial.print("     ");
    delay(50);

    Serial.println();
  }
}

//update sonar sensor values in terms of distance
void updateSonar() {
  for (int i = 0; i < 6; i++) {
    //cm[i]=sonar[i].ping_cm();
    unsigned int uS = sonar[i].ping();
    cm[i] = sonar[i].convert_cm(uS);
    delay(50);

    Serial.println(cm[i]);



  }
}

//display updated sonar sensor values
void sonarInfoDisplay() {
  Serial.println("Distance Display: ");
  for (int i = 0; i < 6; i++) {
    Serial.print("Sonar Pin ");
    Serial.print(i);
    Serial.print(" : ");
    Serial.println(cm[i]);
  }
  delay(50);
}

// update IR sensor states
void updateIRprox() {
  for (int i = 0; i < 4; i++) {
    IR_proximity_state[i] = analogRead(IR_proximity_pins[i]);
    delay(50);
  }
  delay(100);
}

// print updates IR sensor values
void IRProxInfoDisplay() {
  Serial.println("Proximity Display: ");
  for (int i = 0; i < 4; i++) {
    Serial.print("IR Pin ");
    Serial.print(i);
    Serial.print(" : ");
    Serial.println(IR_proximity_state[i]);
  }
  delay(100);
}


