#include <M5Stack.h>
#include "utility/MPU9250.h"
#include "utility/quaternionFilters.h"
#include "BluetoothSerial.h"

#define processing_out false

//基本データ読み取りの場合は false に設定
// Set to false for basic data read
#define AHRS true

//デバッグ用のシリアル出力を取得するには true に設定します。
// Set to true to get Serial output for debugging
#define SerialDebug false

#define LCD false//M5stackに出力したいときはtrue
#define ForceDebug true//力の値だけをM5stack及び、シリアルに出力したいときはtrue
BluetoothSerial bts;

MPU9250 IMU;
// Kalman kalmanX, kalmanY, kalmanZ; // Create the Kalman instances

void setup()
{
  M5.begin();
  Wire.begin();
  Serial.begin(9600);
  bts.begin("M5stackgray1");//PC側で確認するときの名前
  //bluetooth
  M5.Lcd.println("Bluetooth Now");
  delay(1000);


#if LCD
  // Start device display with ID of sensor
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE , BLACK); // Set pixel color; 1 on the monochrome screen
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 0); M5.Lcd.print("MPU9250");
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(0, 20); M5.Lcd.print("9-DOF 16-bit");
  M5.Lcd.setCursor(0, 30); M5.Lcd.print("motion sensor");
  M5.Lcd.setCursor(20, 40); M5.Lcd.print("60 ug LSB");
  delay(1000);

  // Set up for data display
  M5.Lcd.setTextSize(1); // Set text size to normal, 2 is twice normal etc.
  M5.Lcd.fillScreen(BLACK);   // clears the screen and buffer
#endif // LCD

  // Read the WHO_AM_I register, this is a good test of communication
  byte c = IMU.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
  bts.print("MPU9250 "); bts.print("I AM "); bts.print(c, HEX);
  bts.print(" I should be "); bts.println(0x71, HEX);

#if LCD
  M5.Lcd.setCursor(20, 0); M5.Lcd.print("MPU9250");
  M5.Lcd.setCursor(0, 10); M5.Lcd.print("I AM");
  M5.Lcd.setCursor(0, 20); M5.Lcd.print(c, HEX);
  M5.Lcd.setCursor(0, 30); M5.Lcd.print("I Should Be");
  M5.Lcd.setCursor(0, 40); M5.Lcd.print(0x71, HEX);
  delay(1000);
