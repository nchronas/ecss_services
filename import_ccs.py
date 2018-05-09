#!/usr/bin/env python#

import sys
import subprocess
from subprocess import call
import pip

#path = sys.argv[1:]
path = "/Users/nchronas/Documents/repos/delfiPQ/workspace7/"

print "Workspace dir:", path

print "Cloning repos"

cmd = "git clone https://github.com/nchronas/ADB_software.git"
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

cmd = "git clone https://github.com/nchronas/EPS_software.git"
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()


cmd = "git clone https://github.com/nchronas/INA226.git"
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

cmd = "git clone https://github.com/nchronas/TMP100.git"
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

cmd = "git clone https://github.com/nchronas/MB85RS256A.git"
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

cmd = "git clone https://github.com/nchronas/LTC2942.git"
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

print "Making ADB project"

working_dir = path + "ADB_software"

ecss_path = path + "ecss_services/"

print "Creating folders"

call(["mkdir", "libs"], cwd=working_dir)
call(["mkdir", "ttc"], cwd=working_dir)

print "Creating symlinks"

folder = "ADB_software/ttc/"

f = ecss_path + "delfiPQ/ADB/HAL/hal_uart.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

f = ecss_path + "delfiPQ/ADB/devices.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

f = ecss_path + "delfiPQ/ADB/housekeeping.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

f = ecss_path + "delfiPQ/ADB/subsystem.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

f = ecss_path + "delfiPQ/OSAL/osal.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

f = ecss_path + "delfiPQ/packet_engine.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

f = ecss_path + "delfiPQ/satellite.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()


f = ecss_path + "services/housekeeping_service.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

f = ecss_path + "services/test_service.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()


f = ecss_path + "core/hldlc.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

f = ecss_path + "core/packet_utilities.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

f = ecss_path + "core/packet_services.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

f = ecss_path + "core/pkt_pool.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

f = ecss_path + "core/queue.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

folder = "ADB_software/libs/"

f = path + "TMP100/TMP100.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

f = path + "INA226/INA226.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

print "Input to ccs directories"
print ecss_path + "delfiPQ/ADB"
print ecss_path + "delfiPQ/ADB/HAL"
print ecss_path + "delfiPQ/OSAL"
print ecss_path + "delfiPQ"
print ecss_path + "core"
print ecss_path + "services"
print path + "INA226"
print path + "TMP100"



print "Making EPS project"

working_dir = path + "EPS_software"

ecss_path = path + "ecss_services/"

print "Creating folders"

call(["mkdir", "libs"], cwd=working_dir)
call(["mkdir", "ttc"], cwd=working_dir)

print "Creating symlinks"

folder = "EPS_software/ttc/"

f = ecss_path + "delfiPQ/EPS/HAL/hal_uart.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

f = ecss_path + "delfiPQ/EPS/devices.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

f = ecss_path + "delfiPQ/EPS/housekeeping.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

f = ecss_path + "delfiPQ/EPS/subsystem.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

f = ecss_path + "delfiPQ/OSAL/osal.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

f = ecss_path + "delfiPQ/packet_engine.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

f = ecss_path + "delfiPQ/satellite.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()


f = ecss_path + "services/housekeeping_service.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

f = ecss_path + "services/test_service.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()


f = ecss_path + "core/hldlc.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

f = ecss_path + "core/packet_utilities.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

f = ecss_path + "core/packet_services.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

f = ecss_path + "core/pkt_pool.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

f = ecss_path + "core/queue.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

folder = "EPS_software/libs/"
f = path + "TMP100/TMP100.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

f = path + "INA226/INA226.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

f = path + "LTC2942/LTC2942.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

f = path + "MB85RS256A/MB85RS256A.c"
cmd = "ln -s " + f + " " + folder
pipe = subprocess.Popen(cmd, shell=True)
pipe.wait()

print "Input to ccs directories"
print ecss_path + "delfiPQ/EPS"
print ecss_path + "delfiPQ/EPS/HAL"
print ecss_path + "delfiPQ/OSAL"
print ecss_path + "delfiPQ"
print ecss_path + "core"
print ecss_path + "services"
print path + "INA226"
print path + "TMP100"
print path + "MB85RS256A"
print path + "LTC2942"

#print "Install pyserial, you need to have pip"
#pip.main(['install', "pyserial"])

print "Dont forget to:"
print "add the ti-rtos kernel (import from simplelink and add build dependency)"
print "Add c99 flag"
print "Modify simplelink uart driverlib for ecss packet handling"
