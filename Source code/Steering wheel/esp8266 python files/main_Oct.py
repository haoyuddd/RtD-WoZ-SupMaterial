# Complete project details at https://RandomNerdTutorials.com
# https://www.norwegiancreations.com/2015/10/tutorial-potentiometers-with-arduino-and-filtering/

from machine import sleep
import usocket as socket
import json

from machine import I2C
from machine import Pin
import mpu6050

i2c = I2C(scl=Pin(5), sda=Pin(4))       #initializing the I2C method for ESP8266
mpu= mpu6050.accel(i2c)

import esp
esp.osdebug(None)

button = machine.Pin(2, machine.Pin.IN)

minn = -1000
maxn =  1000
clampedOld = 450
EMA_a = 0.5
EMA_S = 450

def clamp(n, minn, maxn):
    if n < minn:
        return minn
    elif n > maxn:
        return maxn
    else:
        return n
    
#  Prominent Arduino map function :)
def map(x, in_min, in_max, out_min, out_max):
    return int((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)


UDP_IP = "192.168.4.2" 
UDP_PORT = 41234

# socket init
# sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

def send(message):
    # print(message, button.value())
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.sendto(message, (UDP_IP, UDP_PORT))

msg = {"angle": 450}
centre = json.dumps(msg)
send(centre)

# angle testing
# angles = [-400, 450, 1300]
# indexes = [0,1,2,1]

while True:
    over180 = False
    toTheRight = False
    GZOld = 0
    # steering wheel turning test
#     for x in indexes:
#         msg["angle"] = angles[x]
#         send(json.dumps(msg))
#         sleep(400)
    if button.value() == 0:
        msg["angle"] = 450
    else:
        # get acc sensor data, X axis
        vals = mpu.get_values()
        print(vals)
        
        #check X axis
        XAxis = vals.get('AcX')
        adjusted = map(XAxis, -17000, 17000, 250, 650) # Acx: -16500 to 16500 plus minus
        # clamped = clamp(adjusted,250,650)
        
        # check movement
        GZ = vals.get('GyZ')
        
        #check Y axis
        YAxis = vals.get('AcY')
        if YAxis > 0:
            if XAxis > 0:
                adjusted = 1300 - adjusted
            else:
                adjusted = 500 - adjusted
        toPos = (adjusted - 450) / 900 * 40 + 50
        print("to Pos: ", toPos)
    #     EMA_S = (EMA_a*clamped) + ((1-EMA_a)*EMA_S);
    #     clamped = EMA_S
    #     print(int(clamped))
        print(int(adjusted))
    #     msg["angle"] = int(clamped)
        
    #     if abs(clamped-clampedOld) > 15:
    #         send(json.dumps(msg))
    #         clampedOld = clamped
        # normally GZ: -300 ~ -400
        
        if not over180:
            # cross 180 check 50 <-> 850
            if abs(adjusted-clampedOld) > 600:
                over180 = True
                if GZ < -800:
                    toTheRight = True
                    msg["angle"] = 850
                else:
                    toTheRight = False
                    msg["angle"] = 50
            else:
                if abs(adjusted-clampedOld) > 15:
                    msg["angle"] = int(adjusted)
                    clampedOld = adjusted
        else:
            # over 180 degrees
            if ToTheRight:
                if GZ > 0 and adjusted < 850 and adjusted > 450:
                    msg["angle"] = int(adjusted)
                    over180 = False
                    clampedOld = adjusted
                else:
                    msg["angle"] = 850
            else:
                if GZ < -800 and adjusted > 50 and adjusted < 450:
                    msg["angle"] = int(adjusted)
                    over180 = False
                    clampedOld = adjusted
                else:
                    msg["angle"] = 50
        
        send(json.dumps(msg))
        sleep(50)


