
#include "devices.h"

#include "hal_uart.h"

#include "INA226.h"
#include "TMP100.h"
#include "LTC2942.h"
#include "MB85RS256A.h"

#define MAX_EPS_TMP_DEVS 4
#define MAX_EPS_INA_DEVS 20

static struct tmp_device eps_tmp_device[MAX_EPS_TMP_DEVS];
static struct ina_device eps_ina_device[MAX_EPS_INA_DEVS];
static struct ina_device eps_fram_device;

void device_init() {

  HAL_peripheral_open();

  //for(uint8_t i = 0; i <= MAX_EPS_TMP_DEVS; i++) {
  //  eps_tmp_device[i].id = i + SOL_YP_TEMP_DEV_ID;
  //  eps_tmp_device[i].temp = 0;
    //get_atmos(par_id id, eps_tmp_device[i].temp);
  //  eps_tmp_device[i].resolution = TMP100_RESOLUTION_12_BIT;
  //  tmp_init(eps_tmp_device[i].id,
  //              &(eps_tmp_device[i].mul),
  //              eps_tmp_device[i].resolution);
  //}

  //for(uint8_t i = 0; i <= MAX_EPS_INA_DEVS; i++) {
//    eps_ina_device[i].id = i + EPS_OBC_MON_DEV_ID;
    //get_atmos(par_id id, eps_ina_device[i].voltage);
    //get_atmos(par_id id, eps_ina_device[i].current);
//    eps_ina_device[i].voltage = 0;
//    eps_ina_device[i].current = 0;
//    eps_ina_device[i].shunt = 0.040;
//    ina_setShuntResistor(eps_ina_device[i].id,
//                         eps_ina_device[i].shunt);
//  }

  eps_ina_device[EPS_UR_MON_DEV_ID - EPS_OBC_MON_DEV_ID].id = EPS_UR_MON_DEV_ID;
  //get_atmos(par_id id, eps_ina_device[i].voltage);
  //get_atmos(par_id id, eps_ina_device[i].current);
  eps_ina_device[EPS_UR_MON_DEV_ID - EPS_OBC_MON_DEV_ID].voltage = 0;
  eps_ina_device[EPS_UR_MON_DEV_ID - EPS_OBC_MON_DEV_ID].current = 0;
  eps_ina_device[EPS_UR_MON_DEV_ID - EPS_OBC_MON_DEV_ID].shunt = 0.040;
  ina_setShuntResistor(eps_ina_device[EPS_UR_MON_DEV_ID - EPS_OBC_MON_DEV_ID].id,
                       eps_ina_device[EPS_UR_MON_DEV_ID - EPS_OBC_MON_DEV_ID].shunt);

   eps_ina_device[EPS_DC_MON_DEV_ID - EPS_OBC_MON_DEV_ID].id = EPS_DC_MON_DEV_ID;
   //get_atmos(par_id id, eps_ina_device[i].voltage);
   //get_atmos(par_id id, eps_ina_device[i].current);
   eps_ina_device[EPS_DC_MON_DEV_ID - EPS_OBC_MON_DEV_ID].voltage = 0;
   eps_ina_device[EPS_DC_MON_DEV_ID - EPS_OBC_MON_DEV_ID].current = 0;
   eps_ina_device[EPS_DC_MON_DEV_ID - EPS_OBC_MON_DEV_ID].shunt = 0.040;
   ina_setShuntResistor(eps_ina_device[EPS_DC_MON_DEV_ID - EPS_OBC_MON_DEV_ID].id,
                        eps_ina_device[EPS_DC_MON_DEV_ID - EPS_OBC_MON_DEV_ID].shunt);

  //ltc_init(BATT_CHARGE_DEV_ID,
  //         &(eps_device[id].property.R_sense),
  //         &(eps_device[id].property.M),
  //         eps_device[id].property.Q,
  //         eps_device[id].property.R,
  //         eps_device[id].property.I);

  //FRAM_init(EPS_FRAM_DEV_ID);

}

