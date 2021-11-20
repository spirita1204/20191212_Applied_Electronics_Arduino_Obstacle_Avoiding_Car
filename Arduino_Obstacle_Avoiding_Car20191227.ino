#include <Adafruit_NeoPixel.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define dirR 12//右馬達1
#define PwmR 10//ENA
#define dirL 11//左馬達3
#define PwmL 9//ENB
#define trigPin 6//超音波Pin
#define echoPin 5
#define servoPin A3//馬達 Pin
#define  SPK  8
int freq;
int inB;
int choose=1;

unsigned long d;//前方障礙物距離
const int leftSpeed=255;//左輪轉速
const int rightSpeed=255;//右輪轉速
LiquidCrystal_I2C lcd(0x27,16,2);   
Servo servo ;

Adafruit_NeoPixel   ring = Adafruit_NeoPixel(16, 7);
//呼吸燈
void breathing(int r, int g, int b) {
  float bright = (exp(sin(millis() / 2000.0 * PI)) - 0.36787944) * 108.0;
  bright = bright / 255;     // 先除 255, 然後下一行再乘以指定的 3 原色值
  for (int i = 0; i < ring.numPixels(); i++) {
    ring.setPixelColor(i, bright * r, bright * g, bright * b);
   }
  ring.show();
}


void cM(int dL,int dR,int sL,int sR){
  digitalWrite(dirL,dL);
  digitalWrite(dirR,dR);
  analogWrite(PwmL,sL);
  analogWrite(PwmR,sR);
}

void setup()
{
  Serial.begin(115200);
//  pinMode( DirL , OUTPUT);
//  pinMode( DirR , OUTPUT);
  cM(HIGH,HIGH,0,0);
  ring.begin();                   
  ring.setBrightness(32);           // 0~255
  ring.show();    
  lcd.init();
  lcd.backlight();
  pinMode(dirR,OUTPUT);//右馬達
  pinMode(dirL,OUTPUT);//左馬達
 //超音波腳位設定
  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);
 //超音波初始化
  servo.attach(servoPin);
  servo.write(95);

  //delay(1000) ;
  //forward();//自走車開始前進
  //Serial.begin(9600);
}
unsigned long ping_d() {
    digitalWrite(trigPin,HIGH) ;
    delayMicroseconds(1000) ;
    digitalWrite(trigPin,LOW);
    return ( pulseIn(echoPin,HIGH)/58) ;
}
 void forward() {
    //右馬達
    digitalWrite(dirR,HIGH);
    analogWrite(PwmR,rightSpeed);
    //左馬達
    digitalWrite(dirL,HIGH);
    analogWrite(PwmL,leftSpeed);
  }
    void motoStop(){
    //右馬達
    analogWrite(PwmR,0);
    //左馬達
    analogWrite(PwmL,0);
   }

void backward() {
   // 右馬達
    digitalWrite(dirR,LOW);
    analogWrite(PwmR,rightSpeed) ;
    // 左馬達
    digitalWrite(dirL,LOW);
    analogWrite(PwmL,leftSpeed) ;
  }

 void turnLeft(){
   // 右馬達 前進
    digitalWrite(dirR,HIGH);
    analogWrite(PwmR,rightSpeed) ;
    // 左馬達 後退
    digitalWrite(dirL,LOW);
    analogWrite(PwmL,leftSpeed) ;
  }     

void turnRight(){
   // 右馬達 前進
    digitalWrite(dirR,LOW);
    analogWrite(PwmR,rightSpeed) ;
    // 左馬達 後退
    digitalWrite(dirL,HIGH);
    analogWrite(PwmL,leftSpeed) ;
  }  
void loop()
{   
  if (Serial.available() > 0) {
     inB= Serial.read();
     if (inB=='0')
      {    
        choose=1;
        breathing(255, 0, 255);
        cM(LOW,LOW,0,0); //停
        delay(200);
       }
     if (inB=='1')
     { 
      choose=1;
      breathing(255,255,0);
      cM(LOW,LOW,0,0); //停
       delay(200);
       cM(HIGH,HIGH,255,255);//前進
     } 
     if (inB=='2')
     {
      choose=1;
      breathing(0, 0, 255);
      cM(LOW,LOW,0,0); //停
       delay(200);
      cM(LOW,LOW,255,255);//後退
     }
     if (inB=='3')
     {choose=1;
      breathing(255, 128, 0);
      cM(LOW,LOW,0,0); //停
      delay(200);
      cM(LOW,HIGH,255,255);//逆時針
     }
     if (inB=='4') {
      choose=1;
      breathing(128, 128,128);
       cM(LOW,LOW,0,0); //停
        delay(200);
      cM(HIGH,LOW,255,255);//順時針
     }
     if(inB=='S'){
        choose=0;
      }
     if(choose==0){
        breathing(255, 0, 255);
 int left_d, right_d;//紀錄左,右邊障礙物距離
 d=ping_d();//偵測前方障礙物距離
 //前方30cm處有障礙物,自走車進入判斷模式,決定行進方式
 if(d<=30){
    for(int i=0;i<=15;i++){
      ring.setPixelColor(i, 255, 0, 0); // 點紅第3顆LED(由0算起)
      ring.show();
      }        
       tone(SPK,700);
      delay(200);
      tone(SPK,1000);
      delay(200);
      //
    Serial.println("WARNING!!");
    lcd.print("WARNING");
    delay(500);
    lcd.clear();
    motoStop();//自走車停止前進
    servo.write(40);//馬達轉向右邊
    delay(50);
    right_d = ping_d();//取得右邊障礙物距離
    delay(20);
    //
    Serial.print("DistanceRIGHT: ");
    Serial.println(right_d);
    //delay(100);
    lcd.print(right_d); 
    delay(50);
    lcd.clear();
    //delay(1000);
    servo.write(150) ;//馬達轉向左邊
    delay(50);
    left_d=ping_d();//取得左邊障礙物的距離
    Serial.print("DistanceLEFT: ");
    Serial.println(left_d);
    //delay(100);
    lcd.print(left_d); 
    delay(500);
    lcd.clear();
    //delay(1000);
    servo.write(95); //超音波回正
    if( (left_d>right_d) && (left_d>30)) { //左邊有空間
        turnLeft() ;
        delay(350) ;
        forward() ;
     }else if( (right_d>=left_d) && (right_d>30)) { // 右邊空間大且右邊障礙物距離大於30cm以上 -->右轉彎後前進
        turnRight() ;
         delay(350) ;
         forward() ;
     }else {  // 前,左,右障礙物距離都小於30公分 --->後退->轉彎->前進
         backward() ;
         delay(1500) ;
         turnRight() ;
         delay(350) ;
         forward() ; 
     }    
  } 
   delay(30);
   forward();
     
     
      }
    }
}
