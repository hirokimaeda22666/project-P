#include <Wire.h>
#include <M5Stack.h>
#include <BluetoothSerial.h>

// MMA8451 レジスタアドレス定義
#define AD_MMA8451 0x1D //gyro
#define MMA8451_REG_OUT_X_MSB     0x01   // 14 bit X Data R 
#define MMA8451_REG_SYSMOD        0x0B   // System Mode R
#define MMA8451_REG_WHOAMI        0x0D   // ID Register R
#define MMA8451_REG_XYZ_DATA_CFG  0x0E   // Data Config R/W
#define MMA8451_REG_PL_STATUS     0x10   // PL Status R
#define MMA8451_REG_PL_CFG        0x11   // PL Configuration R/W
#define MMA8451_REG_CTRL_REG1     0x2A   // Control Reg1 R/W
#define MMA8451_REG_CTRL_REG2     0x2B   // Control Reg2 R/W
#define MMA8451_REG_CTRL_REG4     0x2D   // Control Reg4
#define MMA8451_REG_CTRL_REG5     0x2E   // Control Reg5

//標準重力加速度
#define SENSORS_GRAVITY_EARTH             (9.80665F)              /**< Earth's gravity in m/s^2 */
#define SENSORS_GRAVITY_STANDARD          (SENSORS_GRAVITY_EARTH)


// MMA8451 スケールレンジ定義
typedef enum
{
  MMA8451_RANGE_8_G           = 0b10,   // +/- 8g
  MMA8451_RANGE_4_G           = 0b01,   // +/- 4g
  MMA8451_RANGE_2_G           = 0b00    // +/- 2g (default value)
} mma8451_range_t;

// MMA8451 XYZセンサ生データ(14bitデータを格納)
int16_t x, y, z;

//  MMA8451 XYZ角加速度(スケールレンジ向けに割った値を格納 標準重力加速度は未考慮)
float x_g, y_g, z_g;

//　MMA8451のアドレスを設定する
uint8_t mma8451_adr = 0;


/**********************************************
   MMA8451(スレーブ)からArduino(マスタ)に対して送られたデータを受信する
   requestFrom()でMMA8451に対して応答データの要求を行ったあとに実行すること
 *********************************************/
static inline uint8_t i2cread(void) {
  return Wire.read();
}

/**********************************************
   Arduino(マスタ)からMMA8451(スレーブ)に対してデータ送信する
   beginTransmission()とendTransmission()の間で実行すること
 *********************************************/
static inline void i2cwrite(uint8_t x) {
  Wire.write((uint8_t)x);
}

/******************************************
   MMA8451の指定レジスタにデータを書き込む
 *****************************************/
void writeMMA8451_Register(uint8_t reg, uint8_t value) {
  // 指定したI2Cのスレーブ(MMA8451)に対して送信処理を始める
  Wire.beginTransmission(mma8451_adr);

  //書き込みレジスタを指定する
  i2cwrite((uint8_t)reg);

  //データを書き込む
  i2cwrite((uint8_t)(value));

  // 送信完了
  Wire.endTransmission();
}

/******************************************
   MMA8451の指定レジスタからデータを読み出す
 *****************************************/
uint8_t readMMA8451_Register(uint8_t reg) {
  // 指定したI2Cのスレーブ(MMA8451)に対して送信処理を始める
  Wire.beginTransmission(mma8451_adr);

  // 読み込みたいレジスタを送信
  i2cwrite(reg);

  // 送信完了(コネクションは維持する)
  Wire.endTransmission(false);

  // MMA8451に対して1byteのデータを応答データに要求
  Wire.requestFrom(mma8451_adr, 1);

  // readで読み取れるバイト数がなければエラー
  if (! Wire.available()) return -1;

  // MMA8451から1byteのデータを取得して返却
  return (i2cread());
}

/******************************************
   MMA8451とi2c通信を開始する(初期化処理)
 *****************************************/