void update_device(dev_id id) {

  if(id == EPS_OBC_MON_DEV_ID ||
     id == EPS_COMMS_MON_DEV_ID ||
     id == EPS_ADCS_MON_DEV_ID ||
     id == EPS_SU_MON_DEV_ID ||
     id == EPS_DC_MON_DEV_ID ||
     id == EPS_UR_MON_DEV_ID ||
     id == SOL_YP_MON_DEV_ID ||
     id == SOL_YM_MON_DEV_ID ||
     id == SOL_XP_MON_DEV_ID ||
     id == SOL_XM_MON_DEV_ID ) {

    //int16_t current = ina_getCurrent(id);
    //uint16_t voltage = ina_getVoltage(id);
    //set_atmos_parameter(eps_ina_device[id - EPS_OBC_MON_DEV_ID].current,
    //                    current);
    //set_atmos_parameter(eps_ina_device[id - EPS_OBC_MON_DEV_ID].voltage,
    //                    voltage);

    eps_ina_device[id - EPS_OBC_MON_DEV_ID].current = ina_getCurrent(id);
    for(uint32_t d = 0; d < 100000; d++) {}
    eps_ina_device[id - EPS_OBC_MON_DEV_ID].voltage = ina_getVoltage(id);
    for(uint32_t d = 0; d < 100000; d++) {}

    int16_t temp_curr = 0;

    bool ev = ina_averaging( &(eps_ina_device[id - EPS_OBC_MON_DEV_ID].ina_av),
                             eps_ina_device[id - EPS_OBC_MON_DEV_ID].current,
                             &temp_curr);
    if(ev) {
      //eps_ina_device[id - EPS_OBC_MON_DEV_ID].current = temp_curr;
      ina_array(&(eps_ina_device[id - EPS_OBC_MON_DEV_ID].ina_arr), temp_curr);
      //atmos_event();
    }

  } else if(id == SOL_YP_TEMP_DEV_ID ||
            id == SOL_YM_TEMP_DEV_ID ||
            id == SOL_XP_TEMP_DEV_ID ||
            id == SOL_XM_TEMP_DEV_ID) {

    int32_t temp;

    tmp_getTemperature(id,
                       eps_tmp_device[id - SOL_YP_TEMP_DEV_ID].mul,
                       &eps_tmp_device[id - SOL_YP_TEMP_DEV_ID].temp);
    //atmos_event();

    //set_atmos_parameter(eps_ina_device[id - SOL_YP_TEMP_DEV_ID].temp,
    //                    temp);

  }  else if(id == BATT_CHARGE_DEV_ID) {

    //ltc_code_to_voltage(id, uint16_t *voltage);
  }

}

void read_device_parameters(dev_id id, void * data) {

  if(id == EPS_OBC_MON_DEV_ID ||
     id == EPS_COMMS_MON_DEV_ID ||
     id == EPS_ADCS_MON_DEV_ID ||
     id == EPS_SU_MON_DEV_ID ||
     id == EPS_DC_MON_DEV_ID ||
     id == EPS_UR_MON_DEV_ID ||
     id == SOL_YP_MON_DEV_ID ||
     id == SOL_YM_MON_DEV_ID ||
     id == SOL_XP_MON_DEV_ID ||
     id == SOL_XM_MON_DEV_ID ) {

    ((struct ina_device*)data)->shunt = eps_ina_device[id - EPS_OBC_MON_DEV_ID].shunt;
    ((struct ina_device*)data)->current = eps_ina_device[id - EPS_OBC_MON_DEV_ID].current;
    ((struct ina_device*)data)->voltage = eps_ina_device[id - EPS_OBC_MON_DEV_ID].voltage;

  } else if(id == SOL_YP_TEMP_DEV_ID ||
            id == SOL_YM_TEMP_DEV_ID ||
            id == SOL_XP_TEMP_DEV_ID ||
            id == SOL_XM_TEMP_DEV_ID) {

    ((struct tmp_device*)data)->mul = eps_tmp_device[id - SOL_YP_TEMP_DEV_ID].mul;
    ((struct tmp_device*)data)->temp = eps_tmp_device[id - SOL_YP_TEMP_DEV_ID].temp;

  }  else if(id == BATT_CHARGE_DEV_ID) {

    //ltc_code_to_voltage(id, uint16_t *voltage);

  }  else if(id == EPS_FRAM_DEV_ID) {

     FRAM_read(id,
               ((struct fram_device*)data)->address,
               ((struct fram_device*)data)->buffer,
               ((struct fram_device*)data)->count);
  }

}

