import serial


device = serial.Serial('/dev/cu.SLAB_USBtoUART')

var1=0
var2=0
var3=0
var4=0
var5=0
var6=0
var7=0
var8=0

BITMASK_MINMAX=0b11
BITMASK_SPEED_UNITS=0b11100
MAX_FLAG=0
MIN_FLAG=0

#Compute average wind over 10 seconds
windAverageList = [0,0,0,0,0,0,0,0,0,0]
currentIndex = 0



print "----------------------"

i = 0
while True:
    mostSignificant = ord(device.read())
    leastSignificant = ord(device.read())
    lastInt = (mostSignificant << 8) | (leastSignificant)
    if (i == 1):
        var1 = lastInt
    elif (i == 2):
        var2 = lastInt
    elif (i == 3):
        var3 = lastInt
    elif (i == 4):
        var4 = lastInt
    elif (i == 5):
        var5 = lastInt
    elif (i == 6):
        var6 = lastInt
    elif (i == 7):
        var7 = lastInt
    i = i + 1
    if (i >= 8):
        print "flags? = "+bin(var1)
        MINMAX_STATE = (var1 & BITMASK_MINMAX)
        MAX_FLAG = MINMAX_STATE & 0b01
        MIN_FLAG = MINMAX_STATE & 0b10
        if (MAX_FLAG):
            print "MAX MODE"
        if (MIN_FLAG):
            print "MIN MODE"
        print "percent relative humidity = "+str(var2/10.0)
        print "temperature = "+str((var3 / 10.0) - 30) +" c"
        #print "value on display = "+str(var4)
        actualWindSpeed = var5/100.0
        print "wind speed = "+str(actualWindSpeed)+" m/s"
        #print "flow air volume = "+str(var6)
        #print "air area setting = "+str(var7)
        #print "STOP BYTE = "+str(var8)
        print "----------------------"
        # wind warning if wind speed is over 10mph

        windAverageList[currentIndex] = actualWindSpeed
        currentIndex = (currentIndex + 1) % 10
        averageSpeed = sum(windAverageList) / len(windAverageList)
        windWarningState = (averageSpeed > 2)

        print "Average speed (last 10s): "+str(averageSpeed)+ " m/s"
        print "Wind warning state: "+str(windWarningState)
        print "----------------------"
        i=0
