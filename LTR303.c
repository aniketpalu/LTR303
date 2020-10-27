/*
 * LTR-303ALS.c
 *
 *  Created on: Oct 5, 2020
 *      Author: Aniket Paluskar
 */




#include "LTR-303ALS.H"
#include "stdio.h"

//***********************Variable Declarations***********************//


I2C_Transaction I2Ctransaction;
I2C_Handle i2cHandle;
uint8_t writeBuffer[MSGSIZE];
uint8_t readBuffer;

//***********************Static Functions Declaration***********************//


static bool Set_ALS_Integration_Time(I2C_Handle i2cHandle);
static bool Enable_ALS(I2C_Handle i2cHandle);
static uint16_t ALS_CH0_Read();
static uint16_t ALS_CH0_Read();
static uint8_t Read(I2C_Handle i2c, uint8_t RegisterAddress);
static float Calculate_Lux(uint16_t Channel0Data, uint16_t Channel1Data)







/***************************************************************************
 * @fn              LTR303_init()
 *
 * @brief           Starts the sensor and sets basic values
 *
 * @params          I2C_Handle -> I2C Handle
 *
 * @returns         bool
 *
 */
bool LTR303_init(I2C_Handle i2c)
{
    i2cHandle = i2c;
    I2Ctransaction.slaveAddress = LTR303_SLAVE_ADDRESS;
    I2Ctransaction.readBuf = (void *) readBuffer;
    I2Ctransaction.writeBuf = (void *) writeBuffer;


    //Enabling ALS
    if(Enable_ALS(i2cHandle))
    {
        printf("ALS Activated");
    }
    else
    {
        printf("ALS Activation Failed");
    }


    //Setting Integration Time
    if(Set_ALS_Integration_Time(i2cHandle))
    {
        printf("Integration Time Set");
    }
    else
    {
        printf("Integration Time Set Failed");
    }

    return true;
}

/***************************************************************************
 * @fn          ALS_getLux()
 *
 * @brief       Gives Lux value
 *
 * @param       None
 *
 * @return      float
 *
 */
float ALS_getLux()
{
    float Final_Lux;

    Final_Lux = Calculate_Lux(ALS_CH0_Read() , ALS_CH1_Read());

    return Final_Lux;
}

/***************************************************************************
 * @fn          getPartID()
 *
 * @brief       Reads the Part Id of Sensor
 *
 * @param       None
 *
 * @return      uint8_t
 *
 */

uint8_t getPartID()
{
    uint8_t PartID = Read(i2cHandle, PART_ID);

    return PartID;
}

/***************************************************************************
 * @fn          getManufacID()
 *
 * @brief       Reads the Manufacture Id of Sensor
 *
 * @param       None
 *
 * @return      uint8_t
 *
 */

uint8_t getManufacID()
{
    uint8_t ManufacID = Read(i2cHandle, MANIFACTURE_ID);

    return ManufacID;
}



//***********************Static Functions***********************//


/***************************************************************************
 * @fn          Set_ALS_Integration_Time()
 *
 * @brief       Sets up integration time
 *
 * @param       I2C_Handle -> I2C Handle
 *
 * @return      bool
 */

bool Set_ALS_Integration_Time(I2C_Handle i2cHandle)
{
    writeBuffer[0] = LTR303_WRITE;  //Write Command
    writeBuffer[1] = ALS_MEAS_RATE;
    writeBuffer[2] = 0x12;          // Int Time = 200ms, Meas rate = 200ms
    I2Ctransaction.writeCount = 1;

    bool result = I2C_transfer(i2cHandle, &I2Ctransaction);
    return result;
}


/***************************************************************************
 * @fn          Enable_ALS()
 *
 * @brief       Turns one Sensor into Active Mode
 *
 * @param       I2C_Handle -> I2C Handle
 *
 * @return      bool
 */

bool Enable_ALS(I2C_Handle i2cHandle)
{
    writeBuffer[0] = LTR303_WRITE;  // Write Command
    writeBuffer[1] = ALS_CONTROL;
    writeBuffer[2] = 0x01;          // Gain = 1X, SW reset = 0, ALS Mode = 1(Active)
    I2Ctransaction.writeCount = 1;

    bool result = I2C_transfer(i2cHandle, &I2Ctransaction);

    return result;
}

/***************************************************************************
 * @fn          ALS_CH1_Read()
 *
 * @brief       Reads Channel 1 of Sensor
 *
 * @param       None
 *
 * @return      uint16_t
 */

uint16_t ALS_CH1_Read()
{
    uint16_t Channel1Data;
    Channel1Data = Read(i2cHandle, ALS_DATA_CH1_1) << 8;
    Channel1Data | = Read(i2cHandle, ALS_DATA_CH1_0);
    return Channel1Data;
}

/***************************************************************************
 * @fn          ALS_CH0_Read()
 *
 * @brief       Reads Channel 0 of Sensor
 *
 * @param       None
 *
 * @return      uint16_t
 */

uint16_t ALS_CH0_Read()
{
    uint16_t Channel0Data;
    Channel0Data = Read(i2cHandle, ALS_DATA_CH0_1) << 8;
    Channel0Data | = Read(i2cHandle, ALS_DATA_CH0_0) << 8;
    return Channel0Data;
}

/***************************************************************************
 * @fn          Read()
 *
 * @brief       Reads the data from given address,  Reads only 1 byte
 *
 * @param       I2C_Handle -> I2C Handle
 *              RegisterAddress -> Address of Register to be read
 *
 * @return      uint8_t
 */

uint8_t Read(I2C_Handle i2c, uint8_t RegisterAddress)
{
    uint8_t result;
    writeBuffer[0] = LTR303_READ;
    writeBuffer[1] = RegisterAddress;
    I2Ctransaction.readCount = 1;

    if(I2C_transfer(i2c, &I2Ctransaction))
    {
        result = readBuffer;
        return result;
    }
    else
    {
        printf("I2C Transaction Failed");
        return 0;
    }
}
/***************************************************************************
 * @fn          Calculate_Lux()
 *
 * @brief       Calculates Real world Lux value
 *
 * @param       Channel0Data -> Data from Channel 0
 *              Channel1Data -> Data from Channel 1
 *
 * @return      float
 */
float Calculate_Lux(uint16_t Channel0Data, uint16_t Channel1Data)
{
    float ratio, ALS_LUX;

    ratio = Channel1Data/(Channel0Data + Channel1Data);

    if (ratio < 0.45)
    {
        ALS_LUX = ( (1.7743 * Channel0Data) + (1.1059 * Channel1Data) ) / ALS_GAIN / ALS_INT;
    }
    else if (ratio < 0.64 && ratio >= 0.45)
    {
        ALS_LUX = ( (4.2785 * Channel0Data) - (1.9548 * Channel1Data) ) / ALS_GAIN / ALS_INT;
    }
    else if (ratio < 0.85 && ratio >= 0.64)
    {
        ALS_LUX = ( (0.5926 * Channel0Data) + (0.1185 * Channel1Data) ) / ALS_GAIN / ALS_INT;
    }
    else
    {
        ALS_LUX = 0;
    }

    return ALS_LUX;
}
