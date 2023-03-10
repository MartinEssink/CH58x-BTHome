#ifndef BROADCASTER_H
#define BROADCASTER_H

// Simple BLE Broadcaster Task Events
#define SBP_PERIODIC_EVT             0x0001
#define BROADCAST_DISABLE_EVT  		 0x0002

// Task Initialization for the BLE Sensor
extern void Sensor_Init(void);

// Task Event Processor for the BLE Sensor
extern uint16_t Sensor_ProcessEvent(uint8_t task_id, uint16_t events);

#endif
