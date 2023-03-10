#include "config.h"
#include "HAL.h"
#include "sensor.h"

__attribute__((aligned(4))) uint32_t MEM_BUF[BLE_MEMHEAP_SIZE / 4];

__HIGH_CODE
__attribute__((noinline))
void Main_Circulation(){
    while(1){
        TMOS_SystemProcess();
    }
}

int main(void){
#if(defined(DCDC_ENABLE)) && (DCDC_ENABLE == TRUE)
    PWR_DCDCCfg(ENABLE);
#endif
    SetSysClock(CLK_SOURCE_PLL_60MHz);
    // SetSysClock(CLK_SOURCE_HSE_16MHz);
#if(defined(HAL_SLEEP)) && (HAL_SLEEP == TRUE)
    GPIOA_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_PU);
    GPIOB_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_PU);
#endif
    // LowPower_Shutdown(0);//Test shutdown current
    CH58X_BLEInit();
    HAL_Init();
    GAPRole_BroadcasterInit();
    Sensor_Init();
    Main_Circulation();
}