/*
 * LTR-303ALS.h
 *
 *  Created on: Oct 5, 2020
 *      Author: Aniket Paluskar
 */

#ifndef APPLICATION_LTR_303ALS_LTR_303ALS_H_
#define APPLICATION_LTR_303ALS_LTR_303ALS_H_

//Invoking I2C Driver
#include <ti/drivers/I2C.h>

#define LTR303_SLAVE_ADDRESS 0x29
#define LTR303_WRITE  0x52
#define LTR303_READ  0x53
#define ALS_DATA_CH1_0  0x88
#define ALS_DATA_CH1_1  0x89
#define ALS_DATA_CH0_0  0x8A
#define ALS_DATA_CH0_1  0x8B
#define MANIFACTURE_ID  0x87
#define PART_ID  0x86
#define ALS_CONTROL  0x80
#define ALS_MEAS_RATE 0x85
#define ALS_STATUS 0x8C

#define MSGSIZE 3

#define ALS_GAIN 1  //Default Values for Testing , Gain is set as x1
#define ALS_INT 2   //Default Values for Testing , Integration time is 200ms

bool LTR303_init(I2C_Handle i2c);
float ALS_getLux();
uint8_t getPartID();
uint8_t getManufacID();

#endif /* APPLICATION_LTR_303ALS_LTR_303ALS_H_ */
