#define DEBUG 0 // 0 : RX0 TX1, 1: SoftSerial
uint8_t DEVICE_ID = 1;
const int RST_PIN = 2;

#if DEBUG
#include <SoftwareSerial.h>
int RX=2;
int TX=3;
SoftwareSerial BTSerial(RX, TX);
#endif

#include <am1008w_k_i2c.h>

void(* resetFunc) (void) = 0;
AM1008W_K_I2C am1008w_k_i2c;
// #define AM1008W_K_I2C_DEBUG

char state = '0';

typedef struct packet_s 
{ 
  uint8_t device_id;     // 1byte
  uint8_t voc;           // 1byte
  uint16_t fcnt;         // 2byte
  uint16_t co2;          // 2byte
  uint16_t humidity;     // 2byte
  uint16_t temp;         // 2byte 
  uint8_t pm1p0;         // 1byte
  uint8_t pm2p5;         // 1byte
  uint8_t pm10;          // 1byte
  uint8_t now_r_ref_r;   // 1byte
  uint16_t ref_r;        // 2byte
  uint16_t now_r;        // 2byte
}Packet_s;  //18byte
   
typedef union packet_u 
{ 
  Packet_s ps;  
  uint8_t pl[18]; 
}Packet_u;

Packet_u packet_data;

unsigned char CalcChecksum(unsigned char *data, int leng)
{
  unsigned char csum;

    csum = 0xFF;
    for (;leng > 0;leng--)
        csum += *data++;
    return ~csum;
}

void setup()
{
  digitalWrite(RST_PIN, HIGH);
  pinMode(RST_PIN, OUTPUT); 
#if DEBUG
  BTSerial.begin(115200);  
#endif
  am1008w_k_i2c.begin();
  Serial.begin(115200);
  delay(1000);

  packet_data.ps.device_id = DEVICE_ID;
  packet_data.ps.co2 = 0;
  packet_data.ps.voc = 0;
  packet_data.ps.humidity = 0;
  packet_data.ps.temp = 0;
  packet_data.ps.pm1p0 = 0;
  packet_data.ps.pm2p5 = 0;
  packet_data.ps.pm10 = 0;
  packet_data.ps.now_r_ref_r = 0;
  packet_data.ps.ref_r = 0;
  packet_data.ps.now_r = 0;
}

void loop()
{ 
#if DEBUG
  if(BTSerial.available())
  {
    String buf = BTSerial.readString();
    state = buf[0];
    packet_data.ps.fcnt = 0;
  }
#else
  if(Serial.available())
  {
    String buf = Serial.readString();
    state = buf[0];
    packet_data.ps.fcnt = 0;
  }
#endif
  if(state == '2') digitalWrite(RST_PIN, LOW);
  if (state == '1')
  {
    uint8_t ret = am1008w_k_i2c.read_data_command();
    
    packet_data.ps.co2 = am1008w_k_i2c.get_co2();
    packet_data.ps.voc = am1008w_k_i2c.get_voc();
    packet_data.ps.humidity = uint16_t(am1008w_k_i2c.get_humidity()*100);
    packet_data.ps.temp = uint16_t(am1008w_k_i2c.get_temperature()*100);
    packet_data.ps.pm1p0 = am1008w_k_i2c.get_pm1p0();
    packet_data.ps.pm2p5 = am1008w_k_i2c.get_pm2p5();
    packet_data.ps.pm10 = am1008w_k_i2c.get_pm10();
    packet_data.ps.now_r_ref_r = am1008w_k_i2c.get_voc_now_r_ref_r();
    packet_data.ps.ref_r = am1008w_k_i2c.get_voc_ref_r();
    packet_data.ps.now_r = am1008w_k_i2c.get_voc_now_r();
    
#if DEBUG
    BTSerial.write(packet_data.pl, sizeof(packet_data.pl));
    BTSerial.write(CalcChecksum((unsigned char*)packet_data.pl, sizeof(packet_data.pl)));
    BTSerial.write("\r\n");
#else
    Serial.write(packet_data.pl, sizeof(packet_data.pl));
    Serial.write(CalcChecksum((unsigned char*)packet_data.pl, sizeof(packet_data.pl)));
    Serial.write("\r\n");
#endif      
    packet_data.ps.fcnt++;
    delay(1000);
  }
}
