/* ========================================
 *
ISR FILE: it has been chosen to use an ISR on timer to deal with acceleration data at a correct frequency.
Checking, with an ISR if new data are available (status register bit XYZDA=1, 0x08), with a 
frequency higher than the one of the system (i.e. f LIS3DH 100 HZ, ISR 200 Hz) ensures that data are not 
missed (flag will be high not every ISR but only when status register confirm new data). In the previous 
project it has been tested 400 HZ but with the same test as before, it has been confirmed to achieve 
good result and correct frequency even with 200 Hz.
-   This has been confirmed also in Bridge control Panel where the rate at which data are recevied and 
    plotted is around 98-102 data per seconds. (as per requirements)
-   Furthermore it has been checked using cool-term and an error message that the frequency is enough and data
    never overrun [status register bit[7]= XYZOR, 0x80 and 0xF0]; using this frequency of ISR and and checking if 
    status_register&0x80 is true, no error message was printed in coolterm, confirming that data are not missed.
It has been decided to read directly XYZ data in the ISR to be sure that the data are the ones corresponent 
to the exat time when XYZDA flag has been written to 1 (time does not intercurr between staus register reading 
and data reading);
 *
 * ========================================
*/

#include "InterruptRoutines.h"
#define LIS3DH_DEVICE_ADDRESS 0x18
#define LIS3DH_OUT_X_L 0x28
#define LIS3DH_STATUS_REG 0x27
#define STATUS_REGISTER_NEW_DATA 0x08 //status register bit[3]=1 when new data is available
uint8_t status_register_newdata;

ErrorCode error;

CY_ISR(Custom_ISR_Timer_XYZ){
    
     Timer_ReadStatusRegister();
     error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_STATUS_REG,
                                        &status_register_newdata);
        
     if ((error==NO_ERROR)&&((status_register_newdata) & (STATUS_REGISTER_NEW_DATA))){
        
            error=I2C_Peripheral_ReadRegisterMulti(LIS3DH_DEVICE_ADDRESS, LIS3DH_OUT_X_L, 6 , (&Acceleration[0]));
    //                           
            if(error == NO_ERROR){
                PacketReadyFlag=1;
            }
}
}


/* [] END OF FILE */
