#include "devices.h"

#include "hal_uart.h"

#include "INA226.h"
#include "TMP100.h"

#define MAX_TMP_DEVS 1
#define MAX_INA_DEVS 1

static struct tmp_device tmp_dev[MAX_TMP_DEVS];
static struct ina_device ina_dev[MAX_INA_DEVS];

void device_init() {

  HAL_peripheral_open();

  uint8_t i;

  for(i = 0; i < MAX_TMP_DEVS; i++) {
    tmp_dev[i].id = ADB_TEMP_DEV_ID;
    tmp_dev[i].temp = 0;
    tmp_dev[i].raw_temp = 0;
    tmp_init(tmp_dev[i].id);
  }

  for(i = 0; i < MAX_INA_DEVS; i++) {
    ina_dev[i].id = i + ADB_MON_DEV_ID;
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

}

void update_device(dev_id id) {

  if(id == ADB_MON_DEV_ID) {

    uint8_t pos_index = id - ADB_MON_DEV_ID;

    ina_dev[pos_index].current_raw = INA226_readShuntCurrent_raw(id);
    OSAL_sys_delay(1);

    ina_dev[pos_index].voltage_raw = INA226_readBusVoltage_raw(id);
    OSAL_sys_delay(1);

    ina_dev[pos_index].power_raw = INA226_readBusPower_raw(id);

    ina_dev[pos_index].power = INA226_rawBusPower(ina_dev[pos_index].power_raw , ina_dev[pos_index].powerLSB);
    ina_dev[pos_index].current = INA226_rawShuntCurrent( ina_dev[pos_index].current_raw, ina_dev[pos_index].currentLSB);
    ina_dev[pos_index].voltage = INA226_rawBusVoltage(ina_dev[pos_index].voltage_raw);

  } else if(id == ADB_TEMP_DEV_ID) {

    uint8_t pos_index = id - ADB_TEMP_DEV_ID;

    tmp_getTemperature_raw(id, &tmp_dev[pos_index].raw_temp);
    tmp_getRawTemperature(id, tmp_dev[pos_index].raw_temp, &tmp_dev[pos_index].temp);


  }

}

void read_device_parameters(dev_id id, void * data) {

  if(id == ADB_MON_DEV_ID) {

      uint8_t pos_index = id - ADB_MON_DEV_ID;

    ((struct ina_device*)data)->power = ina_dev[pos_index].power;
    ((struct ina_device*)data)->current = ina_dev[pos_index].current;
    ((struct ina_device*)data)->voltage = ina_dev[pos_index].voltage;

    ((struct ina_device*)data)->power_raw = ina_dev[pos_index].power_raw;
    ((struct ina_device*)data)->current_raw = ina_dev[pos_index].current_raw;
    ((struct ina_device*)data)->voltage_raw = ina_dev[pos_index].voltage_raw;


  } else if(id == ADB_TEMP_DEV_ID) {

      uint8_t pos_index = id - ADB_TEMP_DEV_ID;

     ((struct tmp_device*)data)->raw_temp = tmp_dev[pos_index].raw_temp;
     ((struct tmp_device*)data)->temp = tmp_dev[pos_index].temp;

  }

}

void write_device_parameters(dev_id id, void * data) {

}
