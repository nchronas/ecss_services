#ifndef __DEVICES_H
#define __DEVICES_H

#include "satellite.h"
#include "stdint.h"

struct ina_array {
  bool enabled;
  int16_t current[5];

  uint8_t number_of_data;
  uint8_t data_counter;
};

struct ina_average {
  bool enabled;
  int16_t av[5];

  uint8_t number_of_averages;
  uint8_t average_counter;
};

struct ina_device {
  dev_id id;
  uint16_t voltage;
  int16_t current;
  double shunt;
  struct ina_average ina_av;
  struct ina_array ina_arr;
};

struct tmp_device {
  dev_id id;
  uint8_t resolution;
  int32_t temp;
  uint16_t mul;
};

struct fram_device {
  dev_id id;
  uint16_t address;
  uint8_t *buffer;
  size_t count;
};

void device_init();

void update_device(dev_id id);

void read_device_parameters(dev_id id, void * data);

void write_device_parameters(dev_id id, void * data);

void get_device_parameters(dev_id id, void * data);

bool ina_averaging(struct ina_average *ina_av,
                   int16_t current,
                   int16_t *new_value);

#endif
