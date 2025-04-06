#include "LoRaWan_APP.h"
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BME280.h>

/*
===========================================================================================================================================
    LORAWAN VARS & CONFIG
===========================================================================================================================================
*/
/* OTAA para*/  // --> "MB DEVICE" in TTN console
uint8_t devEui[]  = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x06, 0xC9, 0xF3 };
uint8_t appEui[]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02 };
uint8_t appKey[]  = { 0x3C, 0x77, 0x92, 0x13, 0x16, 0xE9, 0xDE, 0x6C, 0xA8, 0xB3, 0xF0, 0x60, 0x3F, 0x5C, 0x9F, 0xDE };

/* ABP para*/
uint8_t nwkSKey[] = { 0x15, 0xb1, 0xd0, 0xef, 0xa4, 0x63, 0xdf, 0xbe, 0x3d, 0x11, 0x18, 0x1e, 0x1e, 0xc7, 0xda,0x85 };
uint8_t appSKey[] = { 0xd7, 0x2c, 0x78, 0x75, 0x8c, 0xdc, 0xca, 0xbf, 0x55, 0xee, 0x4a, 0x77, 0x8d, 0x16, 0xef,0x67 };
uint32_t devAddr  =  ( uint32_t )0x007e6ae1;

/*LoraWan channelsmask, default channels 0-7*/ 
uint16_t userChannelsMask[6]={ 0x00FF,0x0000,0x0000,0x0000,0x0000,0x0000 };

/*LoraWan region, select in arduino IDE tools*/
LoRaMacRegion_t loraWanRegion   = ACTIVE_REGION;

/*LoraWan Class, Class A and Class C are supported*/
DeviceClass_t  loraWanClass     = CLASS_C;

/*the application data transmission duty cycle.  value in [ms].*/
uint32_t appTxDutyCycle         = 60000;

/*OTAA or ABP*/
bool overTheAirActivation       = true;

/*ADR enable*/
bool loraWanAdr                 = true;

/* Indicates if the node is sending confirmed or unconfirmed messages */
bool isTxConfirmed              = true;

/* Application port */
uint8_t appPort                 = 2;
/*
* Note, that if NbTrials is set to 1 or 2, the MAC will not decrease
* the datarate, in case the LoRaMAC layer did not receive an acknowledgment
*/
uint8_t confirmedNbTrials       = 4;

/*
===========================================================================================================================================
    IMU & BME VARS & CONFIG
===========================================================================================================================================
*/
// I2C Adresses
#define IMU1_ADDR 0x68  // MPU 1 on Wire 0
#define IMU2_ADDR 0x68  // MPU 2 on Wire 1
#define IMU3_ADDR 0x69  // MPU 3 on Wire 1

// MPU6050 Register Addresses
#define MPU6050_SMPLRT_DIV            0x19
#define MPU6050_MPU_CONFIG            0x1A
#define MPU6050_GYRO_CONFIG           0x1B
#define MPU6050_ACCEL_CONFIG          0x1C
#define MPU6050_INT_PIN_CFG           0x37
#define MPU6050_INT_ENABLE            0x38
#define MPU6050_INT_STATUS            0x3A
#define MPU6050_ACCEL_XOUT_H          0x3B
#define MPU6050_ACCEL_XOUT_L          0x3C
#define MPU6050_PWR_MGMT_1            0x6B // Device defaults to the SLEEP mode
#define MPU6050_WHO_AM_I_MPU6050      0x75 // Should return 0x68
#define MPU6050_WHOAMI_DEFAULT_VALUE  0x68

// Vars for IMU data
const int num_imu_data  = 6;              // Gyro x3 et accel x3
int16_t imu_data[num_imu_data];           // store data read from 1 IMU

const byte imu_packet_len = 241;          // # values in IMU packet
const byte weather_packet_len = 5;        // # values in WEATHER packet
const byte num_iter = 80;                 // # iterations that will be stored in imu_packet


// Init Temp/Hum sensor
Adafruit_BME280 bme; // I2C

// variable used in mapping of data in prepareTxFrame frame
int float_to_int_factor = 10;             // to keep a precision of .1, we multiply the float by 10 to keep the .x value in the int.

