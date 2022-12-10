#include "display.h"

// адрес может быть 0x27 или 0x3f
static LiquidCrystal_I2C lcd(0x20, 20, 4); // Устанавливаем дисплей
static bool ishow = true;

byte TermChar[8] = {
  B00100,
  B01010,
  B01010,
  B01110,
  B01110,
  B11111,
  B11111,
  B01110
};

byte HumenityChar[8] = {
  B00100,
  B00100,
  B01010,
  B01010,
  B10001,
  B10011,
  B10111,
  B01110
};

void disp_init() {
  lcd.init();
  lcd.backlight();  // Включаем подсветку дисплея
  lcd.createChar(1, TermChar);
  lcd.createChar(2, HumenityChar);

}

void clear_display(){
  lcd.clear();  
}

void clearLCDLine(int start, int line)
{               
  lcd.setCursor(start, line);
  for(int n = start; n < 20; n++) // 20 indicates symbols in line. For 2x16 LCD write - 16
  {
    lcd.print(" ");
  }
}

// For clock time
void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  if(digits < 10){
    lcd.print('0');
  }
  lcd.print(digits);
}

void shownotification(float val, int max, int min){
  if(val < max && val > min){
    lcd.print("NORMAL   ");
  }

  else if(val >= max ){
    lcd.print("WARNING!  ");
  }
  else if(val <= min){
    lcd.print("WARNING!  ");
  } 
}

void printTemp(float temp, int max, int min){
  lcd.setCursor(0,0);
  lcd.print("-> DEBUG MODE");
  lcd.setCursor(1, 1);
  lcd.print(String(int(floor(temp))) + char(223) + "C    ");
  lcd.setCursor(9, 1);
  shownotification(temp, max, min);
  lcd.setCursor(19, 1);
  lcd.write(1);
}

void printHum(float hum, int max, int min){
  lcd.setCursor(1, 2);
  lcd.print(String(int(floor(hum))) + " %    ");
  lcd.setCursor(9, 2);
  shownotification(hum, max, min);
  lcd.setCursor(19, 2);
  lcd.write(2);
}

void print_time(byte hour, byte minute, byte second){
  // time
  lcd.setCursor(1, 3);
  printDigits(hour); // print hours
  lcd.print(":");
  printDigits(minute); // print hours
  lcd.print(":");
  printDigits(second); // print hours
}

void print_date(byte day, byte month, int year){
  lcd.setCursor(11, 3);
  // date
  printDigits(day); // print hours
  lcd.print(".");
  printDigits(month); // print hours
  lcd.print(".");
  printDigits(String(year).substring(2, 4).toInt()); // print hours  
}

void print_not_select(String r_min, String r_max, int min, int max){  
  lcd.print(r_min);
  lcd.setCursor(14, 1);
  lcd.print("<"+String(min)+">");
      
  lcd.setCursor(0, 2);
  lcd.print(r_max);
      
  lcd.setCursor(14, 2);
  lcd.print("<"+String(max)+">");
      
  lcd.setCursor(0, 3);
  lcd.print("<-- 4   5-OK   6 -->");
}

void show_bottom(String msg){
  lcd.setCursor(0, 3);
  lcd.print(msg);   
}

void bright_input(String numb){
  if(ishow){
    lcd.print("<"+numb+">");        
  }
  else{
    lcd.print("    "); 
  } 
}

void invers(){
  if(ishow == true){
      ishow = false;      
    }
    else{
      ishow = true;            
    }         
}

void print_moved_itm(int pointer, String r_min, String r_max){
  if(pointer == 1){
    lcd.print(">" + r_min);
    lcd.setCursor(0, 2);
    lcd.print(" " + r_max);
  }
  
  if(pointer == 2){
    lcd.print(" " + r_min);
    lcd.setCursor(0, 2);
    lcd.print(">" + r_max);
  }
  show_bottom("<-- 4   5-OK   6 -->");  
}

void item_select(int pointer, String r_min, String r_max, String numb){
    if(pointer == 1){
      lcd.print(">" + r_min);
      lcd.setCursor(14, 1);
      bright_input(numb);
      lcd.setCursor(0, 2);
      lcd.print(" " + r_max);   
    }
    if(pointer == 2){
      lcd.print(" " + r_min);
      lcd.setCursor(0, 2);
      lcd.print(">" + r_max);
      lcd.setCursor(14, 2);
      bright_input(numb);    
    }
    show_bottom("CLOSE C   A SAVE");
}

void SettingsTemp(int pointer, bool isChange, String numb, int min_temp, int max_temp){
    lcd.setCursor(0,0);
    lcd.print("-> SETTINGS TEMP");
    lcd.setCursor(0, 1);
    // no pointers   
    if(pointer == 0){
      print_not_select(" MIN TEMP", " MAX TEMP", min_temp, max_temp);
    }
    
    if(pointer == 1 && isChange == false){
      print_moved_itm(pointer, "MIN TEMP", "MAX TEMP");
    }
    
    if(pointer == 2 && isChange == false){
      print_moved_itm(pointer, "MIN TEMP", "MAX TEMP");
    }
        
    if(pointer == 1 && isChange == true){
      item_select(pointer, "MIN TEMP", "MAX TEMP", numb);
    }
    
    if(pointer == 2 && isChange == true){
      item_select(pointer, "MIN TEMP", "MAX TEMP", numb);     
    }
  invers();
}

void SettingsHum(int pointer, bool isChange, String numb, int min_hum, int max_hum){
 lcd.setCursor(0,0);
    lcd.print("-> SETTINGS HUMANITY");
    lcd.setCursor(0, 1);
    // no pointers   
    if(pointer == 0){
      print_not_select(" MIN HUM", " MAX HUM", min_hum, max_hum);
    }
    
    if(pointer == 1 && isChange == false){
      print_moved_itm(pointer, "MIN HUM", "MAX HUM");
    }
    
    if(pointer == 2 && isChange == false){
      print_moved_itm(pointer, "MIN HUM", "MAX HUM");
    }
        
    if(pointer == 1 && isChange == true){
      item_select(pointer, "MIN HUM", "MAX TEMP", numb);
    }
    
    if(pointer == 2 && isChange == true){
      item_select(pointer, "MIN HUM", "MAX TEMP", numb);     
    }
  invers();
}

void SettingsRelays(int pointer, bool state_temp, bool state_hum){
  //lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("-> SETTINGS RELAYS");
  lcd.setCursor(0, 1);
  
   if(pointer == 0){
      lcd.print(" COOL/WARM");
      lcd.setCursor(14, 1);
      if(state_temp == true){
        lcd.print("ON");
      }
      else{
        lcd.print("OFF");
      }
      lcd.setCursor(0, 2);
      lcd.print(" HUMINITY");
      lcd.setCursor(14, 2);
      
      if(state_hum == true){
        lcd.print("ON");
      }
      else{
        lcd.print("OFF");
      }      
    }
    
    if(pointer == 1){
      print_moved_itm(pointer, "COOL/WARM", "HUMINITY");
    }
    
    if(pointer == 2 ){
      print_moved_itm(pointer, "COOL/WARM", "HUMINITY");
    }
 
  lcd.setCursor(0, 3);
  lcd.print("        5-OK   6 -->");
}



