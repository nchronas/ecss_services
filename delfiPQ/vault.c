#include "vault.h"

#include <stdint.h>

#include "devices.h"

struct vault_data {
  par_id id;
  //uint8_t size;
  uint16_t address;
};

//static struct vault_data eps_vault_data[LAST_VAULT_ID] = {
//  {
//    .size = 1,
//    .address = 0x0000
//  }
//};

//void read_vault_parameter(atmos * id, void * data) {

//  struct fram_device dev;

 // dev.address = id->address;
 // dev.buffer = (uint8_t*)data;
 // dev.count = id->size;

  //read_device_parameters(EPS_FRAM_DEV_ID,  (void *) &dev);
//}

void write_vault_parameter(vault_id id, void * data) {

  struct fram_device dev;

  //dev.address = eps_vault_data[id].address;
  dev.buffer = (uint8_t*)data;
  //dev.count = eps_vault_data[id].size;

  write_device_parameters(EPS_FRAM_DEV_ID,  (void *) &dev);

}

// void read_vault_parameter(vault_id id, void * data) {
//
//   struct fram_device dev;
//
//   dev.address = eps_vault_data[id].address;
//   dev.buffer = (uint8_t*)data;
//   dev.count = eps_vault_data[id].size;
//
//   read_device_parameters(EPS_FRAM_DEV_ID,  (void *) &dev);
// }
//
// void write_vault_parameter(vault_id id, void * data) {
//
//   struct fram_device dev;
//
//   dev.address = eps_vault_data[id].address;
//   dev.buffer = (uint8_t*)data;
//   dev.count = eps_vault_data[id].size;
//
//   write_device_parameters(EPS_FRAM_DEV_ID,  (void *) &dev);
//
// }
