#pragma once
#include <Arduino.h>

class Timer {

  private:
    uint32_t timer = 0;
    uint16_t period = 0;

  public:
    Timer(uint16_t nprd = 0) {
      setPeriod(nprd);
    }

    void setPeriod(uint16_t nprd) {
      period = nprd;
    }

    uint16_t getPeriod() {
      return period;
    }

    bool ready() {
      // count for period
      if (millis() - timer >= period) {        
        // save timer for next iteration
        timer = millis();
        return true;
      }
      return false;
    }

};