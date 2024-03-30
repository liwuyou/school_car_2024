#include <Servo.h> 
#include <MsTimer2.h>

Servo myservo;  
                          
int pos = 70;   //舵机中值，对于我的舵机左->右 40-100
int lift = 20;
int right = 120;

// 舵机PWM引脚9，左电机PWM引脚5,右电机PWM引脚6；
int servo = 9;  int motor1=5;  int motor2=6;
int motor1_speed =60; int motor2_speed =60;

// 设置对管状态
int white = 0;
int black = 1;


int err,last_err; //pid 参数定义
int output;
float kp=10,ki=0,kd=0;
float P,I,D;

// 红外对管左->右引脚为2,3,4,7,8,12,13
int infrared[7]={2,3,4,7,8,12,13};

//实例特判
int zhijiao = 0;
int hu = 0;
int hu2 = 0;
int hu2_flag =0;
int final =0;


void flash() {
  final =1;
}


void setup() 
{ 
  myservo.attach(9);  // 该舵机由arduino第九脚控制
  myservo.write(pos);

  //电机pwm引脚初始化，模式设置为输出模式
  pinMode(motor1,OUTPUT);  
  pinMode(motor2,OUTPUT);
  pinMode(10,OUTPUT);

  Serial.begin(9600);//串口初始化
  //红外对管引脚模式设置为输入上拉模式
  pinMode(infrared[0],INPUT_PULLUP);
  pinMode(infrared[1],INPUT_PULLUP);
  pinMode(infrared[2],INPUT_PULLUP);
  pinMode(infrared[3],INPUT_PULLUP);
  pinMode(infrared[4],INPUT_PULLUP);
  pinMode(infrared[5],INPUT_PULLUP);
  pinMode(infrared[6],INPUT_PULLUP);

  //初始速度
  analogWrite(motor1,motor1_speed);
  analogWrite(motor2,motor2_speed);
}
void loop() 
{ 
  if(zhijiao == 0){
    zhijiao = 1;
    analogWrite(motor2,80);
    delay(1800);
    myservo.write(40);
    analogWrite(motor2,100);  
    delay(1000); 
  }
  if((hu == 0 && zhijiao == 1 && digitalRead(infrared[0])==black  && digitalRead(infrared[3])==black) || (hu == 0 && zhijiao == 1 && digitalRead(infrared[1])==black  && digitalRead(infrared[3])==black)){
    hu = 1;
    analogWrite(5,0);  
    analogWrite(6,0);
    analogWrite(11,100);
    myservo.write(70);
    delay(1800); 

    myservo.write(30);
    analogWrite(11,0);
    analogWrite(5,100);  
    analogWrite(6,100);
    delay(1500);

    
  }

  if(final==1 && digitalRead(infrared[1])==black && digitalRead(infrared[2])==black && digitalRead(infrared[3])==black){
  analogWrite(5,0);  
  analogWrite(6,0);
  delay(5500);     
  }

  if(hu==1  && hu2==0 && digitalRead(infrared[1])==black && digitalRead(infrared[2])==black && digitalRead(infrared[3])==black){
    hu2 = 1;
    analogWrite(5,0);  
    analogWrite(6,0);
    analogWrite(11,140);
    myservo.write(90);
    delay(1500); 
    myservo.write(30);
    analogWrite(11,0);
    analogWrite(5,120);  
    analogWrite(6,120);
    delay(1000);
    motor1_speed =70;motor2_speed =70;    
    MsTimer2::set(25000, flash); // 5000ms period
    MsTimer2::start();
  }
  sensor();
  int control_turn =pos + pid();
  myservo.write(control_turn);
  if(err == 4){
    analogWrite(motor1,motor1_speed+60);
  }
  else if(err == -4){
    analogWrite(motor2,motor2_speed+60);
  }
  else if(err == 2){
    analogWrite(motor1,motor1_speed+20);
    if(hu==1  && hu2==0){
      analogWrite(motor1,motor1_speed+40);
    }
  }
  else if(err == -2){
    analogWrite(motor2,motor2_speed+20);
    if(hu==1  && hu2==0){
      analogWrite(motor2,motor2_speed+40);
    }
  }
  else{
  analogWrite(motor1,motor1_speed);
  analogWrite(motor2,motor2_speed);    
  }
}

int pid(){
  P = err;
  I += err;
  D = err - last_err;
  output = kp*P + ki*I + kd*D;
  last_err = err;
  return output;
}

void sensor(){
  int a_1 = digitalRead(infrared[0]);
  int a_2 = digitalRead(infrared[1]);
  int a_3 = digitalRead(infrared[2]);
  int a_4 = digitalRead(infrared[3]);
  int a_5 = digitalRead(infrared[4]);
  int a_6 = digitalRead(infrared[5]);
  int a_7 = digitalRead(infrared[6]);
  if(a_1==black && a_2==white && a_3==white && a_4==white && a_5==white && a_6==white && a_7==white){
      err = -4;
  }
  else if(a_1==white && a_2==black && a_3==white && a_4==white && a_5==white && a_6==white && a_7==white){
      err = -2;
  }
  else if(a_1==white && a_2==white && a_3==black && a_4==white && a_5==white && a_6==white && a_7==white){
      err = -1;
  }
  else if(a_1==white && a_2==white && a_3==white && a_4==black && a_5==white && a_6==white && a_7==white){
      err = 0;
  } 
  else if(a_1==white && a_2==white && a_3==white && a_4==white && a_5==black && a_6==white && a_7==white){
      err = 1;
  }
  else if(a_1==white && a_2==white && a_3==white && a_4==white && a_5==white && a_6==black && a_7==white){
      err = 2;
  }  
  else if(a_1==white && a_2==white && a_3==white && a_4==white && a_5==white && a_6==white && a_7==black){
      err = 4;
  }
  Serial.println(err);  //测试代码
}