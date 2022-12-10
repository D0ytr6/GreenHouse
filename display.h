#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <string.h>

void disp_init();
void clear_display();
void clearLCDLine(int start, int line);
void printDigits(int digits);
void shownotification(float val, int max, int min);
void printTemp(float temp, int max, int min);
void printHum(float hum, int max, int min);
void print_time(byte hour, byte minute, byte second);
void print_date(byte day, byte month, int year);
void SettingsTemp(int pointer, bool isChange, String numb, int min_temp, int max_temp);
void SettingsHum(int pointer, bool isChange, String numb, int min_hum, int max_hum);
void SettingsRelays(int pointer, bool state_temp, bool state_hum);