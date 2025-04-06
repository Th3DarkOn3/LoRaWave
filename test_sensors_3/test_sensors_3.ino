#include <Wire.h>
#include <Adafruit_BME280.h>

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
const int num_imu_data  = 6;          // Gyro x3 et accel x3
int16_t imu_data[num_imu_data];       // store data read from 1 IMU

byte imu_packet[241];                 // Store all data for LoRa transmission         
byte num_iter = 80;                   // # iterations that will be stored in imu_packet

byte weather_packet[5];               // Store temperature, humidity and pressure

// Init Temp/Hum sensor
Adafruit_BME280 bme; // I2C


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

void setup() {
  Wire.begin(4,15);     // CONFIG I2C 0 (OLED) : SDA,SCL
  Wire1.begin(21,22);   // CONFIG I2C 1
  Serial.begin(115200);
  
  pinMode(LED_BUILTIN, OUTPUT); // config use of LED
  
  imu_packet[0]     = 1;                  // set first bit to 1 to show this is a wave message
  weather_packet[0] = 2;                  // set first bit to 1 to show this is a weather message
  
  // INIT IMU1
  int err1 = init_imu(Wire, IMU1_ADDR);
  if (err1 != 0) {
    Serial.print("Error initializing IMU1: ");
    Serial.println(err1);
    while (true) {
        digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on
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
    }
  }
  else{Serial.println("Init IMU3 OK!");}

  // INIT BME280
  bme.begin(0x76);
}


void loop() {
  bool isWaveMsg = false;
  
  if (isWaveMsg){
    for (int i=0; i<num_iter;i++){
      read_imu(Wire,IMU1_ADDR);                     // read data of IMU 1 
      cleanup_gyro_data();                                  
      imu_packet[1+(i*3)] = imu_data[0];

      read_imu(Wire1,IMU2_ADDR);                    // read data of IMU 2
      cleanup_gyro_data(); 
      imu_packet[2+(i*3)] = imu_data[0];                                 

      read_imu(Wire1,IMU3_ADDR);                    // read data of IMU 3
      cleanup_gyro_data();                                
      imu_packet[3+(i*3)] = imu_data[0];

      Serial.printf("%d/%d\t - %d\t%d\t%d\r\n",i,num_iter,imu_packet[1+(i*3)],imu_packet[2+(i*3)],imu_packet[3+(i*3)]);
      delay(200);
    }
  
    Serial.printf("DONE -> Message type: %d",imu_packet[0]);
    for (int i=0; i<num_iter;i++){
      Serial.printf("%d\t%d\t%d\r\n",imu_packet[1+(i*3)],imu_packet[2+(i*3)],imu_packet[3+(i*3)]);
    }
  }
  else{

    int float_to_int_factor = 10;                                                                                                           // to keep a precision of .1, we multiply the float by 10 to keep the .x value in the int.
    weather_packet[1] = map(int(bme.readTemperature()*float_to_int_factor), -10*float_to_int_factor,  40*float_to_int_factor,   0,255);     // map temp between -10 and 40  to byte
    weather_packet[2] = map(int(bme.readHumidity()*float_to_int_factor),    0*float_to_int_factor,    100*float_to_int_factor,  0,255);     // map hum  between 0 and 100   to byte
    weather_packet[3] = map(int(bme.readPressure()),                        86800,                    108300,                   0,255);     // map pres between 868hPa*100 and 1083hPa*100 to byte
    weather_packet[4] = random(125,180);
    
    Serial.printf("--> float  values: %.1f\t%.1f\t%.0f\t%d\r\n",bme.readTemperature(),bme.readHumidity(),bme.readPressure(),000);
    Serial.printf("--> int    values: %d\t%d\t%d\t%d\r\n",int(bme.readTemperature()*10),int(bme.readHumidity()*10),int(bme.readPressure()),000);
    Serial.printf("--> mapped values: %d\t%d\t%d\t%d\r\n",weather_packet[1],weather_packet[2],weather_packet[3],weather_packet[4]);

  }

  
  while(true){
    delay(10000);
  } 
}