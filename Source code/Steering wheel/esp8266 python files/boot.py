# This file is executed on every boot (including wake-boot from deepsleep)
#import esp
#esp.osdebug(None)
import uos, machine
#uos.dupterm(None, 1) # disable REPL on UART(0)
import gc
#import webrepl
#webrepl.start()
gc.collect()

import network
import machine
from machine import Pin

led = Pin(0, Pin.OUT)
led.value(1)
#button = machine.Pin(2, machine.Pin.IN)

ssid = 'Espace'
password = '987654321'

ap = network.WLAN(network.AP_IF)
ap.active(True)
ap.config(essid=ssid, password=password)

while ap.active() == False:
  pass

print('Connection successful')
print(ap.ifconfig())

led.value(0)