#endif // LCD

  // if (c == 0x71) // WHO_AM_I should always be 0x68
  {
    bts.println("MPU9250 is online...");

    // Start by performing self test and reporting values
    IMU.MPU9250SelfTest(IMU.SelfTest);
    bts.print("x-axis self test: acceleration trim within : ");
    bts.print(IMU.SelfTest[0], 1); bts.println("% of factory value");
    bts.print("y-axis self test: acceleration trim within : ");
    bts.print(IMU.SelfTest[1], 1); bts.println("% of factory value");
    bts.print("z-axis self test: acceleration trim within : ");
    bts.print(IMU.SelfTest[2], 1); bts.println("% of factory value");
    bts.print("x-axis self test: gyration trim within : ");
    bts.print(IMU.SelfTest[3], 1); bts.println("% of factory value");
    bts.print("y-axis self test: gyration trim within : ");
    bts.print(IMU.SelfTest[4], 1); bts.println("% of factory value");
    bts.print("z-axis self test: gyration trim within : ");
    bts.print(IMU.SelfTest[5], 1); bts.println("% of factory value");

    //ジャイロと加速度計のキャリブレーション、バイアスレジスタの負荷バイアス
    // Calibrate gyro and accelerometers, load biases in bias registers
    IMU.calibrateMPU9250(IMU.gyroBias, IMU.accelBias);

#if LCD
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setCursor(0, 0); M5.Lcd.print("MPU9250 bias");
    M5.Lcd.setCursor(0, 16); M5.Lcd.print(" x   y   z  ");

    M5.Lcd.setCursor(0,  32); M5.Lcd.print((int)(1000 * IMU.accelBias[0]));
    M5.Lcd.setCursor(32, 32); M5.Lcd.print((int)(1000 * IMU.accelBias[1]));
    M5.Lcd.setCursor(64, 32); M5.Lcd.print((int)(1000 * IMU.accelBias[2]));
    M5.Lcd.setCursor(96, 32); M5.Lcd.print("mg");

    M5.Lcd.setCursor(0,  48); M5.Lcd.print(IMU.gyroBias[0], 1);
    M5.Lcd.setCursor(32, 48); M5.Lcd.print(IMU.gyroBias[1], 1);
    M5.Lcd.setCursor(64, 48); M5.Lcd.print(IMU.gyroBias[2], 1);
    M5.Lcd.setCursor(96, 48); M5.Lcd.print("o/s");
    delay(1000);
#endif // LCD

    IMU.initMPU9250();
    // Initialize device for active mode read of acclerometer, gyroscope, and
    // temperature
    bts.println("MPU9250 initialized for active data mode....");

    // Read the WHO_AM_I register of the magnetometer, this is a good test of
    // communication
    byte d = IMU.readByte(AK8963_ADDRESS, WHO_AM_I_AK8963);
    bts.print("AK8963 "); bts.print("I AM "); bts.print(d, HEX);
    bts.print(" I should be "); bts.println(0x48, HEX);

#if LCD
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(20, 0); M5.Lcd.print("AK8963");
    M5.Lcd.setCursor(0, 10); M5.Lcd.print("I AM");
    M5.Lcd.setCursor(0, 20); M5.Lcd.print(d, HEX);
    M5.Lcd.setCursor(0, 30); M5.Lcd.print("I Should Be");
    M5.Lcd.setCursor(0, 40); M5.Lcd.print(0x48, HEX);
    delay(1000);
#endif // LCD

    // Get magnetometer calibration from AK8963 ROM
    IMU.initAK8963(IMU.magCalibration);
    // Initialize device for active mode read of magnetometer
    bts.println("AK8963 initialized for active data mode....");
    if (Serial)
    {
      //  Serial.println("Calibration values: ");
      bts.print("X-Axis sensitivity adjustment value ");
      bts.println(IMU.magCalibration[0], 2);
      bts.print("Y-Axis sensitivity adjustment value ");
      bts.println(IMU.magCalibration[1], 2);
      bts.print("Z-Axis sensitivity adjustment value ");
      bts.println(IMU.magCalibration[2], 2);
    }

#if LCD
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(20, 0); M5.Lcd.print("AK8963");
    M5.Lcd.setCursor(0, 10); M5.Lcd.print("ASAX "); M5.Lcd.setCursor(50, 10);
    M5.Lcd.print(IMU.magCalibration[0], 2);
    M5.Lcd.setCursor(0, 20); M5.Lcd.print("ASAY "); M5.Lcd.setCursor(50, 20);
    M5.Lcd.print(IMU.magCalibration[1], 2);
    M5.Lcd.setCursor(0, 30); M5.Lcd.print("ASAZ "); M5.Lcd.setCursor(50, 30);
    M5.Lcd.print(IMU.magCalibration[2], 2);
    delay(1000);
#endif // LCD
  } // if (c == 0x71)
  // else
  // {
  //   Serial.print("Could not connect to MPU9250: 0x");
  //   Serial.println(c, HEX);
  //   while(1) ; // Loop forever if communication doesn't happen
  // }

  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(GREEN , BLACK);
  M5.Lcd.fillScreen(BLACK);
}