bool MMA8451_begin(uint8_t i2caddr)
{
  //  Wireライブラリの初期化 I2Cバスにマスタかスレーブとして接続
  //  マスタとしてバスに接続する。
  Wire.begin();
  mma8451_adr = i2caddr;

  //デバイスリセット有効
  writeMMA8451_Register(MMA8451_REG_CTRL_REG2, 0x40); // reset

  //デバイスリセット有効になるまで待ち
  while (readMMA8451_Register(MMA8451_REG_CTRL_REG2) & 0x40);

  // フルスケール 4gに設定
  writeMMA8451_Register(MMA8451_REG_XYZ_DATA_CFG, MMA8451_RANGE_4_G);

  // Powerモード
  // 0x00 normal
  // 0x01 Low Noise Low Power
  // 0x02 High Resolution
  // 0x03 Low Power
  writeMMA8451_Register(MMA8451_REG_CTRL_REG2, 0x02);

  // DRDY on INT1
  writeMMA8451_Register(MMA8451_REG_CTRL_REG4, 0x01);
  writeMMA8451_Register(MMA8451_REG_CTRL_REG5, 0x01);

  // Turn on orientation config
  writeMMA8451_Register(MMA8451_REG_PL_CFG, 0x40);

  // Activate at max rate, low noise mode
  writeMMA8451_Register(MMA8451_REG_CTRL_REG1, 0x01 | 0x04);

  return true;
}

/******************************************
   スケールレンジを取得する

    0x00 2g
    0x01 4g
    0x02 8g
    0x03 reserved
 *****************************************/
mma8451_range_t MMA8451_getRange(void)
{
  return (mma8451_range_t)(readMMA8451_Register(MMA8451_REG_XYZ_DATA_CFG) & 0x03);
}

/******************************************
   スケールレンジを設定する

    0x00 2g
    0x01 4g
    0x02 8g
    0x03 reserved
 *****************************************/
void MMA8451_setRange(mma8451_range_t range)
{
  uint8_t reg1 = readMMA8451_Register(MMA8451_REG_CTRL_REG1);
  writeMMA8451_Register(MMA8451_REG_CTRL_REG1, 0x00);            // deactivate
  writeMMA8451_Register(MMA8451_REG_XYZ_DATA_CFG, range & 0x3);
  writeMMA8451_Register(MMA8451_REG_CTRL_REG1, reg1 | 0x01);     // activate
}


/******************************************
   MMA8451から、XYZ軸のデータを取得する
 *****************************************/
void MMA8451_read(void) {
  // 指定したI2Cのスレーブ(MMA8451)に対して送信処理を始める
  Wire.beginTransmission(mma8451_adr);

  // データ取得開始アドレスはX軸上位バイトのレジスタ
  i2cwrite(MMA8451_REG_OUT_X_MSB);

  // 送信完了(コネクションは維持する)
  Wire.endTransmission(false); // MMA8451 + friends uses repeated start!!

  // MMA8451に対して6byteのデータを応答データに要求
  Wire.requestFrom(mma8451_adr, 6);

  // X軸レジスタのアドレスら計6byte取得する(X軸:14bit, Y軸:14bit, Z軸:14bit)
  /* MMA8451_REG_OUT_X_MSB: 0x01  X軸上位バイト
                            0x02  X軸下位バイト
                            0x03  Y軸上位バイト
                            0x04  Y軸下位バイト
                            0x05  Z軸上位バイト
                            0x06  Z軸下位バイト
  */
  x = Wire.read(); x <<= 8; x |= Wire.read(); x >>= 2;
  y = Wire.read(); y <<= 8; y |= Wire.read(); y >>= 2;
  z = Wire.read(); z <<= 8; z |= Wire.read(); z >>= 2;

  // スケールレンジを取得
  uint8_t range = MMA8451_getRange();
  uint16_t divider = 1;

  /* 角加速度はスケールレンジによって計算方法が異なる
         14bitデータ      Range±2g    Range±4g  Range±8g
      01 1111 1111 1111   1.99975g     +3.9995g   +7.999g
      データシートのTable75の表を参照
  */
  if (range == MMA8451_RANGE_8_G) divider = 1024;
  if (range == MMA8451_RANGE_4_G) divider = 2048;
  if (range == MMA8451_RANGE_2_G) divider = 4096;

  x_g = (float)x / divider;
  y_g = (float)y / divider;
  z_g = (float)z / divider;
}

