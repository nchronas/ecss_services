
#include "devices.h"

#include "hal_uart.h"

#include "INA226.h"
#include "TMP100.h"
#include "LTC2942.h"
#include "MB85RS256A.h"

#define MAX_TMP_DEVS 4
#define MAX_INA_DEVS 10

static struct tmp_device tmp_dev[MAX_TMP_DEVS];
static struct ina_device ina_dev[MAX_INA_DEVS];
//static struct ltc_device eps_ltc_device;

void device_init() {

  HAL_peripheral_open();

  uint8_t i;

  for(i = 0; i < MAX_TMP_DEVS; i++) {
    tmp_dev[i].id = i + SOL_YP_TEMP_DEV_ID;
    tmp_dev[i].temp = 0;
    //get_atmos(par_id id, tmp_dev[i].temp);
    tmp_dev[i].resolution = TMP100_RESOLUTION_12_BIT;
    tmp_init(tmp_dev[i].id,
               &(tmp_dev[i].mul),
               tmp_dev[i].resolution);
  }

  for(i = 0; i < MAX_INA_DEVS; i++) {
    ina_dev[i].id = i + EPS_OBC_MON_DEV_ID;
    ina_dev[i].voltage = 0;
    ina_dev[i].current = 0;
    ina_dev[i].power = 0;
    ina_dev[i].voltage_raw = 0;
    ina_dev[i].current_raw = 0;
    ina_dev[i].power_raw = 0;
    ina_dev[i].shunt = 0.040;
    INA226_configure(ina_dev[i].id,
                      INA226_AVERAGES_1,
                      INA226_BUS_CONV_TIME_1100US,
                      INA226_SHUNT_CONV_TIME_1100US,
                      INA226_MODE_SHUNT_BUS_CONT);
    usleep(10);
      // Calibrate INA226. Rshunt = 0.01 ohm, Max excepted current = 4A
    INA226_calibrate(ina_dev[i].id,
                     0.04,
                     0.5,
                     &(ina_dev[i].currentLSB),
                     &(ina_dev[i].powerLSB));
    usleep(10);
  }

  //ltc_init(BATT_CHARGE_DEV_ID,
  //         &(eps_ltc_device.property.R_sense),
//           &(eps_ltc_device.property.M),
//           eps_ltc_device.property.Q,
//           eps_ltc_device.property.R,
//           eps_ltc_device.property.I);

  FRAM_init(EPS_FRAM_DEV_ID);

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

    uint8_t pos_index = id - EPS_OBC_MON_DEV_ID;

    ina_dev[pos_index].current_raw = INA226_readShuntCurrent_raw(id);
    OSAL_sys_delay(1);

    ina_dev[pos_index].voltage_raw = INA226_readBusVoltage_raw(id);
    OSAL_sys_delay(1);

    ina_dev[pos_index].power_raw = INA226_readBusPower_raw(id);

    ina_dev[pos_index].power = INA226_rawBusPower(ina_dev[pos_index].power_raw , ina_dev[pos_index].powerLSB);
    ina_dev[pos_index].current = INA226_rawShuntCurrent( ina_dev[pos_index].current_raw, ina_dev[pos_index].currentLSB);
    ina_dev[pos_index].voltage = INA226_rawBusVoltage(ina_dev[pos_index].voltage_raw);

  } else if(id == SOL_YP_TEMP_DEV_ID ||
            id == SOL_YM_TEMP_DEV_ID ||
            id == SOL_XP_TEMP_DEV_ID ||
            id == SOL_XM_TEMP_DEV_ID) {

     uint8_t pos_index = id - SOL_YP_TEMP_DEV_ID;

     tmp_getTemperature(id,
                        tmp_dev[pos_index].mul,
                        &tmp_dev[pos_index].temp);


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

      uint8_t pos_index = id - EPS_OBC_MON_DEV_ID;

    ((struct ina_device*)data)->power = ina_dev[pos_index].power;
    ((struct ina_device*)data)->current = ina_dev[pos_index].current;
    ((struct ina_device*)data)->voltage = ina_dev[pos_index].voltage;

  } else if(id == SOL_YP_TEMP_DEV_ID ||
            id == SOL_YM_TEMP_DEV_ID ||
            id == SOL_XP_TEMP_DEV_ID ||
            id == SOL_XM_TEMP_DEV_ID) {

      uint8_t pos_index = id - SOL_YP_TEMP_DEV_ID;

    ((struct tmp_device*)data)->mul = tmp_dev[pos_index].mul;
    ((struct tmp_device*)data)->temp = tmp_dev[pos_index].temp;

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
       ina_dev[temp_id].shunt = ((struct ina_device*)data)->shunt;
       //ina_setShuntResistor(ina_dev[temp_id].id,
       //                     ina_dev[temp_id].shunt);

  } else if(id == SOL_YP_TEMP_DEV_ID ||
            id == SOL_YM_TEMP_DEV_ID ||
            id == SOL_XP_TEMP_DEV_ID ||
            id == SOL_XM_TEMP_DEV_ID) {

        uint16_t temp_id = id - SOL_YP_TEMP_DEV_ID;
        tmp_dev[temp_id].resolution =
                                         ((struct tmp_device*)data)->resolution;
        //tmp_init(tmp_dev[temp_id].id,
        //            &(tmp_dev[temp_id].mul),
        //            tmp_dev[temp_id].resolution);

  }  else if(id == BATT_CHARGE_DEV_ID) {

    //ltc_code_to_voltage(id, uint16_t *voltage);

  }  else if(id == EPS_FRAM_DEV_ID) {

     FRAM_write(id,
                ((struct fram_device*)data)->address,
                ((struct fram_device*)data)->buffer,
                ((struct fram_device*)data)->count);
  }

}
