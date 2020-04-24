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
#ifndef _INTERRUPT_ROUTINES_H
    #define _INTERRUPT_ROUTINES_H
    #include "project.h"
    #include "I2C_Interface.h"
    CY_ISR_PROTO(Custom_ISR_Timer_XYZ);
    
    uint8_t Acceleration[6];
    volatile uint8_t PacketReadyFlag;
    
    
    
   
   
#endif
/* [] END OF FILE */
