/**
* \brief Main source file for the I2C-Master project.
*
* In this project we set up a I2C master device with
* to understand the I2C protocol and communicate with a
* a I2C Slave device (LIS3DH Accelerometer).
*
* \author Gabriele Belotti
* \date , 2020
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

/**
*   Hex value to set the accelerator in high resolution mode acceleration and 100 Hz,
    correspondent to 0101 1000, enabling XYZ 
*/
#define LIS3DH_HIGH_RES_CTRL_REG1_XYZ_100 0x57

/*
ctrl register 4 inhigh resolutio HR=1, +- 4g (bit FS[1:0] set to 01 --> FSR +-4g
*/
#define LIS3DH_CTRL_REG4_HIGH_RES 0x18

#define packet_dimension 14

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    I2C_Peripheral_Start();
    UART_Debug_Start();
    Timer_Start();
    isr_timer_StartEx(Custom_ISR_Timer_XYZ);
    
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
    /*            I2C Writing                 */
    /******************************************/
    
        
    
    
    if (ctrl_reg1 != LIS3DH_HIGH_RES_CTRL_REG1_XYZ_100)
    {

        ctrl_reg1 = LIS3DH_HIGH_RES_CTRL_REG1_XYZ_100;
    
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
    /*     Read Control Register 4     */
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
    /*            I2C Writing   REGISTER 4              */
    /******************************************/
    
    
    if (ctrl_reg4 != LIS3DH_CTRL_REG4_HIGH_RES)
    {

        ctrl_reg4 = LIS3DH_CTRL_REG4_HIGH_RES;
    
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
    
    int16_t OutX;
    int16_t OutY;
    int16_t OutZ;
    float32 OutX_f;
    float32 OutY_f;
    float32 OutZ_f;
    int32 OutX_32;
    int32 OutY_32;
    int32 OutZ_32;
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
                in section 3.2.1 of the datasheet is specified that in high resolution mode mg/digit is 1 
                if FSR=+-2g, nbit=12bit; Since the FSR is 4g the sensitivy to change from digit to MG is aroud 2.
                each value is rescaled in mg
                */
                PacketReadyFlag=0;
                OutX = ((int16)((Acceleration[0]) | (Acceleration[1])<<8)>>4)*2;
                OutY = ((int16)((Acceleration[2]) | (Acceleration[3])<<8)>>4)*2;
                OutZ = ((int16)((Acceleration[4]) | (Acceleration[5])<<8)>>4)*2;
                /*
                acc[m/s]=acc[mg]*9.81/1000; then I want to keep info from 3 decimals so
                acc=acc[m/s]*1000;
                so the overall calculation would be acc[m/s]=acc[mg]*9.81;
                */
                
                OutX_f=OutX*9.81; 
                OutY_f=OutY*9.81; 
                OutZ_f=OutZ*9.81; 
                
                OutX_32= (int32)(OutX_f);
                OutY_32= (int32)(OutY_f);
                OutZ_32= (int32)(OutZ_f);
                
                OutArray[1] = (uint8_t)(OutX_32 & 0xFF);
                OutArray[2] = (uint8_t)((OutX_32 >> 8)&0xFF);
                OutArray[3] = (uint8_t)((OutX_32 >> 16)&0xFF);
                OutArray[4] = (uint8_t)(OutX_32 >> 24);
                
                OutArray[5] = (uint8_t)(OutY_32 & 0xFF);
                OutArray[6] = (uint8_t)((OutY_32 >> 8)&0xFF);
                OutArray[7] = (uint8_t)((OutY_32 >> 16)&0xFF);
                OutArray[8] = (uint8_t)(OutY_32 >> 24);
                
                OutArray[9] = (uint8_t)(OutZ_32 & 0xFF);
                OutArray[10] = (uint8_t)((OutZ_32 >> 8)&0xFF);
                OutArray[11] = (uint8_t)((OutZ_32 >> 16)&0xFF);
                OutArray[12] = (uint8_t)(OutZ_32 >> 24);
                
                UART_Debug_PutArray(OutArray, packet_dimension);
                }
    }
}

/* [] END OF FILE */
