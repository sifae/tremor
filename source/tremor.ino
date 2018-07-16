#include <Wire.h>
//#include <TimerOne.h>

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
int k = 0;


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



long int ti;
volatile bool intFlag = true;

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
}





// Counter
long int cpt = 0;

void callback() {
  intFlag = true;
  digitalWrite(13, digitalRead(13) ^ 1);
}

void loop() {
  int emg = map(analogRead(A0), 0, 1023, 0, 255);         //Данные с мышц
  uint8_t Buf[14];
  I2Cread(MPU9250_ADDRESS, 0x3B, 14, Buf);                //Чтение данных с IMU

  int16_t ax = -(Buf[0] << 8 | Buf[1]);                   //Преобразование данных акселерометра
  int16_t ay = -(Buf[2] << 8 | Buf[3]);
  int16_t az = Buf[4] << 8 | Buf[5];

  int16_t gx = -(Buf[8] << 8 | Buf[9]);                   //Преолбразование данных гироскопа
  int16_t gy = -(Buf[10] << 8 | Buf[11]);
  int16_t gz = Buf[12] << 8 | Buf[13];

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



  uint8_t ST1;                                            //Чтение данных магнетометра
  do
  {
    I2Cread(MAG_ADDRESS, 0x02, 1, &ST1);
  }
  while (!(ST1 & 0x01));

  uint8_t Mag[7];
  I2Cread(MAG_ADDRESS, 0x03, 7, Mag);

  int16_t mx = -(Mag[3] << 8 | Mag[2]);                   //Преобразование данных для магнетометра
  int16_t my = -(Mag[1] << 8 | Mag[0]);
  int16_t mz = -(Mag[5] << 8 | Mag[4]);

  Serial.print (mx + 200, DEC);                           //Вывод данных в serial
  Serial.print ("\t");
  Serial.print (my - 70, DEC);
  Serial.print ("\t");
  Serial.print (mz - 700, DEC);
  Serial.print ("\t");
  Serial.println("");

  if (k <= 10000000) {
    k++;
  }
  else {
    k = 0;
    callback();
  }
}