/***************************************
 * 以下からがshiraiが調整しているプログラム
****************************************/

BluetoothSerial bts;

int sumCount = 10; //byShirai
float buffer[10][3];//byShirai
int count;//byShirai

float value[3];//byShirai


void setup() {
  Serial.begin(9600);
  M5.begin();
  dacWrite(25, 0);//M5Stack固有のうるさい音を上書きして消す処理//byShirai
  /* i2c通信初期化(MMA8451と通信するものとする */
  
  M5.Lcd.setTextSize(3);//byShirai
  M5.Lcd.setCursor(10,10);//byShirai
  M5.Lcd.println("Bluetooth Couldnt start");//byShirai
  bts.begin("ESP32");//byShirai

  M5.Lcd.print("MMA8451 Couldnt start");//byShirai
  if (!MMA8451_begin(AD_MMA8451))  {
    Serial.println("Couldnt start");//byShirai
    while (1);
  }
  M5.Lcd.print("MMA8451 start");
  
    
  /* スケールレンジは Range±2g とする*/
  MMA8451_setRange(MMA8451_RANGE_8_G);

  count = 0;//byShirai
  for (int i = 0; i < sumCount; i++) {//byShirai
    for (int j = 0; j < 3; j++) {//byShirai
      buffer[i][j] = 0;//byShirai
      value[j] = 0;//byShirai
    }//byShirai
  }//byShirai
  M5.Lcd.clear();//byShirai
}

void loop() {
  M5.update();
  //M5.Lcd.clear();
  // MMA8451からXYZ軸の生データを読み込む
  MMA8451_read();

  int now = 0;//byShirai
  float x = bufferProcessing(x_g * SENSORS_GRAVITY_STANDARD, now);//byShirai
  now++;//byShirai
  float y = bufferProcessing(y_g * SENSORS_GRAVITY_STANDARD, now);//byShirai
  now++;//byShirai
  float z = bufferProcessing(z_g * SENSORS_GRAVITY_STANDARD, now);//byShirai
  
  float force = sqrt((x * x) + (y * y) + (z * z));//forceの計算//byShirai
  bts.print(force); bts.print(",");//bluetooth通信でPCにforceの値をシリアル通信//byShirai

  bts.println();//bluetooth通信でPCに改行をシリアル通信//byShirai
  
  M5.Lcd.setTextSize(3);//M5Stackに表示する文字の大きさ//byShirai
  M5.Lcd.setCursor(10,10);//M5Stackに表示する文字の位置//byShirai
  M5.Lcd.print("force : ");M5.Lcd.print(force);//M5Stackに表示する文字//byShirai

  count++;//byShirai
  if (count >= sumCount) {//byShirai
    count = 0;//byShirai
  }//byShirai
  //delay(100); //2019/9/18のオープンキャンパス時に入っていた為遅延の原因になったと考えられるdelay処理//byShirai
}

float bufferProcessing(float rawDate, int now) {//各値(x,y,z)の平均化の処理//byShirai

  buffer[count][now] = rawDate + value[now] * (-1);//byShirai

  int sum = 0;//byShirai
  for (int i = 0; i < sumCount; i++) {//byShirai
    sum += buffer[i][now];//byShirai
  }//byShirai

  float age = sum  / sumCount * 1.0f;//byShirai

  return age;//byShirai
}//byShirai
