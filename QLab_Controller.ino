/*
  Firmware for QLab Control Box
  Author: Brian Ward
  Updated: JuLY 17, 2018
*/
// Libraries
#include <Bounce2.h>
#include <MIDIUSB.h>

// Constants
#define CHANNEL 0x00 // MIDI Channel 0 (or 1, depending on standard)
#define VELOCITY 0x7F
#define NUM_BUTTONS 6

// IMPORTANT: The order of buttonPins below determines which note each button corresponds to. The first entry sends Note 1, etc.
int buttonPins[NUM_BUTTONS] = {6, 7, 8, 9, 10, 11};

int leds[NUM_BUTTONS] = {A0, A1, A2, A3, A4, A5};

Bounce buttons[NUM_BUTTONS];

// Stores the time when the buttons were last pushed. Only used for repeat commands
unsigned long buttonPressTimeStamps[NUM_BUTTONS];
// Flag for if the button has been repeated. Only used on repeat buttons
bool hasRepeated[NUM_BUTTONS];

void setup() {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    // Turn on LEDs
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], HIGH);

    // Set up buttons (using Bounce2)
    buttons[i] = Bounce();
    pinMode(buttonPins[i], INPUT_PULLUP);
    buttons[i].attach(buttonPins[i]);
    buttons[i].interval(5);
  }
}

void loop() {
  // Update Buttons
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].update();
  }

  // If a button fell (was pushed), store the time, and send a Note On
  for (int i = 0; i < NUM_BUTTONS; i++) {
    if (buttons[i].fell()) {
      buttonPressTimeStamps[i] = millis();
      hasRepeated[i] = false;
      noteOn(i + 1);
      MidiUSB.flush();
    }
  }
  
  // Hard-coded repeat for UP and DOWN
  for(int i = 3; i < NUM_BUTTONS; i+=2){
    if(buttons[i].read() == LOW){
      // Mimic keyboard repeating by waiting for 1 second, then repeating 4 times per second
      unsigned long pressDuration =  millis() - buttonPressTimeStamps[i];
      if((hasRepeated[i] || pressDuration >= 750) && (pressDuration >= 85)){
        buttonPressTimeStamps[i] = millis();
        hasRepeated[i] = true;
        noteOn(i + 1);
        MidiUSB.flush();
      }
    }
  }
}

void noteOn(byte pitch) {
  // Construct a Note On (0x09) packet on Channel 1 (0x00) with Velocity 127 (0x7F)
  midiEventPacket_t noteOn = {0x09, 0x90 | CHANNEL, pitch, VELOCITY};
  MidiUSB.sendMIDI(noteOn);
}