void write_device_parameters(dev_id id, void * data) {

  if(id == EPS_OBC_MON_DEV_ID ||
     id == EPS_COMMS_MON_DEV_ID ||
     id == EPS_ADCS_MON_DEV_ID ||
     id == EPS_SU_MON_DEV_ID ||
     id == EPS_DC_MON_DEV_ID ||
     id == EPS_UR_MON_DEV_ID ||
     id == SOL_YP_MON_DEV_ID ||
     id == SOL_YM_MON_DEV_ID ||
     id == SOL_XP_MON_DEV_ID ||
     id == SOL_XM_MON_DEV_ID ) {

       uint16_t temp_id = id - EPS_OBC_MON_DEV_ID;
       eps_ina_device[temp_id].shunt = ((struct ina_device*)data)->shunt;
       //ina_setShuntResistor(eps_ina_device[temp_id].id,
       //                     eps_ina_device[temp_id].shunt);

  } else if(id == SOL_YP_TEMP_DEV_ID ||
            id == SOL_YM_TEMP_DEV_ID ||
            id == SOL_XP_TEMP_DEV_ID ||
            id == SOL_XM_TEMP_DEV_ID) {

        uint16_t temp_id = id - SOL_YP_TEMP_DEV_ID;
        eps_tmp_device[temp_id].resolution =
                                         ((struct tmp_device*)data)->resolution;
        //tmp_init(eps_tmp_device[temp_id].id,
        //            &(eps_tmp_device[temp_id].mul),
        //            eps_tmp_device[temp_id].resolution);

  }  else if(id == BATT_CHARGE_DEV_ID) {

    //ltc_code_to_voltage(id, uint16_t *voltage);

  }  else if(id == EPS_FRAM_DEV_ID) {

     FRAM_write(id,
                ((struct fram_device*)data)->address,
                ((struct fram_device*)data)->buffer,
                ((struct fram_device*)data)->count);
  }

}

void ina_array(struct ina_array *ina_arr,
               int16_t current) {

   if(!ina_arr->enabled) {
     return;
   }

  //shift old values
  if(ina_arr->data_counter == ina_arr->number_of_data - 1) {
   for(uint8_t i=0; i < ina_arr->data_counter - 1; i++) {
     ina_arr->current[i] = ina_arr->current[i + 1];
   }
  }

  //add new value
  ina_arr->current[ina_arr->data_counter] = current;

  //increase if array not full
  if(!(ina_arr->data_counter == ina_arr->number_of_data)) {
   ina_arr->data_counter++;
  }

}

bool ina_averaging(struct ina_average *ina_av,
                   int16_t current,
                   int16_t *new_value) {

  int16_t av = 0;

  //calculate average if its enabled, and add new value if the av is finished
  if(!ina_av->enabled) {
    return true;
  }

  ina_av->av[ina_av->average_counter] = current;
  ina_av->average_counter++;

  // averege pool is not filled yet
  if(!(ina_av->average_counter == ina_av->number_of_averages)) {
    return false;
  }

  for(uint8_t i=0; i < ina_av->number_of_averages; i++) {
    av += ina_av->av[i];
  }
  *new_value = av / ina_av->number_of_averages;
  ina_av->average_counter = 0;

  return true;
}
