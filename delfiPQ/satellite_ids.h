
#ifndef __SATELLITE_IDS_H
#define __SATELLITE_IDS_H

#define SYS_LITTLE_ENDIAN 0
#define SYS_BIG_ENDIAN 1

/**
 * Each subsystem has a unique APP ID that is used to route ECSS packets
 * within the different satellite subsystems.
 */

#define _OBC_APP_ID_   1
#define _EPS_APP_ID_   2
#define _ADB_APP_ID_   3
#define _COMMS_APP_ID_ 4
#define _ADCS_APP_ID_  5
#define _GND_APP_ID_   6
#define _DBG_APP_ID_   7
#define _LAST_APP_ID_  8

#endif /* SERVICES_SUBSYSTEMS_IDS_H_ */
