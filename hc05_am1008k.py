import serial 
from struct import *

ser = serial.Serial(port='COM11', 
                    baudrate=115200, 
                    ) 

def byte_xor(ba1, ba2):
    return bytes([_a ^ _b for _a, _b in zip(ba1, ba2)])

def CheckSum(init_chk, data):
    csum = int.from_bytes(init_chk, byteorder='little', signed=False)
    for i in range(len(data)):
        csum = csum+int.from_bytes(data[i:i+1], byteorder='little', signed=False)    
    csum = csum.to_bytes(2, byteorder='little', signed=False)
    return int.from_bytes(byte_xor(init_chk, csum[0:1]), byteorder='little', signed=False)
    

if __name__ == '__main__':
    ser.write(b'1');
    
    while True:
        try:
            if ser.readable():
                buf = ser.read_until(size=21, expected='\r\n')
                if(len(buf)==21):
                    if(CheckSum(init_chk=b'\xFF',data=buf[0:18]) == unpack("B", buf[18:19])[0]):
                        pl = unpack('BBHBHHBBBBHH', buf[0:18])  
                        device_id = pl[0]
                        voc = pl[1]
                        fcnt = pl[2]
                        co2 = pl[3]
                        humidity = pl[4]
                        temperature = pl[5]
                        pm1p0 = pl[6]
                        pm2p5 = pl[7]
                        pm10 = pl[8]
                        now_r_ref_r = pl[9]
                        ref_r = pl[10]
                        now_r = pl[11]
                        print(device_id,
                              fcnt, 
                              voc, 
                              co2, 
                              humidity, 
                              temperature, 
                              pm1p0, 
                              pm2p5, 
                              pm10, 
                              now_r_ref_r, 
                              ref_r, 
                              now_r)
        except:
            ser.write(b'2')
            print("[INFO] EXIT")
            break
