/* ========================================
 *
ISR FILE: it has been chosen to use an ISR on timer to deal with acceleration data at a correct frequency.
Other methods (such us exploiting FIFO functionality and buffer) are less robust when dealing with real
time application. Checking, with an ISR if new data are available (status register bit XYZDA=1), with a 
frequency higher than the one of the system (i.e. f LIS3DH 100 HZ, ISR 400 Hz) ensures that data are not 
missed. This has been confirmed also in Bridge control Panel where the rate at which data are recevied and 
plotted is around 98-102 data per seconds.
It has been decided to read also XYZ data in the ISR to be sure that the data are the ones corresponent to
the exat time when XYZDA flag has been written to 1;
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
    //read status register value
    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_STATUS_REG,
                                        &status_register_newdata);
    //when new data is available the read XYZ values with ReadRegisterMulti function.   
    if ((error==NO_ERROR)&&((status_register_newdata) & (STATUS_REGISTER_NEW_DATA))){
        
            error=I2C_Peripheral_ReadRegisterMulti(LIS3DH_DEVICE_ADDRESS, LIS3DH_OUT_X_L, 6 , (&Acceleration[0]));
            //if data are correctly read, then communicate to main that data are ready to be sent.
            if(error == NO_ERROR){
            PacketReadyFlag=1;
            }
    }
}


/* [] END OF FILE */
