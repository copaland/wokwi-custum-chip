// Wokwi Custom Chip - For docs and examples see:
// https://docs.wokwi.com/chips-api/getting-started
//
// SPDX-License-Identifier: MIT
// Copyright 2023 김랑기(전자명장)

#include "wokwi-api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GETCHIPSTATEFROM(usr_data) chip_state_t* chip = (chip_state_t*)usr_data


typedef struct {
  pin_t chipselect;
  pin_t   cs_pin;
  pin_t   sck;
  pin_t   so;
  pin_t   vref;
  uint16_t Tplus;
  uint16_t Tminus;

  spi_dev_t spi;
  uint8_t  spi_buffer[1];
  uint8_t bytecount;
} chip_state_t;

bool chip_spi_selected(chip_state_t* chip) {
  if (pin_read(chip->chipselect)) {
    return false;
  }

  return true;
}

char nextResponseCharacter(chip_state_t * chip) {

  const char response[] = "hello there ";
  char r = response[chip->bytecount % strlen(response)];

  chip->bytecount++;
  return r;
}

void select_changed(void *user_data, pin_t pin, uint32_t value){
  GETCHIPSTATEFROM(user_data);
  if (chip_spi_selected(chip)) {
    // chip->spi_buffer[0] = nextResponseCharacter(chip); // reset what we're sending on first byte
    spi_start(chip->spi, chip->spi_buffer, 1);
    printf("Chip selected\n");
  } else {
    printf(" received\n");
    spi_stop(chip->spi);
  }
}



void chip_spi_done(void *user_data, uint8_t *buffer, uint32_t count) {

  chip_state_t *chip = (chip_state_t*)user_data;
  if (!count) {
    // This means that we got here from spi_stop, and no data was received
    return;
  }

  // do it
  printf("'%c' ", buffer[0]);

  chip->spi_buffer[0] = nextResponseCharacter(chip);
  

  if (chip_spi_selected(chip)) {
    spi_start(chip->spi, chip->spi_buffer, sizeof(chip->spi_buffer));
  }

}


void chip_init() {
  chip_state_t *chip = malloc(sizeof(chip_state_t));

  chip->chipselect = pin_init("CS", INPUT_PULLUP);

  const pin_watch_config_t watch_select_config = {
    .edge = BOTH,
    .pin_change = select_changed,
    .user_data = chip,
  };

  pin_watch(chip->chipselect, &watch_select_config);

  const spi_config_t spi1conf = {
    .sck = pin_init("CLK", INPUT),
    .mosi = pin_init("DIIO0", INPUT),
    .miso = pin_init("DOIO1", OUTPUT),
    .mode = 0,
    .done = chip_spi_done, // See the example below
    .user_data = chip,
  };

  chip->spi = spi_init(&spi1conf);
  
  printf("SPI chip started...\n");
}
