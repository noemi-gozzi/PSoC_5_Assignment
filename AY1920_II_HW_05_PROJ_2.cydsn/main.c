/**
* \brief Main source file for the I2C-Master project.
*
* In this project we set up a I2C master device with
* to understand the I2C protocol and communicate with a
* a I2C Slave device (LIS3DH Accelerometer).
*
* \author Gozzi Noemi
* \date April 2020
*/

// Include required header files
#include "I2C_Interface.h"
#include "project.h"
#include "stdio.h"
#include "InterruptRoutines.h"

/**
*   \brief 7-bit I2C address of the slave device.
*/
#define LIS3DH_DEVICE_ADDRESS 0x18

/**
*   \brief Address of the WHO AM I register
*/
#define LIS3DH_WHO_AM_I_REG_ADDR 0x0F

/**
*   \brief Address of the Status register
*/
#define LIS3DH_STATUS_REG 0x27

/**
*   \brief Address of the Control register 1
*/
#define LIS3DH_CTRL_REG1 0x20

/**
*   \brief Hex value to set normal mode to the accelerator
*/
#define LIS3DH_NORMAL_MODE_CTRL_REG1 0x47

/**
*   \brief  Address of the Temperature Sensor Configuration register
*/
#define LIS3DH_TEMP_CFG_REG 0x1F

#define LIS3DH_TEMP_CFG_REG_ACTIVE 0xC0

/**
*   \brief Address of the Control register 4
*/
#define LIS3DH_CTRL_REG4 0x23

#define LIS3DH_CTRL_REG4_BDU_ACTIVE 0x80

/**
*   \brief Address of the ADC output LSB register
*/
#define LIS3DH_OUT_ADC_3L 0x0C

/**
*   \brief Address of the ADC output MSB register
*/
#define LIS3DH_OUT_ADC_3H 0x0D

#define LIS3DH_OUT_X_L 0x28 //register for acceleration in X (LSB)

/*
    Hex value to set the accelerator in normal mode acceleration and 100 Hz,
    correspondent to 0101 0111, enabling XYZ 
    bit[4:7]= ODR: 100Hz: 0101
    bit[3]=Lpen High resolution/Normal Mode: 0
    bit[0:2]= XYZ enable
*/
#define LIS3DH_NORMAL_MODE_CTRL_REG1_XYZ 0x57

/*
    ctrl register 4 in normal mode 0x00
    bit[4:5]= FS[0:1]= 00 for FSR +-2g
    bit [3]= HR: Normal mode 0
*/
#define LIS3DH_CTRL_REG4_NORMAL_MODE 0x00

/* 
    packet dimension: 8 bytes (3 data 2-bytes each, 1 byte header, 1 byte tail
*/

