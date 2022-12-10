#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <DHT.h>
#include <math.h>
#include <RTClib.h>
#include "display.h"
#include "timer.h"
#include <Keypad.h>

//initialize the liquid crystal library
//the first parameter is the I2C address
//the second parameter is how many rows are on your screen
//the third parameter is how many columns are on your screen
#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 22  (AM2302)
#define RELAY_CONDITIONER 12
#define RELAY_WARMER 13

RTC_DS1307 rtc; //time count object
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
Timer display_timer = Timer(1000);
Timer sensor_timer = Timer(3000);

//Variables
int chk;
float hum = 0;  //Stores humidity value
float temp = 0; //Stores temperature value
int MAX_TEMP = 40;
int MIN_TEMP = 10;
int MAX_HUM = 90;
int MIN_HUM = 30;
int Midle_Temp;
int Middle_Hum;
const byte ROWS = 4; 
const byte COLS = 4;
int menu = 1;
int pointer = 0;
bool isChange = false;
bool isRelayConditionerOn = false;
bool isRelayWarmerOn = false;

bool isWorkingConditioner = false;
bool isWorkingWarmer = false;
String number = "";

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {10, 9, 8, 7}; 
byte colPins[COLS] = {6, 5, 4, 3}; 
Keypad keypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS );

void getSensorData(){
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  getMiddleValues();
  if(isRelayConditionerOn){
    if((temp > MAX_TEMP && isWorkingConditioner == false) || (temp < MIN_TEMP && isWorkingConditioner == false)){      
        turnOnConditioner();    
    }
    if(temp > (Midle_Temp - 2) && temp < (Midle_Temp + 2) && isWorkingConditioner == true){
        turnOFFConditioner();       
    }
  }

  if(isRelayWarmerOn){
    if((hum > MAX_HUM || hum < MIN_HUM) && isWorkingWarmer == false){
        turnOnWarmer();    
    }
    if(hum > (Middle_Hum - 10) && hum < (Middle_Hum + 10) && isWorkingWarmer == true){
        turnOFFWarmer();       
    }
  }

}

void MainMenu(){
  
  DateTime now = rtc.now();
  printTemp(temp, MAX_TEMP, MIN_TEMP);
  printHum(hum, MAX_HUM, MIN_HUM);

  print_time(now.hour(), now.minute(), now.second());
  print_date(now.day(), now.month(), now.year());

}

void turnOnConditioner(){
  isWorkingConditioner = true;
  digitalWrite(RELAY_CONDITIONER, HIGH);
}

void turnOFFConditioner(){
  isWorkingConditioner = false;
  digitalWrite(RELAY_CONDITIONER, LOW);  
}

void turnOnWarmer(){
  isWorkingWarmer = true;
  digitalWrite(RELAY_WARMER, HIGH);  
}

void turnOFFWarmer(){
  isWorkingWarmer = false;
  digitalWrite(RELAY_WARMER, LOW);  
}

void getMiddleValues(){
    Midle_Temp = (MAX_TEMP + MIN_TEMP) / 2;
    Middle_Hum = (MAX_HUM + MIN_HUM) / 2;
    
}

void setup() {
  Serial.begin(9600);
    // пин реле как выход
  pinMode(RELAY_CONDITIONER, OUTPUT);
  pinMode(RELAY_WARMER, OUTPUT);
  
  disp_init();
  dht.begin();
  getSensorData();
    // SETUP RTC MODULE
  if (! rtc.begin()) {
      while (1);
  }
    // automatically sets the RTC to the date & time on PC this sketch was compiled
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop() {
  // every 1 second by default
  if (display_timer.ready()){
    if(menu == 1){
      MainMenu();
    }
    if(menu == 2){
        SettingsTemp(pointer, isChange, number, MIN_TEMP, MAX_TEMP);
      }
      
    if(menu == 3){
      SettingsHum(pointer, isChange, number, MIN_HUM, MAX_HUM);
    }
    
    if(menu == 4){
      SettingsRelays(pointer, isRelayConditionerOn, isRelayWarmerOn);
    }
  
      
  }
  // every 3 second by default
  if(sensor_timer.ready()){
    getSensorData();    
  }
  char key = keypad.getKey();
  
  // Якщо сигналу на зміну значення нема
  if(isChange == false){
      // вибираємо меню
      if(key == '4'){ //left
      menu++;
      pointer = 0;     
      clear_display();   
    }
    
    if(key == '6'){ // right
      menu--;
      pointer = 0;   
      clear_display();   
    
    }

    // якщо вибрано меню з налаштуванням          
    if(key == '2'){ // up
      if(menu == 2 || menu == 3 || menu == 4){
        if(pointer != 0){
          pointer--;
        }
      }
    }
    
    if(key == '8'){ // down
      if(menu == 2 || menu == 3 || menu == 4){
        if(pointer != 2){
          pointer++;
        }
      }
    }
    
    if(key == '5'){ // Ок 
      if(menu == 2 || menu == 3){
        // Якщо вибрано підпункт меню, створюємо сигнал на зміну        
        if(pointer == 2 || pointer == 1){ 
          isChange = true;
        }
      }

      if( menu == 4){
        if(pointer == 1){
          isRelayConditionerOn = !isRelayConditionerOn;
          clearLCDLine(14, 1);
          pointer = 0;  
        }
        if(pointer == 2){
          isRelayWarmerOn = !isRelayWarmerOn;
          clearLCDLine(14, 2);
          pointer = 0;  
        }                
      }
      
    }
  }
  
  // Якщо сигналу на зміну прийшов, потрібно ввести нове значення температури
  else{
    // Якщо довжина менше 2х, додаєм число
    if(number.length() != 2){
      number = number + String(key);
    }
    // Зберігаємо число, скидаємо сигнал зміни
    if(key == 'A'){
      // temm settings      
      if(menu == 2){
        if(pointer == 1){ // min settings
          MIN_TEMP = number.toInt();
        }
        if(pointer == 2){ // max settings
          MAX_TEMP = number.toInt();
        }
      }

      if(menu == 3){
        if(pointer == 1){ // min settings
          MIN_HUM = number.toInt();
        }
        if(pointer == 2){ // max settings
          MAX_HUM = number.toInt();
        }
      }
      
      number = "";
      pointer = 0;  
      isChange = false;
    }
    
  }
  
}
