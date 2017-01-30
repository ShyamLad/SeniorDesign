void setup() {     //does not return anything
  Serial.begin(9600);  // begin serial to computer



  Serial3.begin(9600);    // begin serial to BASE ARDUINO
  Serial1.begin(9600);    // begin serial to SD card

 
  delay(100);  //delay of .1 seconds so that you do not compile errors
  // find_red2();
  // setTime(7,0,0,1,1,1);    // (hr,min,sec,day,month,yr)
}

void loop() {
 //float results[4];

 //Relative_ang(results,270);
//
//getData(results); /* the getData function should take a 'char *' paramenter */
//
//Serial.println(results[0]);
//Serial.println(results[1]);
//Serial.println(results[2]);
//Serial.println(results[3]);
//Serial.println("------------------------------------");
//Serial.println("------------------------------------");
//Serial.println("2: %i", results[1]);
//Serial.println("3: %i", results[2]);
//Serial.println("4: %i", results[3]);
wheel_speeds(40,300);

delay(1000);
}


//void getData(float *dest)
//{
//    dest[0] = 1;
//    dest[1] = 2;
//    dest[2] = 3;
//    dest[3] = 4;
//}
//



void wheel_speeds(float tot_spd, float ang){
  float results[4];
  Relative_ang(results,ang);

  float Motor_2 = ((((-1/sqrt(3))*tan((results[0]/180)*M_PI))-1)*(tot_spd/(1/cos((results[0]/180)*M_PI))));
  float Motor_1 = -1*((((1/sqrt(3))*tan((results[0]/180)*M_PI))-1)*(tot_spd/(1/cos((results[0]/180)*M_PI))));  

  float Motor_Comp = .0018*pow(results[0],3) - 0.1019*pow(results[0],2) + 2.446*results[0] + 0.8571;

int a = abs(results[1])-1;
int b = abs(results[2])-1;
int c = abs(results[3])-1;

 float motors[3];
 motors[a]= (results[1]/(abs(results[1]))) * Motor_1;
 motors[b]= Motor_2;
 motors[c]= Motor_Comp;

//Serial.println(results[0]);
//Serial.println(results[1]);
//Serial.println(results[2]);
//Serial.println(results[3]);
//Serial.println("------------------------------------");
//Serial.println("--------------Motors---------------");
//Serial.println("------------------------------------");
//Serial.println(motors[0]);
//Serial.println(motors[1]);
//Serial.println(motors[2]);
//Serial.println("------------------------------------");
//Serial.println("--------------Results---------------");
//Serial.println("------------------------------------");

//   motospd(motors[0],motors[1],motors[2]); //At Min Speed, set compensator to 18. 
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
      ptr[1] = 3;
      ptr[2] = 2;
      ptr[3] = 1;      
       
    }
     else if(ang >= 330 && ang<= 360){
      new_ang = ang;
     //new_config = {new_ang, 3,2,1};
      ptr[0] = new_ang - 360;
      ptr[1] = 3;
      ptr[2] = 2;
      ptr[3] = 1;      
       
    }
    else if (ang > 30 && ang <= 90){
     new_ang = ang - 60;
    //new_config = {new_ang, -2,-1,3};
     ptr[0] = new_ang;
      ptr[1] = -2;
      ptr[2] = -1;
      ptr[3] = 3;
    }
    else if (ang > 90 && ang <= 150){
      new_ang = ang - 120;
     //new_config = {new_ang, 3,1,2};
      ptr[0] = new_ang;
      ptr[1] = 3;
      ptr[2] = 1;
      ptr[3] = 2;
    }
    else if(ang > 150 && ang<= 210){
      new_ang = ang - 180;
     // new_config = {new_ang, -3,-2,1};
      ptr[0] = new_ang;
      ptr[1] = -3;
      ptr[2] = -2;
      ptr[3] = 1;
    }
    else if(ang > 210 && ang<= 270){
      new_ang = ang - 240;
     // new_config = {new_ang, 2,1,3};
      ptr[0] = new_ang;
      ptr[1] = 2;
      ptr[2] = 1;
      ptr[3] = 3;
    }
    else if(ang > 270 && ang< 330){
      new_ang = ang - 300;
     // new_config = {new_ang, -3,-1,2};
      ptr[0] = new_ang;
      ptr[1] = -3;
      ptr[2] = -1;
      ptr[3] = 2;
     }
 
 
 return 0;
          
 
}

