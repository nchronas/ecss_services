#!/usr/bin/env python#

import sys
from subprocess import call

#path = sys.argv[1:]
path = "/Users/nanimo/Documents/repos/nik/"

ecss_path = path + "ecss_services/"

print "Input to ccs directories"
print ""

call(["mkdir", "libs"])
call(["mkdir", "ttc"])


folder = "ttc"
f = ecss_path + "delfiPQ/ADB/HAL/hal_uart.c"
call(["ln", "-s " + f + folder])

f = ecss_path + "delfiPQ/ADB/devices.c"
call(["ln", "-s " + f + folder])

f = ecss_path + "delfiPQ/ADB/housekeeping.c"
call(["ln", "-s " + f + folder])

f = ecss_path + "delfiPQ/ADB/subsystem.c"
call(["ln", "-s " + f + folder])

f = ecss_path + "delfiPQ/OSAL/osal.c"
call(["ln", "-s " + f + folder])

f = ecss_path + "delfiPQ/packet_engine.c"
call(["ln", "-s " + f + folder])

f = ecss_path + "delfiPQ/satellite.c"
call(["ln", "-s " + f + folder])


f = ecss_path + "services/housekeeping_service.c"
call(["ln", "-s " + f + folder])

f = ecss_path + "services/test_service.c"
call(["ln", "-s " + f + folder])


f = ecss_path + "core/hldlc.c"
call(["ln", "-s " + f + folder])

f = ecss_path + "core/packet_services.c"
call(["ln", "-s " + f + folder])

f = ecss_path + "core/pkt_pool.c"
call(["ln", "-s " + f + folder])

f = ecss_path + "core/queue.c"
call(["ln", "-s " + f + folder])


folder = "libs"
f = path + "TMP100/TMP100.c"
call(["ln", "-s " + f + folder])

f = path + "TMP100/INA226.c"
call(["ln", "-s " + f + folder])