/*
===========================================================================================================================================
    OTHER VARS & CONFIG
===========================================================================================================================================
*/
const int led_pin = 0;
int led_state = LOW;

/*
===========================================================================================================================================
    FUNCTIONS & METHODS
===========================================================================================================================================
*/

/*
=== FUNCTIONS & METHODS : IMU & BME
*/
// Function to write during IMU INIT (Taken from FastIMU library)
void writeByteI2C(TwoWire& wire, uint8_t address, uint8_t subAddress, uint8_t data) {
    wire.beginTransmission(address);  // Initialize the Tx buffer
    wire.write(subAddress);           // Put slave register address in Tx buffer
    wire.write(data);                 // Put data in Tx buffer
    wire.endTransmission();           // Send the Tx buffer
}

// Function to read during IMU INIT (Taken from FastIMU library)
uint8_t readByteI2C(TwoWire& wire, uint8_t address, uint8_t subAddress) {
    uint8_t data;                           // `data` will store the register data
    wire.beginTransmission(address);        // Initialize the Tx buffer
    wire.write(subAddress);                 // Put slave register address in Tx buffer
    wire.endTransmission(false);            // Send the Tx buffer, but send a restart to keep connection alive
    wire.requestFrom(address, (uint8_t)1);  // Read one byte from slave register address
    data = wire.read();                     // Fill Rx buffer with result
    return data;                            // Return data read from slave register
}

// Funtion to INIT IMU on specific I2C bus (Taken from FastIMU library)
int init_imu(TwoWire& i2c_bus, byte IMUAddress) {

	if (!(readByteI2C(i2c_bus, IMUAddress, MPU6050_WHO_AM_I_MPU6050) == MPU6050_WHOAMI_DEFAULT_VALUE)) {
		return -1;
	}

	// reset device
	writeByteI2C(i2c_bus, IMUAddress, MPU6050_PWR_MGMT_1, 0x80); // Write a one to bit 7 reset bit; toggle reset device
	delay(100);
	// wake up device
	writeByteI2C(i2c_bus, IMUAddress, MPU6050_PWR_MGMT_1, 0x00); // Clear sleep mode bit (6), enable all sensors
	delay(100); // Wait for all registers to reset

	// get stable time source
	writeByteI2C(i2c_bus, IMUAddress, MPU6050_PWR_MGMT_1, 0x03);  // Auto select clock source to be PLL gyroscope reference if ready else
	delay(200);

	// Configure Gyro and Thermometer
	// Disable FSYNC and set thermometer and gyro bandwidth to 44 and 42 Hz, respectively;
	// minimum delay time for this setting is 4.9 ms, which means sensor fusion update rates cannot
	// be higher than 1 / 0.0049 = ~200 Hz
	// DLPF_CFG = bits 2:0 = 011; this limits the sample rate to 1000 Hz for both 
	// With the MPU6050, it is possible to get gyro sample rates of 8 kHz, or 1 kHz
	writeByteI2C(i2c_bus, IMUAddress, MPU6050_MPU_CONFIG, 0x03);

	// Set sample rate = gyroscope output rate/(1 + SMPLRT_DIV)
	writeByteI2C(i2c_bus, IMUAddress, MPU6050_SMPLRT_DIV, 0x03);  // Use a 250 Hz rate; a rate consistent with the filter update rate
	// determined inset in CONFIG above

	// Set gyroscope full scale range
	// Range selects FS_SEL and GFS_SEL are 0 - 3, so 2-bit values are left-shifted into positions 4:3
	uint8_t c = readByteI2C(i2c_bus, IMUAddress, MPU6050_GYRO_CONFIG); // get current GYRO_CONFIG register value
	// c = c & ~0xE0; // Clear self-test bits [7:5]
	c = c & ~0x03; // Clear Fchoice bits [1:0]
	c = c & ~0x18; // Clear GFS bits [4:3]
	c = c | (uint8_t)3 << 3; // Set 2000dps full scale range for the gyro (11 on 4:3)
	// c =| 0x00; // Set Fchoice for the gyro to 11 by writing its inverse to bits 1:0 of GYRO_CONFIG
	writeByteI2C(i2c_bus, IMUAddress, MPU6050_GYRO_CONFIG, c); // Write new GYRO_CONFIG value to register

	// Set accelerometer full-scale range configuration
	c = readByteI2C(i2c_bus, IMUAddress, MPU6050_ACCEL_CONFIG); // get current ACCEL_CONFIG register value
	// c = c & ~0xE0; // Clear self-test bits [7:5]
	c = c & ~0x18;  // Clear AFS bits [4:3]
	c = c | (uint8_t)3 << 3; // Set 16g full scale range for the accelerometer (11 on 4:3)
	writeByteI2C(i2c_bus, IMUAddress, MPU6050_ACCEL_CONFIG, c); // Write new ACCEL_CONFIG register value

	// Set accelerometer sample rate configuration
	// It is possible to get a 4 kHz sample rate from the accelerometer by choosing 1 for
	writeByteI2C(i2c_bus, IMUAddress, MPU6050_MPU_CONFIG, 0x03); // Set accelerometer rate to 1 kHz and bandwidth to 44 Hz

	// The accelerometer, gyro, and thermometer are set to 1 kHz sample rates,
	// but all these rates are further reduced by a factor of 5 to 200 Hz because of the SMPLRT_DIV setting

	// Configure Interrupts and Bypass Enable
	// Set interrupt pin active high, push-pull, hold interrupt pin level HIGH until interrupt cleared,
	// clear on read of INT_STATUS, and enable I2C_BYPASS_EN so additional chips
	// can join the I2C bus and all can be controlled by the Arduino as master
	writeByteI2C(i2c_bus, IMUAddress, MPU6050_INT_PIN_CFG, 0x22);
	writeByteI2C(i2c_bus, IMUAddress, MPU6050_INT_ENABLE, 0x01);    // Enable data ready (bit 0) interrupt
	delay(100);
	return 0;
}

