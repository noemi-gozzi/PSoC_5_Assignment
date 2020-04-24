/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "InterruptRoutines.h"
#define LIS3DH_DEVICE_ADDRESS 0x18
#define LIS3DH_OUT_X_L 0x28
#define LIS3DH_STATUS_REG 0x27
#define STATUS_REGISTER_NEW_DATA 0x08
uint8_t status_register_newdata;

ErrorCode error;

CY_ISR(Custom_ISR_Timer_XYZ){
    
     Timer_ReadStatusRegister();
     error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_STATUS_REG,
                                        &status_register_newdata);
        
        if ((error==NO_ERROR)&&(status_register_newdata & STATUS_REGISTER_NEW_DATA)){
        
            error=I2C_Peripheral_ReadRegisterMulti(LIS3DH_DEVICE_ADDRESS, LIS3DH_OUT_X_L, 6 , (&Acceleration[0]));
    //                           
            if(error == NO_ERROR){
                PacketReadyFlag=1;
            }
}
}


/* [] END OF FILE */
