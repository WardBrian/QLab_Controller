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

  // If a button fell (was pushed), send a Note On
  for (int i = 0; i < NUM_BUTTONS; i++) {
    if (buttons[i].fell()) {
      noteOn(i + 1);
      MidiUSB.flush();
    }
  }
}

void noteOn(byte pitch) {
  // Construct a Note On (0x09) packet on Channel 1 (0x00) with Velocity 127 (0x7F)
  midiEventPacket_t noteOn = {0x09, 0x90 | CHANNEL, pitch, VELOCITY};
  MidiUSB.sendMIDI(noteOn);
}





