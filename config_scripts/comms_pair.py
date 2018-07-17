'''

Comms pairing script that pair's ROSE "ROSS Comms" boards

Requires either 2 or 3 command line arguments

comms_pair.py COM4 color function

where function is either 'ship'/'ground' or 'remote'/'ebox'


'''


import sys
import serial
import time

settings_file_name = "pair_info.txt"
baud = 57600

def change_modem_settings(com_port, baud, color):
    settings_file = open(settings_file_name, "r")
    print "Reading: "
    for line in settings_file.readlines():
        if line[0] == "#": #skip documentation lines
            continue
        print line.split('\t')
    
    settings_file.close()
    
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
    
    

    

if __name__ == "__main__":
    if len(sys.argv) == 3 or len(sys.argv) == 4:
        #print "proper args"
        
        print "Open " + sys.argv[1] + " port with baud " + str(baud)
        
        change_modem_settings(sys.argv[1], str(baud), sys.argv[2])
    else:
        print "Please use the correct number of arguments."
        quit()
        
        