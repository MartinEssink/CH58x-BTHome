#define BTHOME_ADV_MAX_LEN 						31
#define BTHOME_ADV_MAX_LEN 						31
#define BTHOME_MEAS_MAX_N						15

#define BTHOME_GAP_ADTYPE_FLAGS					0x01
#define BTHOME_GAP_ADTYPE_SERVICE_DATA			0x16

#define BTHOME_FLAG_LIMITED_DISCOVERABLE		0x01 
#define BTHOME_FLAG_GENERAL_DISCOVERABLE		0x02 
#define BTHOME_FLAG_BREDR_NOT_SUPPORTED 		0x04 

#define BTHOME_OBJECTID_PACKET_ID             	0x00
#define BTHOME_OBJECTID_BATTERY               	0x01
#define BTHOME_OBJECTID_TEMPERATURE_PRECISE   	0x02
#define BTHOME_OBJECTID_HUMIDITY_PRECISE      	0x03
#define BTHOME_OBJECTID_PRESSURE              	0x04
#define BTHOME_OBJECTID_ILLUMINANCE           	0x05
#define BTHOME_OBJECTID_MASS_KILOS            	0x06
#define BTHOME_OBJECTID_MASS_POUNDS           	0x07
#define BTHOME_OBJECTID_DEW_POINT             	0x08
#define BTHOME_OBJECTID_COUNT_SMALL           	0x09
#define BTHOME_OBJECTID_ENERGY                	0x0A
#define BTHOME_OBJECTID_POWER                 	0x0B
#define BTHOME_OBJECTID_VOLTAGE               	0x0C
#define BTHOME_OBJECTID_PM_2_5                	0x0D
#define BTHOME_OBJECTID_PM_10                 	0x0E
#define BTHOME_OBJECTID_GENERIC_BOOL          	0x0F
#define BTHOME_OBJECTID_POWER_STATE           	0x10
#define BTHOME_OBJECTID_OPENING_STATE         	0x11
#define BTHOME_OBJECTID_CO2                   	0x12
#define BTHOME_OBJECTID_TOTAL_VOC             	0x13
#define BTHOME_OBJECTID_MOISTURE_PRECISE      	0x14
#define BTHOME_OBJECTID_BATTERY_STATE         	0x15
#define BTHOME_OBJECTID_BATTERY_CHARGE_STATE  	0x16
#define BTHOME_OBJECTID_CARBON_MONOXIDE_STATE 	0x17
#define BTHOME_OBJECTID_COLD_STATE            	0x18
#define BTHOME_OBJECTID_CONNECTIVITY_STATE    	0x19
#define BTHOME_OBJECTID_DOOR_STATE            	0x1A
#define BTHOME_OBJECTID_GARAGE_DOOR_STATE     	0x1B
#define BTHOME_OBJECTID_GAS_STATE             	0x1C
#define BTHOME_OBJECTID_HEAT_STATE            	0x1D
#define BTHOME_OBJECTID_LIGHT_STATE           	0x1E
#define BTHOME_OBJECTID_LOCK_STATE            	0x1F
#define BTHOME_OBJECTID_MOISTURE_STATE        	0x20
#define BTHOME_OBJECTID_MOTION_STATE          	0x21
#define BTHOME_OBJECTID_MOVING_STATE          	0x22
#define BTHOME_OBJECTID_OCCUPANCY_STATE       	0x23
#define BTHOME_OBJECTID_PLUG_STATE            	0x24
#define BTHOME_OBJECTID_PRESENCE_STATE        	0x25
#define BTHOME_OBJECTID_PROBLEM_STATE         	0x26
#define BTHOME_OBJECTID_RUNNING_STATE         	0x27
#define BTHOME_OBJECTID_SAFETY_STATE          	0x28
#define BTHOME_OBJECTID_SMOKE_STATE           	0x29
#define BTHOME_OBJECTID_SOUND_STATE           	0x2A
#define BTHOME_OBJECTID_TAMPER_STATE          	0x2B
#define BTHOME_OBJECTID_VIBRATION_STATE       	0x2C
#define BTHOME_OBJECTID_WINDOW_STATE          	0x2D
#define BTHOME_OBJECTID_HUMIDITY_COARSE       	0x2E
#define BTHOME_OBJECTID_MOISTURE_COARSE       	0x2F
#define BTHOME_OBJECTID_BUTTON_EVENT          	0x3A
#define BTHOME_OBJECTID_DIMMER_EVENT          	0x3C
#define BTHOME_OBJECTID_COUNT_MEDIUM          	0x3D
#define BTHOME_OBJECTID_COUNT_LARGE           	0x3E
#define BTHOME_OBJECTID_ROTATION              	0x3F
#define BTHOME_OBJECTID_DISTANCE_MILLIMETERS  	0x40
#define BTHOME_OBJECTID_DISTANCE_METERS       	0x41
#define BTHOME_OBJECTID_DURATION              	0x42
#define BTHOME_OBJECTID_CURRENT               	0x43
#define BTHOME_OBJECTID_SPEED                 	0x44
#define BTHOME_OBJECTID_TEMPERATURE_COARSE    	0x45
#define BTHOME_OBJECTID_UV_INDEX              	0x46
#define BTHOME_OBJECTID_VOLUME_LITERS_COURSE    0x47
#define BTHOME_OBJECTID_VOLUME_MILLILITERS      0x48
#define BTHOME_OBJECTID_VOLUME_FLOW_RATE        0x49
#define BTHOME_OBJECTID_VOLTAGE_COURSE          0x4A
#define BTHOME_OBJECTID_GAS_VOLUME              0x4B
#define BTHOME_OBJECTID_GAS_VOLUME2             0x4C
#define BTHOME_OBJECTID_ENERGY_KWH              0x4D
#define BTHOME_OBJECTID_VOLUME_LITERS           0x4E
#define BTHOME_OBJECTID_WATER_VOLUME_LITERS     0x4F


#define BTHOME_MAX_OBJECTID		             	0x50

typedef void (*bthome_meas_fn_ptr)(uint8_t*);

const uint8_t BTHomeDataLength[BTHOME_MAX_OBJECTID] = {
//  0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F,
	1,1,2,2,3,3,2,2,2,1,3,3,2,2,2,1,//0
	1,1,2,1,2,1,1,1,1,1,1,1,1,1,1,1,//1
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,//2
	0,0,0,0,0,0,0,0,0,0,1,0,1,2,4,2,//3
	2,2,3,2,2,2,1,2,2,2,2,3,4,4,4,4,//4
	};//5

class BTHome{
public:
	uint8_t advData[BTHOME_ADV_MAX_LEN];
	uint8_t advDataLength;

	BTHome();
	BTHome(uint8_t *key);
	void addMeasurement(uint8_t objectId, bthome_meas_fn_ptr fn);
	void writeAdvData();

private:
	uint8_t advDataIndex, bthomeDataIndex;

	uint8_t measObjectIds[BTHOME_MEAS_MAX_N];
	bthome_meas_fn_ptr measFns[BTHOME_MEAS_MAX_N];
	uint8_t measLength;

	bool encrypt;
	uint8_t *encryptKey;
	uint8_t nonce[13];
	uint32_t counter;

	void ccmEncrypt(uint8_t *inout, uint8_t dataLen, uint8_t *mac);
};