#include <Wire.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include "base64.h"

#define    MPU9250_ADDRESS            0x68
#define    MAG_ADDRESS                0x0C

#define    GYRO_FULL_SCALE_250_DPS    0x00
#define    GYRO_FULL_SCALE_500_DPS    0x08
#define    GYRO_FULL_SCALE_1000_DPS   0x10
#define    GYRO_FULL_SCALE_2000_DPS   0x18

#define    ACC_FULL_SCALE_2_G        0x00
#define    ACC_FULL_SCALE_4_G        0x08
#define    ACC_FULL_SCALE_8_G        0x10
#define    ACC_FULL_SCALE_16_G       0x18

#define ssid "Todnetz_3"
#define password "minigore"
#define addr "http://192.168.43.150/"

ESP8266WiFiMulti WiFiMulti;

int k = 0;
String data_string;
String a;

// Counter
long int cpt = 0;

long int ti;
volatile bool intFlag = true;

int16_t ax,ay,az;
int16_t gx,gy,gz;
int16_t mx,my,mz;

uint8_t Mag[7];                                                                   //Буфер для хранения данных магнетрона
uint8_t Buf[14];                                                                  //Буфер для хранения данных акселерометра + гироскопа

void I2Cread(uint8_t Address, uint8_t Register, uint8_t Nbytes, uint8_t* Data) {  //Функция чтения по I2C из регистра
  Wire.beginTransmission(Address);
  Wire.write(Register);
  Wire.endTransmission();
  Wire.requestFrom(Address, Nbytes);
  uint8_t index = 0;
  while (Wire.available())
    Data[index++] = Wire.read();
}


void I2CwriteByte(uint8_t Address, uint8_t Register, uint8_t Data) {             //Функция записи по I2C в регистр
  Wire.beginTransmission(Address);
  Wire.write(Register);
  Wire.write(Data);
  Wire.endTransmission();
}

String sendtoserver(String data){
  String response = "None";
  if((WiFiMulti.run() == WL_CONNECTED)) {
        HTTPClient http;                  //Http begin
        http.setTimeout(500000);
        
        String request = addr + base64::encode(data);
        http.begin(request); //HTTP
  
        int httpCode = http.GET();      //Http Get
        
        if(httpCode > 0) {
          if(httpCode == HTTP_CODE_OK) {
            response = http.getString();
          }
          else{
            response = "Unknown Response";
          }
        } 
        else {
           response = http.errorToString(httpCode).c_str();
        }
        http.end();
    }
    return response;
}

void setup()
{
  Wire.begin();
  Serial.begin(115200);
  I2CwriteByte(MPU9250_ADDRESS, 29, 0x06);                                        //Фильтр акселерометра на 5Гц
  I2CwriteByte(MPU9250_ADDRESS, 26, 0x06);                                        //Фильтр гироскопа на 5Гц
  I2CwriteByte(MPU9250_ADDRESS, 27, GYRO_FULL_SCALE_1000_DPS);                    //настройка радиуса гироскопа
  I2CwriteByte(MPU9250_ADDRESS, 28, ACC_FULL_SCALE_4_G);                          //Настройка радиуса акселерометра
  I2CwriteByte(MPU9250_ADDRESS, 0x37, 0x02);
  I2CwriteByte(MAG_ADDRESS, 0x0A, 0x16);                                          //Настройка разрешения магнетометра
  pinMode(D6, OUTPUT);
  pinMode(A0, INPUT);
  ti = millis();
  delay(4000);
  WiFiMulti.addAP(ssid, password);
}


void callback() {
  intFlag = true;
  digitalWrite(13, digitalRead(13) ^ 1);
}

void out(){                           //Вывод данных в Serial
  //ax,ay,az - ускорение по x,y,z
  //gx,gy,gz - угловая скорость по x,y,z
  //mx,my,mz - курс по x,y,z
  Serial.print (ax, DEC);                                 //Вывод данных в serial
  Serial.print ("\t");                    
  Serial.print (ay, DEC);
  Serial.print ("\t");
  Serial.print (az, DEC);
  Serial.print ("\t");
  Serial.print (gx, DEC);
  Serial.print ("\t");
  Serial.print (gy, DEC);
  Serial.print ("\t");
  Serial.print (gz, DEC);
  Serial.print ("\t");
  Serial.print (mx + 200, DEC);                           //Вывод данных в serial
  Serial.print ("\t");
  Serial.print (my - 70, DEC);
  Serial.print ("\t");
  Serial.print (mz - 700, DEC);
  Serial.print ("\t");
  Serial.println("");
}

void Accelerometer_data_conversion(){                    //Преобразование данных акселерометра
  ax = -(Buf[0] << 8 | Buf[1]);                   
  ay = -(Buf[2] << 8 | Buf[3]);
  az = Buf[4] << 8 | Buf[5];
}

void Gyroscope_data_conversion(){                        //Преобразование данных гироскопа
  gx = -(Buf[8] << 8 | Buf[9]);                   
  gy = -(Buf[10] << 8 | Buf[11]);
  gz = Buf[12] << 8 | Buf[13];
}

void Magnetometer_data_conversion(){                     //Преобразование данных для магнетометра
  mx = -(Mag[3] << 8 | Mag[2]);                   
  my = -(Mag[1] << 8 | Mag[0]);
  mz = -(Mag[5] << 8 | Mag[4]);
}

void loop() {
  int emg = map(analogRead(A0), 0, 1023, 0, 255);         //Данные с мышц
  
  I2Cread(MPU9250_ADDRESS, 0x3B, 14, Buf);                //Чтение данных с IMU
  
  Accelerometer_data_conversion();
  Gyroscope_data_conversion();
  
  data_string = String(ax) + " " + String(ay) + " " + String(az) +
  " " + String(gx) + " " + String(gy) + " " + String(gz);
  a = sendtoserver(data_string);
  Serial.println(a);
  
  uint8_t ST1;                                            //Чтение данных магнетометра
  do
  {
    I2Cread(MAG_ADDRESS, 0x02, 1, &ST1);
  }
  while (!(ST1 & 0x01));

  I2Cread(MAG_ADDRESS, 0x03, 7, Mag);

  Magnetometer_data_conversion();

  if (k <= 10000000) {
    k++;
  }
  else {
    k = 0;
    callback();
  }
}
