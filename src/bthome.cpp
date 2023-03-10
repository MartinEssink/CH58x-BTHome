#include "config.h"
#include "bthome.h"

// Constructor for unencrypted advertisement
BTHome::BTHome() : advDataLength(8), measLength(0), encrypt(false), counter(0){}

// Constructor for encrypted advertisement
// [in] pointer to 16-byte key.
BTHome::BTHome(uint8_t *key) : advDataLength(16), measLength(0), encrypt(true), encryptKey(key), counter( tmos_rand() ){
	// Read the MAC in reverse order to the nonce
	uint8_t mac_tmp[6];
	GetMACAddress(mac_tmp);
	for(uint8_t i=0; i<6; i++){
		nonce[i] = mac_tmp[5-i];
	}

	// Add bthome info to nonce
	nonce[6] = 0xd2;
	nonce[7] = 0xfc;
	nonce[8] = 0x41;
}

// Add a measurement to be advertised
// [in] ObjectID of the sensor type.
// [in] Callback function pointer that writes sensor data
void BTHome::addMeasurement(uint8_t objectId, bthome_meas_fn_ptr fn){
	// Check if we are not exceeding the maximum number of sensors or bytes advertised
	if(objectId < BTHOME_MAX_OBJECTID && BTHomeDataLength[objectId] < BTHOME_ADV_MAX_LEN - advDataLength){
		// Add the sensor
		measObjectIds[measLength] = objectId;
		measFns[measLength] = fn;
		measLength++;
		advDataLength += BTHomeDataLength[objectId] + 1;
	}
}

void BTHome::writeAdvData(){
	// Start writing at the beginning of advData
	advDataIndex = 0;

	// BLE Flags
	advData[advDataIndex++] = 0x02;
	advData[advDataIndex++] = BTHOME_GAP_ADTYPE_FLAGS;
	advData[advDataIndex++] = BTHOME_FLAG_BREDR_NOT_SUPPORTED|BTHOME_FLAG_GENERAL_DISCOVERABLE;

	// BTHome data header
	uint8_t bthomeDataIndex = advDataIndex++;
	advData[advDataIndex++] = BTHOME_GAP_ADTYPE_SERVICE_DATA;
	advData[advDataIndex++] = 0xD2;
	advData[advDataIndex++] = 0xFC;
	advData[advDataIndex++] = (encrypt ? 0x41 : 0x40);

	// Measurements
	for(uint8_t i=0; i<measLength; i++){
		advData[advDataIndex++] = measObjectIds[i];
		(*measFns[i])( &advData[advDataIndex] );
		advDataIndex += BTHomeDataLength[measObjectIds[i]];
	}

	// Encrypt the measurements
	if(encrypt){
		//Increment counter
		counter++;
		//Write updated counter to output and nonce
		memcpy(&nonce[9], &counter, 4);
		memcpy(&advData[advDataIndex], &counter, 4);
		//Encrypt payload and write mic
		ccmEncrypt(&advData[bthomeDataIndex + 5], advDataIndex - (bthomeDataIndex + 5), &advData[advDataIndex + 4]);
		// Account for counter and mic
		advDataIndex += 8;
	}

	// Write payload length
	advData[bthomeDataIndex] = advDataIndex - bthomeDataIndex - 1;
}

// AES-CCM encryption. Optimised for BTHome parameters.
// Uses hardware AES, see e.g. tinycrypt for a software implementation.
// [in/out] plaintext in and ciphertext out
// [in] length of plaintext/ciphertext
// [out] mic (Messiage Integrity Check)
void BTHome::ccmEncrypt(uint8_t *inout, uint8_t dataLen, uint8_t *mac){
    uint8_t m, i;
    uint8_t b[16], y[16], s[16];

    b[0] = 0x09;
    memcpy(b + 1, nonce, 13);
    b[14] = 0x00;
    b[15] = dataLen;
    LL_Encrypt(encryptKey, b, y);

    b[0] = 0x01;
    b[15] = 0x00;
    LL_Encrypt(encryptKey, b, s);
    memcpy(mac, s, 4);

    while(dataLen > 0){
       m = min(dataLen, 16);
  
       for(i = 0; i < m; i++) y[i] = inout[i] ^ y[i];
       LL_Encrypt(encryptKey, y, y);

       b[15]++;
       LL_Encrypt(encryptKey, b, s);
       for(i = 0; i < m; i++) inout[i] = inout[i] ^ s[i];

       dataLen -= m;
       inout += m;
    }
  
    for(i = 0; i < 4; i++) mac[i] = mac[i] ^ y[i];
}
