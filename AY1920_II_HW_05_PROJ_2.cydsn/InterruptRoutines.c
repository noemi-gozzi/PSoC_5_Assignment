/* ========================================
 *
ISR FILE: it has been chosen to use an ISR on timer to deal with acceleration data at a correct frequency.
Other methods (such us exploiting FIFO functionality and buffer) are less robust when dealing with real
time application. Checking, with an ISR if new data are available (status register bit XYZDA=1), with a 
frequency higher than the one of the system (i.e. f LIS3DH 100 HZ, ISR 400 Hz) ensures that data are not 
missed; then inside ISR status register is checked, and the flag is high only when new data are available
(not each ISR).
-   This has been confirmed also in Bridge control Panel where the rate at which data are recevied and 
    plotted is around 98-102 data per seconds. (as per requirements)
-   Furthermore it has been checked using cool-term and an error message that the frequency is enough and data
    never overrun [status register bit[7]= XYZOR]; using this frequency of ISR and and checking inside the ISR if 
    status_register&0x80 was true (also with 0xF0 to check X,Y,Z separately), and using an Overrun flag to communicate 
    with the main, no error message was printed in coolterm, confirming that data were not missed.
    
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
#define packet_dimension 6
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
        
            error=I2C_Peripheral_ReadRegisterMulti(LIS3DH_DEVICE_ADDRESS, LIS3DH_OUT_X_L, packet_dimension , (&Acceleration[0]));
            //if data are correctly read, then communicate to main that data are ready to be sent.
            if(error == NO_ERROR){
            PacketReadyFlag=1;
            }
    }
}


/* [] END OF FILE */