// Method to read data from IMU VIA I2C
void read_imu(TwoWire& i2c_bus, byte imu_addr){
  i2c_bus.beginTransmission(imu_addr);  // MPU6050 address
  i2c_bus.write(MPU6050_ACCEL_XOUT_H);  // Starting register
  i2c_bus.endTransmission(false);       // Retain connection

  i2c_bus.requestFrom(imu_addr, 12);    // Request 12 bytes of data, 2 bytes per info

  imu_data[0] = i2c_bus.read() << 8 | i2c_bus.read();
  imu_data[1] = i2c_bus.read() << 8 | i2c_bus.read();
  imu_data[2] = i2c_bus.read() << 8 | i2c_bus.read();

  imu_data[3] = i2c_bus.read() << 8 | i2c_bus.read();
  imu_data[4] = i2c_bus.read() << 8 | i2c_bus.read();
  imu_data[5] = i2c_bus.read() << 8 | i2c_bus.read();
}

// Method to Set gyro data limits and map it to byte
void cleanup_gyro_data(){
    // set limits to MIN MAX values
    if (imu_data[0] > 2100){
      imu_data[0] = 2100;
    }
    else if (imu_data[0] < -2000){
      imu_data[0] = -2000;
    }

    // map values to byte
    imu_data[0] = map(imu_data[0],-2000,2100,0,255);

    //imu_data[0] = imu_data[0]
    //Serial.println(imu_data[0]);
}

