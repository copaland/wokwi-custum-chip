// Wokwi Custom Chip - For docs and examples see:
// https://docs.wokwi.com/chips-api/getting-started
//
// SPDX-License-Identifier: MIT
// Copyright 2023 김랑기(전자명장)

#include "wokwi-api.h"
#include <stdio.h>
#include <stdlib.h>

char digits[] = { 0x3F,0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x7, 0x7F, 0x6F };

typedef struct {
  pin_t segment_pins[7];
  pin_t pinA;
  pin_t pinB;
  pin_t pinC;
  pin_t pinD;
} chip_state_t;


static void update_output(chip_state_t *chip) {
  uint32_t a = pin_read(chip->pinA);
  uint32_t b = pin_read(chip->pinB);
  uint32_t c = pin_read(chip->pinC);
  uint32_t d = pin_read(chip->pinD);

  int out = a + 2 * b + 4 * c + 8 * d;

  if (out < 10) {
    char num = digits[out];

    for(int i = 0; i < 7; i++) {
      // common cathode -> common anode
      // ~ operator use
      pin_write(chip->segment_pins[i], !((num >> i) & 0x01));
    }
  }
  else {
    for(int i = 0; i < 7; i++) {
      pin_write(chip->segment_pins[i], 0);
    }
  }
  //printf("Update output: %d \n", out);
}

static void chip_pin_change(void *user_data, pin_t pin, uint32_t value) {
  update_output((chip_state_t*)user_data);
}

void chip_init() {
  chip_state_t *chip = malloc(sizeof(chip_state_t));

  // TODO: Initialize the chip, set up IO pins, create timers, etc.
  chip->pinA = pin_init("A", INPUT);
  chip->pinB = pin_init("B", INPUT);
  chip->pinC = pin_init("C", INPUT);
  chip->pinD = pin_init("D", INPUT);

  chip->segment_pins[0] = pin_init("a", OUTPUT);
  chip->segment_pins[1] = pin_init("b", OUTPUT);
  chip->segment_pins[2] = pin_init("c", OUTPUT);
  chip->segment_pins[3] = pin_init("d", OUTPUT);
  chip->segment_pins[4] = pin_init("e", OUTPUT);
  chip->segment_pins[5] = pin_init("f", OUTPUT);
  chip->segment_pins[6] = pin_init("g", OUTPUT);

  const pin_watch_config_t watch_config = {
    .edge = BOTH,
    .pin_change = chip_pin_change,
    .user_data = chip,
  };

  pin_watch(chip->pinA, &watch_config);
  pin_watch(chip->pinB, &watch_config);
  pin_watch(chip->pinC, &watch_config);
  pin_watch(chip->pinD, &watch_config);
  update_output(chip);
}