void loop()
{
  //intPin が高くなると、すべてのデータ レジスタに新しいデータが含まれる
  //割り込み時に、データの準備ができている割り込みを確認します。
  // If intPin goes high, all data registers have new data
  // On interrupt, check if data ready interrupt
  if (IMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
  {

    IMU.readGyroData(IMU.gyroCount);  // Read the x/y/z adc values
    IMU.getGres();

    //ジャイロの値を実際の度/秒に計算する
    // Calculate the gyro value into actual degrees per second
    // This depends on scale being set
    IMU.gx = (float)IMU.gyroCount[0] * IMU.gRes;
    IMU.gy = (float)IMU.gyroCount[1] * IMU.gRes;
    IMU.gz = (float)IMU.gyroCount[2] * IMU.gRes;

    IMU.readAccelData(IMU.accelCount);  // Read the x/y/z adc values
    IMU.getAres();

    //実際のｇの蓄積値を計算する
    // Now we'll calculate the accleration value into actual g's
    // This depends on scale being set
    IMU.ax = (float)IMU.accelCount[0] * IMU.aRes;  - IMU.accelBias[0];
    IMU.ay = (float)IMU.accelCount[1] * IMU.aRes;  - IMU.accelBias[1];
    IMU.az = (float)IMU.accelCount[2] * IMU.aRes;  - IMU.accelBias[2];

    IMU.readMagData(IMU.magCount);  // Read the x/y/z adc values
    IMU.getMres();
    // User environmental x-axis correction in milliGauss, should be
    // automatically calculated
    IMU.magbias[0] = +470.;
    // User environmental x-axis correction in milliGauss TODO axis??
    IMU.magbias[1] = +120.;
    // User environmental x-axis correction in milliGauss
    IMU.magbias[2] = +125.;

    //ミリガウスで磁力計の値を計算する
    // Calculate the magnetometer values in milliGauss
    // Include factory calibration per data sheet and user environmental
    // corrections
    // Get actual magnetometer value, this depends on scale being set
    IMU.mx = (float)IMU.magCount[0] * IMU.mRes * IMU.magCalibration[0] -
             IMU.magbias[0];
    IMU.my = (float)IMU.magCount[1] * IMU.mRes * IMU.magCalibration[1] -
             IMU.magbias[1];
    IMU.mz = (float)IMU.magCount[2] * IMU.mRes * IMU.magCalibration[2] -
             IMU.magbias[2];
  } // if (readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)

  // Must be called before updating quaternions!
  IMU.updateTime();

  //加速度計のセンサーx(y)軸は、y(x)軸と整列している
  //磁力計;磁力計Z軸(+ダウン)はZ軸と反対である
  //加速度計とジャイロの(+アップ)!私たちはこのためにいくつかの手当をしなければならない
  //クォーターニオン フィルタに出力を供給する場合の方向の一致。の場合
  //MPU-9250は、センサーを前方に保つ磁気回転を選択しました
  //LSM9DS0 センサーと同様に、X 軸に沿って使用します。この回転は、
  //任意の便利な方向の規則を可能にするために変更されました。これは問題です。
  //航空機オリエンテーション規格!ジャイロレートを rad/s として渡す
  // Sensors x (y)-axis of the accelerometer is aligned with the y (x)-axis of
  // the magnetometer; the magnetometer z-axis (+ down) is opposite to z-axis
  // (+ up) of accelerometer and gyro! We have to make some allowance for this
  // orientationmismatch in feeding the output to the quaternion filter. For the
  // MPU-9250, we have chosen a magnetic rotation that keeps the sensor forward
  // along the x-axis just like in the LSM9DS0 sensor. This rotation can be
  // modified to allow any convenient orientation convention. This is ok by
  // aircraft orientation standards! Pass gyro rate as rad/s
  //  MadgwickQuaternionUpdate(ax, ay, az, gx*PI/180.0f, gy*PI/180.0f, gz*PI/180.0f,  my,  mx, mz);
  MahonyQuaternionUpdate(IMU.ax, IMU.ay, IMU.az, IMU.gx * DEG_TO_RAD,
                         IMU.gy * DEG_TO_RAD, IMU.gz * DEG_TO_RAD, IMU.my,
                         IMU.mx, IMU.mz, IMU.deltat);

  if (!ForceDebug)
  {
    if (!AHRS)
    {
      IMU.delt_t = millis() - IMU.count;
      if (IMU.delt_t > 500)
      {
        if (SerialDebug)
        {
          // Print acceleration values in milligs!
          bts.print("X-acceleration: "); bts.print(1000 * IMU.ax);
          bts.print(" mg ");
          bts.print("Y-acceleration: "); bts.print(1000 * IMU.ay);
          bts.print(" mg ");
          bts.print("Z-acceleration: "); bts.print(1000 * IMU.az);
          bts.println(" mg ");
          bts.print("");


          // Print gyro values in degree/sec
          bts.print("X-gyro rate: "); bts.print(IMU.gx, 3);
          bts.print(" degrees/sec ");
          bts.print("Y-gyro rate: "); bts.print(IMU.gy, 3);
          bts.print(" degrees/sec ");
          bts.print("Z-gyro rate: "); bts.print(IMU.gz, 3);
          bts.println(" degrees/sec");

          // Print mag values in degree/sec
          bts.print("X-mag field: "); bts.print(IMU.mx);
          bts.print(" mG ");
          bts.print("Y-mag field: "); bts.print(IMU.my);
          bts.print(" mG ");
          bts.print("Z-mag field: "); bts.print(IMU.mz);
          bts.println(" mG");

          IMU.tempCount = IMU.readTempData();  // Read the adc values
          //摂氏の温度
          // Temperature in degrees Centigrade
          IMU.temperature = ((float) IMU.tempCount) / 333.87 + 21.0;
          //印刷温度
          // Print temperature in degrees Centigrade
          bts.print("Temperature is ");  bts.print(IMU.temperature, 1);
          bts.println(" degrees C");
          bts.println("");
        }

#if (LCD)
        M5.Lcd.fillScreen(BLACK);
        M5.Lcd.setTextColor(GREEN , BLACK);
        M5.Lcd.setCursor(0, 0); M5.Lcd.print("MPU9250/AK8963");
        M5.Lcd.setCursor(0, 32); M5.Lcd.print(" x   y   z  ");

        M5.Lcd.setCursor(0,  48); M5.Lcd.print((int)(1000 * IMU.ax));
        M5.Lcd.setCursor(32, 48); M5.Lcd.print((int)(1000 * IMU.ay));
        M5.Lcd.setCursor(64, 48); M5.Lcd.print((int)(1000 * IMU.az));
        M5.Lcd.setCursor(96, 48); M5.Lcd.print("mg");

        M5.Lcd.setCursor(0,  64); M5.Lcd.print((int)(IMU.gx));
        M5.Lcd.setCursor(32, 64); M5.Lcd.print((int)(IMU.gy));
        M5.Lcd.setCursor(64, 64); M5.Lcd.print((int)(IMU.gz));
        M5.Lcd.setCursor(96, 64); M5.Lcd.print("o/s");

        M5.Lcd.setCursor(0,  96); M5.Lcd.print((int)(IMU.mx));
        M5.Lcd.setCursor(32, 96); M5.Lcd.print((int)(IMU.my));
        M5.Lcd.setCursor(64, 96); M5.Lcd.print((int)(IMU.mz));
        M5.Lcd.setCursor(96, 96); M5.Lcd.print("mG");

        M5.Lcd.setCursor(0,  128); M5.Lcd.print("Gyro T ");
        M5.Lcd.setCursor(50,  128); M5.Lcd.print(IMU.temperature, 1);
        M5.Lcd.print(" C");
#endif // LCD

        IMU.count = millis();
        // digitalWrite(myLed, !digitalRead(myLed));  // toggle led
      } // if (IMU.delt_t > 500)
    } // if (!AHRS)
    else
    {
      //データレートに依存しない0.5sレートでのシリアル印刷または表示
      // Serial print and/or display at 0.5 s rate independent of data rates
      IMU.delt_t = millis() - IMU.count;
      //読み取り速度に依存しない LCD を半秒に 1 回更新する
      // update LCD once per half-second independent of read rate
      // if (IMU.delt_t > 500)
      if (IMU.delt_t > 100)
      {
        if (SerialDebug)
        {
          bts.print("ax = "); bts.print((int)1000 * IMU.ax);
          bts.print(" ay = "); bts.print((int)1000 * IMU.ay);
          bts.print(" az = "); bts.print((int)1000 * IMU.az);
          bts.println(" mg");

          bts.print("gx = "); bts.print( IMU.gx, 2);
          bts.print(" gy = "); bts.print( IMU.gy, 2);
          bts.print(" gz = "); bts.print( IMU.gz, 2);
          bts.println(" deg/s");

          bts.print("mx = "); bts.print( (int)IMU.mx );
          bts.print(" my = "); bts.print( (int)IMU.my );
          bts.print(" mz = "); bts.print( (int)IMU.mz );
          bts.println(" mG");

          bts.print("q0 = "); bts.print(*getQ());
          bts.print(" qx = "); bts.print(*(getQ() + 1));
          bts.print(" qy = "); bts.print(*(getQ() + 2));
          bts.print(" qz = "); bts.println(*(getQ() + 3));
        }

        //正の Z 軸は地球に向かって下にあります。
        //ヨーはセンサー間の角度です
        //X軸と地球磁気北(またはローカルで修正された場合は真北)
        //センサーの正のヨーを見下ろす偏角は反時計回りです。
        //ピッチは、センサーX軸と地球地上平面の間の角度で、
        //地球は正、空に向かっては負です。ロールは間の角度です
        //センサーY軸と地球地上面、Y軸アップは正ロールです。これら
        //構築された均質な回転行列の定義から生じる
        //四分の一から。テット・ブライアンの角度とオイラーの角度は
        //非通勤;つまり、回転の正しい方向を取得します。
        //このコンフィギュレーションはヨーヨーである正しい順序で適用されなければならない
        // Define output variables from updated quaternion---these are Tait-Bryan
        // angles, commonly used in aircraft orientation. In this coordinate system,
        // the positive z-axis is down toward Earth. Yaw is the angle between Sensor
        // x-axis and Earth magnetic North (or true North if corrected for local
        // declination, looking down on the sensor positive yaw is counterclockwise.
        // Pitch is angle between sensor x-axis and Earth ground plane, toward the
        // Earth is positive, up toward the sky is negative. Roll is angle between
        // sensor y-axis and Earth ground plane, y-axis up is positive roll. These
        // arise from the definition of the homogeneous rotation matrix constructed
        // from quaternions. Tait-Bryan angles as well as Euler angles are
        // non-commutative; that is, the get the correct orientation the rotations
        // must be applied in the correct order which for this configuration is yaw,
        // pitch, and then roll.
        // For more see
        // http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
        // which has additional links.
        IMU.yaw   = atan2(2.0f * (*(getQ() + 1) * *(getQ() + 2) + *getQ() *
                                  *(getQ() + 3)), *getQ() * *getQ() + * (getQ() + 1) * *(getQ() + 1)
                          - * (getQ() + 2) * *(getQ() + 2) - * (getQ() + 3) * *(getQ() + 3));
        IMU.pitch = -asin(2.0f * (*(getQ() + 1) * *(getQ() + 3) - *getQ() *
                                  *(getQ() + 2)));
        IMU.roll  = atan2(2.0f * (*getQ() * *(getQ() + 1) + * (getQ() + 2) *
                                  *(getQ() + 3)), *getQ() * *getQ() - * (getQ() + 1) * *(getQ() + 1)
                          - * (getQ() + 2) * *(getQ() + 2) + * (getQ() + 3) * *(getQ() + 3));
        IMU.pitch *= RAD_TO_DEG;
        IMU.yaw   *= RAD_TO_DEG;
        // Declination of SparkFun Electronics (40°05'26.6"N 105°11'05.9"W) is
        //   8° 30' E  ± 0° 21' (or 8.5°) on 2016-07-19
        // - http://www.ngdc.noaa.gov/geomag-web/#declination
        IMU.yaw   -= 8.5;
        IMU.roll  *= RAD_TO_DEG;

        if (SerialDebug)
        {
          bts.print("Yaw, Pitch, Roll: ");
          bts.print(IMU.yaw, 2);
          bts.print(", ");
          bts.print(IMU.pitch, 2);
          bts.print(", ");
          bts.println(IMU.roll, 2);

          bts.print("rate = ");
          bts.print((float)IMU.sumCount / IMU.sum, 2);
          bts.println(" Hz");
          bts.println("");
        }

#if (LCD)
        // M5.Lcd.fillScreen(BLACK);
        M5.Lcd.setTextFont(2);

        M5.Lcd.setCursor(0, 0); M5.Lcd.print("     x       y       z ");
        M5.Lcd.setCursor(0,  24);
        M5.Lcd.printf("% 6d  % 6d  % 6d     mg   \r\n",  (int)(1000 * IMU.ax), (int)(1000 * IMU.ay), (int)(1000 * IMU.az));
        M5.Lcd.setCursor(0,  44);
        M5.Lcd.printf("% 6d  % 6d  % 6d      o/s  \r\n", (int)(IMU.gx), (int)(IMU.gy), (int)(IMU.gz));
        M5.Lcd.setCursor(0,  64);
        M5.Lcd.printf("% 6d  % 6d  % 6d     mG    \r\n",  (int)(IMU.mx), (int)(IMU.my), (int)(IMU.mz));

        M5.Lcd.setCursor(0,  100);
        M5.Lcd.printf("  yaw: % 5.2f    pitch: % 5.2f    roll: % 5.2f   \r\n", (IMU.yaw), (IMU.pitch), (IMU.roll));

        //これらの設定により、フィルタは~145 Hz のレートで更新されます。
        //マホニースキームを使用してマホニースキームと>200 Hz
        //表示は 2 Hz のみで更新されます。フィルタの更新レートが決定されます。
        //主にそれぞれのアルゴリズムの数学的ステップによって、
        //プロセッサ速度(3.3V Pro Miniの場合は8MHz)、磁力計ODR:
        //磁力計のための10 HzのODRは、上記のレート、最大を生成します
        //100 Hzの磁力計ODRは36 - 145のフィルタ更新速度を生成し、
        //マドウィックスキームとマホニースキームの場合は、それぞれ38Hzです。これは
        //磁力計の読み取りはジャイロよりも時間がかかるため、または
        //加速度計の読み取り。このフィルタの更新レートは、
        //の安定化制御のための正確なプラットホームの向きを維持する
        //動きの速いロボットかクワッドコプター。200 Hz の更新レートと比較する
        //インベンセンスのMPU6050のオンボードデジタルモーションプロセッサによって生成される
        //6 DoF および MPU9150 9DoF センサー。3.3 V 8 MHzプロミニはかなりやっています
        //まぁ！
        // With these settings the filter is updating at a ~145 Hz rate using the
        // Madgwick scheme and >200 Hz using the Mahony scheme even though the
        // display refreshes at only 2 Hz. The filter update rate is determined
        // mostly by the mathematical steps in the respective algorithms, the
        // processor speed (8 MHz for the 3.3V Pro Mini), and the magnetometer ODR:
        // an ODR of 10 Hz for the magnetometer produce the above rates, maximum
        // magnetometer ODR of 100 Hz produces filter update rates of 36 - 145 and
        // ~38 Hz for the Madgwick and Mahony schemes, respectively. This is
        // presumably because the magnetometer read takes longer than the gyro or
        // accelerometer reads. This filter update rate should be fast enough to
        // maintain accurate platform orientation for stabilization control of a
        // fast-moving robot or quadcopter. Compare to the update rate of 200 Hz
        // produced by the on-board Digital Motion Processor of Invensense's MPU6050
        // 6 DoF and MPU9150 9DoF sensors. The 3.3 V 8 MHz Pro Mini is doing pretty
        // well!

        // M5.Lcd.setCursor(0, 60);
        // M5.Lcd.printf("yaw:%6.2f   pitch:%6.2f   roll:%6.2f  ypr \r\n",(IMU.yaw), (IMU.pitch), (IMU.roll));
        M5.Lcd.setCursor(12, 144);
        M5.Lcd.print("rt: ");
        M5.Lcd.print((float) IMU.sumCount / IMU.sum, 2);
        M5.Lcd.println(" Hz");
#endif // LCD

        IMU.count = millis();
        IMU.sumCount = 0;
        IMU.sum = 0;

#if(processing_out)

        bts.print(((IMU.yaw)));    bts.print(";");
        bts.print(((IMU.pitch))); bts.print(";");
        bts.print(((IMU.roll)));   bts.print(";");
        bts.print(26.5);    bts.print(";");
        bts.print(0.01);    bts.print(";");
        bts.print(0.02);    bts.println();
#endif
      } // if (IMU.delt_t > 500)
    } // if (AHRS)
  }//if(!ForceDebug)
  else
  {
    IMU.delt_t = millis() - IMU.count;
    //読み取り速度に依存しない 0.1秒に 1 回更新する
    if ((IMU.delt_t > 100))
    {
      //重力加速度の値を求める
      float force = sqrt((IMU.ax * IMU.ax) + (IMU.ay * IMU.ay) + (IMU.az * IMU.az));
      bts.print(force); bts.print(",");
      bts.println("");

      M5.Lcd.setCursor(0,  0);
      M5.Lcd.setTextSize(3);
      M5.Lcd.print("force:"); M5.Lcd.print(force);

      IMU.count = millis();
      IMU.sumCount = 0;
      IMU.sum = 0;
    }//if (IMU.delt_t > 100)
  }

}