/*
=== FUNCTIONS & METHODS : LORAWAN
*/
/* Prepares the payload of the frame */
static void prepareTxFrame( uint8_t port , byte msg_type){
    if (msg_type == 1){
      // DEBUG
      Serial.println("msg_type ==  WAVE");
      
      appDataSize = imu_packet_len;
      appData[0] = 1 & 0xFF;

      // STORE IMU DATA DURING 16 SECONDS 
      for (int i=0; i<num_iter;i++){
        led_state = !led_state;
        digitalWrite(led_pin,led_state);
        
        read_imu(Wire,IMU1_ADDR);                     // read data of IMU 1 
        cleanup_gyro_data();                                  
        appData[1+(i*3)] = imu_data[0] & 0xFF;

        read_imu(Wire1,IMU2_ADDR);                    // read data of IMU 2
        cleanup_gyro_data(); 
        appData[2+(i*3)] = imu_data[0] & 0xFF;                                                                  

        read_imu(Wire1,IMU3_ADDR);                    // read data of IMU 3
        cleanup_gyro_data();                                
        appData[3+(i*3)] = imu_data[0] & 0xFF;

        //Serial.printf("%d/%d\t - %d\t%d\t%d\r\n",i,num_iter,imu_packet[1+(i*3)],imu_packet[2+(i*3)],imu_packet[3+(i*3)]);
        delay(200);
      }

    }
    else if (msg_type == 2){
      Serial.println("msg_type ==  WEATHER");
      appDataSize = weather_packet_len;

      appData[0] = 2 & 0xFF;
      appData[1] = map(int(bme.readTemperature()*float_to_int_factor), -10*float_to_int_factor,  40*float_to_int_factor,   0,255);     // map temp between -10 and 40  to byte
      appData[2] = map(int(bme.readHumidity()*float_to_int_factor),    0*float_to_int_factor,    100*float_to_int_factor,  0,255);     // map hum  between 0 and 100   to byte
      appData[3] = map(int(bme.readPressure()),                        86800,                    108300,                   0,255);     // map pres between 868hPa*100 and 1083hPa*100 to byte
      appData[4] = random(125,180);                                                                                                     // map temp between 0 and 25 to byte !! for demo, random value used to simulate average temp

      delay(16000);

    }
    else{
      Serial.println("DEBUGGING MESSAGE");
      // TEMP DEBUG
      appDataSize = 241;
      appData[0] = 0x01;  // define WAVE message
      for (int i=1; i < appDataSize; i++){
        appData[i] = i & 0xFF;
      }
      delay(1000);
    }
}

//downlink data handle function example
void downLinkDataHandle(McpsIndication_t *mcpsIndication)
{
  //Serial.printf("+REV DATA:%s,RXSIZE %d,PORT %d\r\n",mcpsIndication->RxSlot?"RXWIN2":"RXWIN1",mcpsIndication->BufferSize,mcpsIndication->Port);
  //Serial.print("+REV DATA:");
  int data_in;
  String temp = "";

  char msg_type = mcpsIndication->Buffer[0];

  for (int i = 1; i< 53;i++){
    if (char(mcpsIndication->Buffer[i]) == ';'){
        for (int j = 1; j < i; j++){
          temp += char(mcpsIndication->Buffer[j]);
        }
        data_in = temp.toInt();
        break;
    }
  }
  if (msg_type == '1'){                                       // Message is to turn on led
    Serial.printf("TURN ON LED FOR %d SECONDS\r\n",data_in);  
    digitalWrite(led_pin,HIGH);     
    delay(data_in*1000);              // incoming data is in seconds, so * 1000
    digitalWrite(led_pin, LOW);     
  }
  else if (msg_type == '2'){                                  // Message is to modify measuring interval
    appTxDutyCycle = data_in*1000*60; // incoming data is in minutes, so * 1000 * 60
    Serial.printf("MODIFY SLEEP TO %d mS\r\n",appTxDutyCycle);

  }else{
    Serial.println("INCOMING DATA WRONG FORMAT");
  }
  //Serial.printf("msg type:      %c\r\n",msg_type);
  //Serial.printf("temp data:     %s\r\n",temp);
  //Serial.printf("led time:      %d\r\n",data_in);

  

  
}

