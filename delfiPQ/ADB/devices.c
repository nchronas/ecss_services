
#include "devices.h"

#include "hal_uart.h"

#include "INA226.h"
#include "TMP100.h"
#include "LTC2942.h"
#include "MB85RS256A.h"

#define MAX_EPS_TMP_DEVS 1
#define MAX_EPS_INA_DEVS 1

static struct tmp_device adb_tmp_device;
static struct ina_device adb_ina_device;

void device_init() {

  HAL_peripheral_open();

  ADB_MON_DEV_ID          = 26,
  ADB_TEMP_DEV_ID         = 27,

  adb_tmp_device.id = ADB_TEMP_DEV_ID;
  adb_tmp_device.temp = 0;
  //get_atmos(par_id id, eps_tmp_device[i].temp);
  adb_tmp_device.resolution = TMP100_RESOLUTION_12_BIT;
  tmp_init(adb_tmp_device.id,
           &(adb_tmp_device.mul),
           adb_tmp_device.resolution);


  eps_ina_device[i].id = i + EPS_OBC_MON_DEV_ID;
  eps_ina_device[i].voltage = 0;
  eps_ina_device[i].current = 0;
  eps_ina_device[i].power = 0;
  eps_ina_device[i].voltage_raw = 0;
  eps_ina_device[i].current_raw = 0;
  eps_ina_device[i].power_raw = 0;
  eps_ina_device[i].shunt = 0.040;
  INA226_configure(eps_ina_device[i].id,
                    INA226_AVERAGES_1,
                    INA226_BUS_CONV_TIME_1100US,
                    INA226_SHUNT_CONV_TIME_1100US,
                    INA226_MODE_SHUNT_BUS_CONT);
  usleep(10);
    // Calibrate INA226. Rshunt = 0.01 ohm, Max excepted current = 4A
  INA226_calibrate(eps_ina_device[i].id,
                   0.04,
                   0.5,
                   &(eps_ina_device[i].currentLSB),
                   &(eps_ina_device[i].powerLSB));
  usleep(10);

}

void update_device(dev_id id) {

  if(id == EPS_OBC_MON_DEV_ID) {

    uint8_t pos_index = id - EPS_OBC_MON_DEV_ID;

    eps_ina_device[pos_index].current_raw = INA226_readShuntCurrent_raw(id);
    OSAL_sys_delay(1);

    eps_ina_device[pos_index].voltage_raw = INA226_readBusVoltage_raw(id);
    OSAL_sys_delay(1);

    eps_ina_device[pos_index].power_raw = INA226_readBusPower_raw(id);

    eps_ina_device[pos_index].power = INA226_rawBusPower(eps_ina_device[pos_index].power_raw , eps_ina_device[pos_index].powerLSB);
    eps_ina_device[pos_index].current = INA226_rawShuntCurrent( eps_ina_device[pos_index].current_raw, eps_ina_device[pos_index].currentLSB);
    eps_ina_device[pos_index].voltage = INA226_rawBusVoltage(eps_ina_device[pos_index].voltage_raw);

  } else if(id == SOL_YP_TEMP_DEV_ID) {

    int32_t temp;

    tmp_getTemperature(id,
                       eps_tmp_device[id - SOL_YP_TEMP_DEV_ID].mul,
                       &eps_tmp_device[id - SOL_YP_TEMP_DEV_ID].temp);

  }

}

void read_device_parameters(dev_id id, void * data) {

  if(id == EPS_OBC_MON_DEV_ID) {

      uint8_t pos_index = id - EPS_OBC_MON_DEV_ID;

    ((struct ina_device*)data)->power = eps_ina_device[pos_index].power;
    ((struct ina_device*)data)->current = eps_ina_device[pos_index].current;
    ((struct ina_device*)data)->voltage = eps_ina_device[pos_index].voltage;

  } else if(id == SOL_YP_TEMP_DEV_ID) {

    ((struct tmp_device*)data)->mul = eps_tmp_device[id - SOL_YP_TEMP_DEV_ID].mul;
    ((struct tmp_device*)data)->temp = eps_tmp_device[id - SOL_YP_TEMP_DEV_ID].temp;

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

  }

}
