#include "config.h"
#include "sensor.h"
#include "bthome.h"
#include <math.h>

// The advertising interval in a burst (units of 625us, 0x00A0 - 0x3FFF)
#define DEFAULT_ADVERTISING_INTERVAL    0x3FFF
// THe length of an advertising burst. 
#define DEFAULT_ADVERTISING_LENGTH      160

// Update interval for reading sensor data. When lower than the advertisment 
// interval the advertisement interval is also set by this.
#define DEFAULT_UPDATE_PERIOD           (10*1600) // 10 seconds

// Task ID for internal task/event processing
static uint8_t Sensor_TaskID; 
// Callbacks are not needed
static gapRolesBroadcasterCBs_t Sensor_SensorCBs = {NULL, NULL};

// Encryption key for BTHome data
uint8_t key[] = {0x23, 0x1d, 0x39, 0xc1, 0xd7, 0xcc, 0x1a, 0xb1, 0xae, 0xe2, 0x24, 0xcd, 0x09, 0x6d, 0xb9, 0x32};
// Pointer to the BTHome instance, will be initialised later
static BTHome *bthome;

// Callback function to gather temperature data. Temperature is written to [data]
void getTemperature0(uint8_t *data){
    // Save the current ADC config
    uint8_t adc_config = R8_ADC_CFG;
    // Initialise the ADC to measure VCC
    ADC_InterBATSampInit();
    // This holds an integer value corresponding to VCC, see datasheet for info.
    uint16_t adc_vcc_raw = ADC_ExcutSingleConver();

    // Power up the thermistor (Pin B15)
    GPIOB_ModeCfg(GPIO_Pin_15, GPIO_ModeOut_PP_5mA);
    // GPIOA_ResetBits(GPIO_Pin_12);
    // Initialise AIN0 (Pin A4) 
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeIN_Floating);
    ADC_ExtSingleChSampInit(SampleFreq_3_2, ADC_PGA_0);
    R8_ADC_CHANNEL = 13;
    // This holds an integer value corresponding to the temperature.
    uint16_t adc_temp_raw = ADC_ExcutSingleConver();
    // Reset/disable the ADC
    R8_ADC_CFG = adc_config;
    // Reset the GPIO
    GPIOA_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_PU);
    GPIOB_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_PU);

    // Temperature conversion. Depends on part values.
    float adc_ratio = (float)(4 * adc_vcc_raw - 6144) / (float)(adc_temp_raw);
    // float Ti = 0.00335401643468052993 + 0.00025316455696202532*log(0.94 * (adc_ratio - 1)); // 1/T = 1/T0 + 1/B * ln( R0 * ( ( adcVCC / adcVal ) - 1 ) / RT0 )
    float Ti = 0.00335401643468052993 + 0.00025316455696202532*logf(0.2128 * (adc_ratio - 1)); // 1/T = 1/T0 + 1/B * ln( R0 * ( ( adcVCC / adcVal ) - 1 ) / RT0 )
    //         1/298.15K                1/3950B                    10k/47k
    uint16_t temp = -27315. + 100. / Ti;

    // Write 2 bytes of data to the output
    data[0] = (temp & 0xFF);
    data[1] = (temp >> 8);
}

// Dummy function
// void get0(uint8_t *data){
//     data[0] = 0x00;
//     data[1] = 0x00;
// }

void Sensor_Init(){
    //Initialize BTHome with key or without, to enable or disable encryption respectively
    bthome = new BTHome(key);
    // bthome = new BTHome();

    // Each measurement is added, with objectid and callback function.
    bthome->addMeasurement(BTHOME_OBJECTID_TEMPERATURE_PRECISE, &getTemperature0);
    // bthome->addMeasurement(BTHOME_OBJECTID_HUMIDITY_PRECISE, &get0); // Minimum payload length bug (fixed in BTHome 2.5.1)

    // This is the ID that is used for periodic events
    Sensor_TaskID = TMOS_ProcessEventRegister(Sensor_ProcessEvent);  

    // GAP Advertiesment settings
    uint8_t initial_advertising_enable = TRUE;
    uint8_t initial_adv_event_type = GAP_ADTYPE_ADV_NONCONN_IND; //GAP_ADTYPE_EXT_NONCONN_NONSCAN_UNDIRECT
    GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &initial_advertising_enable);
    GAPRole_SetParameter(GAPROLE_ADV_EVENT_TYPE, sizeof(uint8_t), &initial_adv_event_type);

    // Initialize the advertised data
    bthome->writeAdvData();
    GAPRole_SetParameter(GAPROLE_ADVERT_DATA, bthome->advDataLength, &bthome->advData);

    // The advertisement interval. See the top of this file.
    uint16_t advInt = DEFAULT_ADVERTISING_INTERVAL;
    GAP_SetParamValue(TGAP_DISC_ADV_INT_MIN, advInt);
    GAP_SetParamValue(TGAP_DISC_ADV_INT_MAX, advInt);
    
    // Start broadcasting
    GAPRole_BroadcasterStartDevice(&Sensor_SensorCBs);
    tmos_start_task(Sensor_TaskID, SBP_PERIODIC_EVT, DEFAULT_UPDATE_PERIOD);
    tmos_start_task(Sensor_TaskID, BROADCAST_DISABLE_EVT, DEFAULT_ADVERTISING_LENGTH);
}

uint16_t Sensor_ProcessEvent(uint8_t task_id, uint16_t events){
    if(events & SYS_EVENT_MSG){
        uint8_t *pMsg;

        if((pMsg = tmos_msg_receive(Sensor_TaskID)) != NULL){
            // Release the TMOS message
            tmos_msg_deallocate(pMsg);
        }

        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }

    if(events & SBP_PERIODIC_EVT){
        bthome->writeAdvData();
        GAPRole_SetParameter(GAPROLE_ADVERT_DATA, bthome->advDataLength, &bthome->advData);

        uint16_t advInt = DEFAULT_ADVERTISING_INTERVAL;
        GAP_SetParamValue(TGAP_DISC_ADV_INT_MIN, advInt);
        GAP_SetParamValue(TGAP_DISC_ADV_INT_MAX, advInt);

        uint8_t advertising_enable = TRUE;
        GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &advertising_enable);

        tmos_start_task(Sensor_TaskID, SBP_PERIODIC_EVT, DEFAULT_UPDATE_PERIOD);
        tmos_start_task(Sensor_TaskID, BROADCAST_DISABLE_EVT, DEFAULT_ADVERTISING_LENGTH);

        return (events ^ SBP_PERIODIC_EVT);
    }

    if(events & BROADCAST_DISABLE_EVT){
        uint8_t advertising_enable = FALSE;
        GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &advertising_enable);

        return (events ^ BROADCAST_DISABLE_EVT);
    }

    // Discard unknown events
    return 0;
}