#include <M5Stack.h>
#include "utility/MPU9250.h"
#include <BluetoothSerial.h>

#define Debug true

MPU9250 IMU;

BluetoothSerial bts;

int sumCount = 5;
float Buffer[5];
int count;

float Max, Min;

float monitoringForce[5];

float top;
bool dawnCheker;

void setup() {
  M5.begin();
  dacWrite(25, 0);//M5Stack固有のうるさい音を上書きして消す処理
  //  serial for debugging
  Serial.begin(9600);
  //  i2c as a master
  Wire.begin();
  IMU.initMPU9250();

  bts.begin("ESP32");//byShirai

  count = 0;
  for (int i = 0; i < sumCount; i++) {
    Buffer[i] = 9.8;
    monitoringForce[i] = 9.8;
  }

  Max = 0;
  Min = 8;

  top = 0.98;
}

void loop() {

  if (IMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
  {

    bool small = false;
    bool large = false;

    IMU.readAccelData(IMU.accelCount);  // Read the x/y/z adc values
    IMU.getAres();

    M5.Lcd.setTextSize(3);//M5Stackに表示する文字の大きさ
    M5.Lcd.setCursor(10, 10); //M5Stackに表示する文字の位置

    float x, y, z;

    x = (float)IMU.accelCount[0] * IMU.aRes ; //- accelBias[0]
    y = (float)IMU.accelCount[1] * IMU.aRes ; //* IMU.aRes
    z = (float)IMU.accelCount[2] * IMU.aRes ;

    M5.Lcd.print("x : "); M5.Lcd.print(x); M5.Lcd.print("\n");
    M5.Lcd.print("y : "); M5.Lcd.print(y); M5.Lcd.print("\n");
    M5.Lcd.print("z : "); M5.Lcd.print(z); M5.Lcd.print("\n");

    float force = sqrt((x * x) + (y * y) + (z * z));//forceの計算

    if (Debug)force =  bufferProcessing(force * 9.8) * 0.1;
    else force = force * 9.8 * 0.1;

    if (force > Max)Max = force;
    if (force < Min)Min = force;

    M5.Lcd.print("force : "); M5.Lcd.print(force); M5.Lcd.print("\n");
    M5.Lcd.print("Max : "); M5.Lcd.print(Max); M5.Lcd.print("\n");
    M5.Lcd.print("Min : "); M5.Lcd.print(Min);

    for (int i = 0; i < sumCount - 1; i++) {
      monitoringForce[i] = monitoringForce[i + 1];
    }
    monitoringForce[4] = force;

    if (monitoringForce[0] < monitoringForce[1]) {
      if (monitoringForce[1] < monitoringForce[2]) {
        if (monitoringForce[2] > monitoringForce[3]) {
          if (monitoringForce[3] > monitoringForce[4]) {
            top = monitoringForce[2];
            dawnCheker = true;
            //   bts.print("top");
          }
        }
      }
    }
    if (top > 4 && dawnCheker) {
      //bts.print("large : ");
      bts.print(top + monitoringForce[4]);
      top = 0;
    } else if (force <= 0.5) {
      //bts.print("small : ");
      bts.print(top + monitoringForce[4]);
      top = 0;
    } else {
      bts.print(0.00);
    }
    
    bts.print(",");
    bts.print(force);

    bts.println();

    count++;
    if (count >= sumCount) {
      count = 0;
    }


  }
}

float bufferProcessing(float rawDate) {
  float sum = 0;

  Buffer[count] = rawDate;

  for (int i = 0; i < sumCount; i++) {
    sum += Buffer[i];
  }

  float age = sum  / sumCount * 1.0f;

  return age;
}
