//#define MAX_SPI_SPEED 500000	// дефайн для изменения скорости SPI, по умолч 1000000
#include <GyverMAX7219.h>
#include <DS3231.h>
#include <Wire.h>
#include <EEPROM.h>

//=========== часов =====================
DS3231 Clock;
//bool Century=false;
bool h12;
bool PM;
int setH;
int brit;// яркость матрицы
int setM;
bool keyMenuOld;
volatile boolean intFlag = 0; 

#define keyMenu 2 //key menu
#define keyH 3    //key hours
#define keyM 4    //key minutes
int interval = 3000; // time to indacate time
int interval_1 = 1000;// time to indikate temperature
unsigned long previousMillis;
//=========== индикатор =================
#define AM_W 32  // 4 матрицы (32 точки)
#define AM_H 8  // 2 матрицы (16 точек)

// дисплей 4х1, пин CS 10, остальные на аппаратный SPI
MAX7219 < 4, 1, 10 > mtrx;
//String cyfry[10]={"0", "1","2","3","4","5","6","7","8","9"};


void setPrint(void){//indication on set time
        mtrx.clear();
        mtrx.setCursor(2,0);
        if(setH<10){
          mtrx.print(" ");
          }
        mtrx.print(setH, DEC);
        mtrx.print(":");
        if(setM<10){
          mtrx.print("0");
          }
        mtrx.print(setM, DEC);
        mtrx.update();
        }


void setT(void){
 setH = Clock.getHour(h12, PM); 
 setM = Clock.getMinute(); 
 
 mtrx.clear();
 mtrx.setCursor(2,0);
 mtrx.print("настр");
 mtrx.update();
 delay(1000);
 setPrint();
    while(digitalRead(keyMenu) == 1){
      if(digitalRead(keyH)==0){
        if(setH<23){
            setH=setH+1;}
            else{
              setH=0;
              }
        setPrint();
        delay(500);
        }
      if(digitalRead(keyM)==0){
        if(setM <59){
          setM=setM+1;}
          else{
            setM=0;}
        setPrint();
        delay(500);
        }
      
      }
   Clock.setMinute(setM); 
   Clock.setHour(setH);
  delay(500);

  
  
 }


//============ настройка ================
void setup() {
  pinMode(keyMenu, INPUT);
  pinMode(keyH,INPUT);
  pinMode(keyM,INPUT);
   mtrx.begin();
  mtrx.setRotation(2);// rotate to 180 
  mtrx.setBright(1);//int(EEPROM.read(0)));
  // Start the I2C interface
  Wire.begin();
   Clock.setClockMode(false);// Set 12/24h mode. True is 12-h, false is 24-hour.
  //Clock.setMinute(48); 
  //Clock.setHour(18); 
}

//============ петля ===================  
void loop() {
  //0 < - time -> interval <- temp -> interval_1
 unsigned long currentMillis = millis();

if( digitalRead(keyMenu) == 0 && keyMenuOld == 1){
  setT();
  }
 keyMenuOld = digitalRead(keyMenu);

//=========== CLOCK ==========
    if(currentMillis - previousMillis <= interval  ){
           mtrx.clear();
           mtrx.setCursor(2,0);
             if(Clock.getHour(h12, PM)<10){
              mtrx.print(" ");
              }
           mtrx.print(Clock.getHour(h12, PM), DEC);
           //if(currentMillis - 
           mtrx.print(":");
             if(Clock.getMinute()<10){
              mtrx.print("0");
              }
           mtrx.print(Clock.getMinute(), DEC);
           mtrx.update();
      //delay(1000);
    }
 //=========== TEMPERATURE =======
if(currentMillis - previousMillis > interval &&  currentMillis - previousMillis < (interval+interval_1) ){
    
 mtrx.clear();
 mtrx.setCursor(2,0);
 if (Clock.getTemperature()>0){
 mtrx.print("+");}
 mtrx.print(Clock.getTemperature(), 1);
 mtrx.update();
 //delay(1000);
 //previousMillis = currentMillis;
}

if(currentMillis - previousMillis >= (interval+interval_1)){
  previousMillis = currentMillis;
}
}