/*
===========================================================================================================================================
    VOID SETUP
===========================================================================================================================================
*/
void setup() {
  Serial.begin(115200);
  Mcu.begin(HELTEC_BOARD,SLOW_CLK_TPYE);

  Wire.begin(4,15);     // CONFIG I2C 0 (OLED) : SDA,SCL
  Wire1.begin(21,22);   // CONFIG I2C 1

  pinMode(LED_BUILTIN, OUTPUT); // config use of LED
  pinMode(led_pin, OUTPUT);
  
  // INIT IMU1
  int err1 = init_imu(Wire, IMU1_ADDR);
  if (err1 != 0) {
    Serial.print("Error initializing IMU1: ");
    Serial.println(err1);
    while (true) {
        digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on
        
        // SHOW ERROR LED
        digitalWrite(led_pin,HIGH);
        delay(1000);
        digitalWrite(led_pin, LOW);  // turn the LED on
        delay(1000);
        // # of iterations represents # of imu
        for (int i=0; i < 1; i++){
          digitalWrite(led_pin,HIGH);
          delay(100);
          digitalWrite(led_pin, LOW);  // turn the LED on
          delay(100);
        }
        
    }
  }
  else{Serial.println("Init IMU1 OK!");}

  // INIT IMU2
  int err2 = init_imu(Wire1, IMU2_ADDR);
  if (err2 != 0) {
    Serial.print("Error initializing IMU2: ");
    Serial.println(err2);
    while (true) {
      digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on
      
      // SHOW ERROR LED
      digitalWrite(led_pin,HIGH);
      delay(1000);
      digitalWrite(led_pin, LOW);  // turn the LED on
      delay(1000);
      // # of iterations represents # of imu
      for (int i=0; i < 2; i++){
        digitalWrite(led_pin,HIGH);
        delay(100);
        digitalWrite(led_pin, LOW);  // turn the LED on
        delay(100);
      }
    }
  }
  else{Serial.println("Init IMU2 OK!");}

  // INIT IMU3
  int err3 = init_imu(Wire1, IMU3_ADDR);
  if (err3 != 0) {
    Serial.print("Error initializing IMU3: ");
    Serial.println(err3);
    while (true) {
      digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on

      // SHOW ERROR LED
      digitalWrite(led_pin,HIGH);
      delay(1000);
      digitalWrite(led_pin, LOW);  // turn the LED on
      delay(1000);
      // # of iterations represents # of imu
      for (int i=0; i < 3; i++){
        digitalWrite(led_pin,HIGH);
        delay(100);
        digitalWrite(led_pin, LOW);  // turn the LED on
        delay(100);
      }
    }
  }
  else{Serial.println("Init IMU3 OK!");}

  // INIT BME280
  bme.begin(0x76);

}

/*
===========================================================================================================================================
    VOID LOOP
===========================================================================================================================================
*/
void loop()
{
  
  switch( deviceState )
  {
    case DEVICE_STATE_INIT:
    {
      //Serial.println("--> init");
      #if(LORAWAN_DEVEUI_AUTO)
            LoRaWAN.generateDeveuiByChipID();
      #endif
      LoRaWAN.init(loraWanClass,loraWanRegion);
      //both set join DR and DR when ADR off 
      LoRaWAN.setDefaultDR(3);
      break;
    }
    case DEVICE_STATE_JOIN:
    {
      //Serial.println("--> join");
      LoRaWAN.join();
      break;
    }
    case DEVICE_STATE_SEND:
    {
      //Serial.println("--> send");
      /*
      Serial.println((esp_timer_get_time()/1000));
      Serial.println(appTxDutyCycle+16000);
      Serial.println((esp_timer_get_time()/1000/(appTxDutyCycle+16000)));
      */
      
      if ((esp_timer_get_time()/1000/(appTxDutyCycle+16000))%4==0){     // After every 4 IMU packets, we send a weather packet
        prepareTxFrame(appPort, 2);   // Prepare a weather packet
      }
      else{
        prepareTxFrame(appPort, 1);
      }
      
      LoRaWAN.send();
      deviceState = DEVICE_STATE_CYCLE;

      /*
      Serial.println("=== PRINT DATA FROM appData ===");
      Serial.printf("first byte = %d\r\n",appData[0]);
      for (int i=0; i<num_iter;i++){
        Serial.printf("%02X\t%02X\t%02X\r\n",appData[1+(i*3)],appData[2+(i*3)],appData[3+(i*3)]);
      }
      */

      break;
    }
    case DEVICE_STATE_CYCLE:
    {
      //Serial.println("--> cycle");
      // Schedule next packet transmission
      txDutyCycleTime = appTxDutyCycle + randr( -APP_TX_DUTYCYCLE_RND, APP_TX_DUTYCYCLE_RND );
      LoRaWAN.cycle(txDutyCycleTime);
      deviceState = DEVICE_STATE_SLEEP;
      //Serial.println("--> sleep");

      break;
    }
    case DEVICE_STATE_SLEEP:
    {
      //Serial.println("--> sleep");
      LoRaWAN.sleep(loraWanClass);
      break;
    }
    default:
    {
      //Serial.println("--> default");
      deviceState = DEVICE_STATE_INIT;
      break;
    }
  }
}