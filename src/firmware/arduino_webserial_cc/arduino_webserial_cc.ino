/*
 * 更新日期110/11/24 estea chen
 */
#include <Servo.h>
#include <DHTStable.h>
#include <Wire.h> 
//ws2812
#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);  
DHTStable DHT;
Servo myservo;  // create servo object to control a servo
#define NUMPIXELS 12 

char* serialString()
{
  //static char str[21]; // For strings of max length=20
  static char str[52]; // For strings of max length=20
  if (!Serial.available()) return NULL;
  delay(16); // wait for all characters to arrive
  memset(str,0,sizeof(str)); // clear str
  byte count=0;
  while (Serial.available())
  {
    char c=Serial.read();
    //if (c>=32 && count<sizeof(str)-1)
    //c最大35
    //if (c>=32 && count<sizeof(str)-1)
    //{
      str[count]=c;
      count++;
    //}
  }
  str[count]='\0'; // make it a zero terminated string
  return str;
}

void setup() {
  Serial.begin(115200);
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif
  lcd.init(); //初始化LCD 
  lcd.begin(16, 2); //初始化 LCD，代表我們使用的LCD一行有16個字元，共2行。
  lcd.backlight(); //開啟背光
}

void loop() 
{
  static boolean needPrompt=true;
  
  char* inputData;

  if (needPrompt)
  {
    //Serial.print("Please enter inputs and press enter at the end:\n");
    needPrompt=false;
  }
  inputData= serialString();
  
  if (inputData!=NULL)
  {
    
    //取出命令、腳位、值、時間
    char* commandString = strtok(inputData, "#"); 
    char* inputPin = strtok(NULL, "#");
    //取出第3個值
    char* inputValue = strtok(NULL, "#");
    //取出第4個值
    char* inputTime = strtok(NULL, "#");
    
    //ws2812
    if(strcmp(commandString, "ws") == 0){
        int r = atoi(strtok(inputValue,","));
        int g = atoi(strtok(NULL, ","));
        int b = atoi(strtok(NULL, ","));
        Adafruit_NeoPixel pixels(NUMPIXELS, atoi(inputPin), NEO_GRB + NEO_KHZ800);
        pixels.begin();        
        //pixels.clear();
        char* sp = "";
        for( int i = 0; i<NUMPIXELS ; i++){
          //sp = inputTime[i];
          if (inputTime[i] == '1') {
            sp = "0";
          }else if(inputTime[i] == '2'){
            sp = "1";
          }else if(inputTime[i] == '3'){
            sp = "2";
          }else if(inputTime[i] == '4'){
            sp = "3";
          }else if(inputTime[i] == '5'){
            sp = "4";
          }else if(inputTime[i] == '6'){
            sp = "5";
          }else if(inputTime[i] == '7'){
            sp = "6";
          }else if(inputTime[i] == '8'){
            sp = "7";
          }else if(inputTime[i] == '9'){
            sp = "8";
          }else if(inputTime[i] == 'a'){
            sp = "9";
          }else if(inputTime[i] == 'b'){
            sp = "10";
          }else if(inputTime[i] == 'c'){
            sp = "11";
          }
          pixels.setPixelColor(atoi(sp), pixels.Color(r, g, b));
        }
        pixels.show(); 
      }
    //lcd 16x2
    //format: l#string#row
    if(strcmp(commandString, "l") == 0) {
      if(strcmp(inputPin, "clear") == 0) {
          lcd.clear();
      }else{
          Serial.println(inputPin);
          if(atoi(inputValue) == 0){
            lcd.setCursor(0,0);
          }else{
            lcd.setCursor(0,1);
          }
          lcd.print(inputPin);  
      }
    }
      
    //超音波
    if(strcmp(commandString, "HC-SR04")== 0){
       long duration, cm; 
       int trigPin = atoi(inputPin);
       int echoPin = atoi(inputValue);
       pinMode(trigPin, OUTPUT);        // 定義輸入及輸出 
       pinMode(echoPin, INPUT);
       digitalWrite(trigPin, LOW);
       delayMicroseconds(5);
       //digitalWrite(trigPin, HIGH);     // 給 Trig 高電位，持續 10微秒  
       digitalWrite(trigPin, HIGH);     // 給 Trig 高電位，持續 10微秒  
       delayMicroseconds(10);
       digitalWrite(trigPin, LOW);
       //pinMode(echoPin, INPUT);             // 讀取 echo 的電位
       pinMode(echoPin, INPUT);             // 讀取 echo 的電位
       duration = pulseIn(echoPin, HIGH);   // 收到高電位時的時間
       cm = (duration/2) / 29.1;         // 將時間換算成距離 cm
       Serial.println(cm);        
    }
    
    //dht11
    if(strcmp(commandString, "dht11Set") == 0){
      DHT.read11(atoi(inputPin));
    }
    
    if(strcmp(commandString, "dht11Read") == 0){
      //int chk = DHT.read11(atoi(inputPin));
      Serial.print(DHT.getTemperature());
      Serial.print(",");
      Serial.println(DHT.getHumidity());
      /*
      if( atoi(inputValue) == 1 ){
          Serial.println(DHT.getTemperature(), 1);
      }else{
          Serial.println(DHT.getHumidity(), 1);
      }*/
    }
    //tone
    if(strcmp(commandString, "tonePlay") == 0){
      tone(atoi(inputPin), atoi(inputValue),inputTime);
      delay(inputTime);
      noTone(atoi(inputPin));
      delay(10);
    }
    //伺服馬達
    if(strcmp(commandString, "servoWrite") == 0){
      myservo.attach(atoi(inputPin));
      myservo.write(atoi(inputValue));
      delay(100);
    }
    //類比讀取
    if(strcmp(commandString, "analogRead") == 0){
      Serial.print("A");
      Serial.print(atoi(inputPin));
      Serial.print(":");
      Serial.println(analogRead(atoi(inputPin)));
    }
    //數位讀取
    if(strcmp(commandString, "digitalRead") == 0){
      pinMode(atoi(inputPin),INPUT);
      //2-19
      Serial.print("D");
      Serial.print(atoi(inputPin));
      Serial.print(":");
      Serial.println(digitalRead(atoi(inputPin)));
    }
    //類比寫入
    if(strcmp(commandString, "analogWrite") == 0){
      analogWrite(atoi(inputPin),atoi(inputValue));
    }
    //數位寫入
    if(strcmp(commandString, "digitalWrite") == 0){
         pinMode(atoi(inputPin),OUTPUT);
         digitalWrite(atoi(inputPin),atoi(inputValue));
     }
    needPrompt=true;
    //delay(1000);
  }
  
}
