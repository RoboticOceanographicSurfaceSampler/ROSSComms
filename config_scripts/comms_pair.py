'''

Comms pairing script that pair's ROSE "ROSS Comms" boards

Requires either 2 or 3 command line arguments

comms_pair.py COM4 color function

where function is either 'ship'/'ground' or 'remote'


'''


import sys
import serial
import time

baud = 57600

if __name__ == "__main__":
    if len(sys.argv) == 3 or len(sys.argv) == 4:
        print "proper args"
        
        print "Open " + sys.argv[1] + " port with baud " + str(baud)
        
        time.sleep(2)
        print "+++"
        time.sleep(2)
        print "ATDH XXX\n"  #Write high-address byte
        time.sleep(.1)
        print "ATDL XXX\n"  #Write low-address byte
        time.sleep(.1)
        print "ATWR\n"   #Write to non-volatile memory
        time.sleep(.1)
        print "ATCN\n"   #Exit
        time.sleep(.1)
         
    else:
        print "Please use the correct number of arguments."
        quit()
        
        