#define packet_dimension 8

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    I2C_Peripheral_Start();
    UART_Debug_Start();
    
    CyDelay(5); //"The boot procedure is complete about 5 milliseconds after device power-up."
    
    // String to print out messages on the UART
    char message[50];

    // Check which devices are present on the I2C bus
    for (int i = 0 ; i < 128; i++)
    {
        if (I2C_Peripheral_IsDeviceConnected(i))
        {
            // print out the address is hex format
            sprintf(message, "Device 0x%02X is connected\r\n", i);
            UART_Debug_PutString(message); 
        }
        
    }
    
    /******************************************/
    /*            I2C Reading                 */
    /******************************************/
    
    /* Read WHO AM I REGISTER register */
    uint8_t who_am_i_reg;
    ErrorCode error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                                  LIS3DH_WHO_AM_I_REG_ADDR, 
                                                  &who_am_i_reg);
    if (error == NO_ERROR)
    {
        sprintf(message, "WHO AM I REG: 0x%02X [Expected: 0x33]\r\n", who_am_i_reg);
        UART_Debug_PutString(message); 
    }
    else
    {
        UART_Debug_PutString("Error occurred during I2C comm\r\n");   
    }
    
    /*      I2C Reading Status Register       */
    
    uint8_t status_register; 
    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_STATUS_REG,
                                        &status_register);
    
    if (error == NO_ERROR)
    {
        sprintf(message, "STATUS REGISTER: 0x%02X\r\n", status_register);
        UART_Debug_PutString(message); 
    }
    else
    {
        UART_Debug_PutString("Error occurred during I2C comm to read status register\r\n");   
    }
    
    /******************************************/
    /*        Read Control Register 1         */
    /******************************************/
    uint8_t ctrl_reg1; 
    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_CTRL_REG1,
                                        &ctrl_reg1);
    
    if (error == NO_ERROR)
    {
        sprintf(message, "CONTROL REGISTER 1: 0x%02X\r\n", ctrl_reg1);
        UART_Debug_PutString(message); 
    }
    else
    {
        UART_Debug_PutString("Error occurred during I2C comm to read control register 1\r\n");   
    }
    
    /******************************************/
    /*    I2C Writing  normal mode, 100Hz     */
    /******************************************/
    
        
    
    
    if (ctrl_reg1 != LIS3DH_NORMAL_MODE_CTRL_REG1_XYZ)
    {

        ctrl_reg1 = LIS3DH_NORMAL_MODE_CTRL_REG1_XYZ;
    
        error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                             LIS3DH_CTRL_REG1,
                                             ctrl_reg1);
    
        if (error == NO_ERROR)
        {
            sprintf(message, "CONTROL REGISTER 1 successfully written as: 0x%02X\r\n", ctrl_reg1);
            UART_Debug_PutString(message); 
        }
        else
        {
            UART_Debug_PutString("Error occurred during I2C comm to set control register 1\r\n");   
        }
        /******************************************/
        /*     Read Control Register 1 again      */
        /******************************************/

        error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                            LIS3DH_CTRL_REG1,
                                            &ctrl_reg1);
        
        if (error == NO_ERROR)
        {
            sprintf(message, "CONTROL REGISTER 1 after being updated: 0x%02X\r\n", ctrl_reg1);
            UART_Debug_PutString(message); 
        }
        else
        {
            UART_Debug_PutString("Error occurred during I2C comm to read control register 1\r\n");   
        }
    }
    
    
    
    
    
    /******************************************/
    /*        Read Control Register 4         */
    /******************************************/
    uint8_t ctrl_reg4;

    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_CTRL_REG4,
                                        &ctrl_reg4);
    
    if (error == NO_ERROR)
    {
        sprintf(message, "CONTROL REGISTER 4: 0x%02X\r\n", ctrl_reg4);
        UART_Debug_PutString(message); 
    }
    else
    {
        UART_Debug_PutString("Error occurred during I2C comm to read control register4\r\n");   
    }
    
    /******************************************/
    /*       I2C Writing   REGISTER 4         */
    /******************************************/
    
    
    if (ctrl_reg4 != LIS3DH_CTRL_REG4_NORMAL_MODE)
    {

        ctrl_reg4 = LIS3DH_CTRL_REG4_NORMAL_MODE;
    
        error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                             LIS3DH_CTRL_REG4,
                                             ctrl_reg4);
    
        if (error == NO_ERROR)
        {
            sprintf(message, "CONTROL REGISTER 4 successfully written as: 0x%02X\r\n", ctrl_reg4);
            UART_Debug_PutString(message); 
        }
        else
        {
            UART_Debug_PutString("Error occurred during I2C comm to set control register 1\r\n");   
        }
        error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_CTRL_REG4,
                                        &ctrl_reg4);
    
        //if updated print the new value
        if (error == NO_ERROR)
        {
            sprintf(message, "CONTROL REGISTER 4 after being updated: 0x%02X\r\n", ctrl_reg4);
            UART_Debug_PutString(message); 
        }
        else
        {
            UART_Debug_PutString("Error occurred during I2C comm to read control register4\r\n");   
        }
        
    }
    //timer initialization to be sure that ISR is not interrupting register setting 
    
    Timer_Start();
    isr_timer_StartEx(Custom_ISR_Timer_XYZ);
    /*
    variable settings
    */
    
    int16_t OutX;
    int16_t OutY;
    int16_t OutZ;
    uint8_t header = 0xA0;
    uint8_t footer = 0xC0;
    uint8_t OutArray[packet_dimension]; 
    
    OutArray[0] = header;
    OutArray[packet_dimension-1] = footer;
    PacketReadyFlag=0;
    
    for(;;)
    {
                
        if (PacketReadyFlag){
                /*
                in section 3.2.1 of the datasheet is specified that in normal mode mg/digit sensitivity is 4 
                that is an approximation that deals with FSR=+-2g, nbit=10bit; 
                fsr/2=512=2000mg
                acc[mg]=acc[digit]*3.9026 --> in Int is around 4
                */
            
                PacketReadyFlag=0; //flag is cleared
                OutX = ((int16)((Acceleration[0]) | (Acceleration[1])<<8)>>6)*4;
                OutY = ((int16)((Acceleration[2]) | (Acceleration[3])<<8)>>6)*4;
                OutZ = ((int16)((Acceleration[4]) | (Acceleration[5])<<8)>>6)*4;
                
                /*
                Acceleration values are organized in 8 bits data to be sent through UART communication
                */
                
                OutArray[1] = (uint8_t)(OutX & 0xFF);
                OutArray[2] = (uint8_t)(OutX >> 8);
                OutArray[3] = (uint8_t)(OutY & 0xFF);
                OutArray[4] = (uint8_t)(OutY >> 8);
                OutArray[5] = (uint8_t)(OutZ & 0xFF);
                OutArray[6] = (uint8_t)(OutZ >> 8);
                
                UART_Debug_PutArray(OutArray, packet_dimension);
                }
    }
}

/* [] END OF FILE */

