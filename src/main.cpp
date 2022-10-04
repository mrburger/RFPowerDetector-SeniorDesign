#include <Arduino.h>
#include<defs.h>

#include <Metro.h> // Task scheduler

/*-- Function Definition --*/
void toggleLED();

Metro blinkTimer = Metro(LED_BLINK_DELAY); // Idle status LED Blink

void setup() 
{
  // TODO: Initialize Flash and RAM
}

void loop() 
{
  if (blinkTimer.check() == 1)
  {
    toggleLED();
  }
}

void toggleLED()
{
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}