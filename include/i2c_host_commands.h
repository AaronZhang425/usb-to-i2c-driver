// Represents different commands the host can send to the I2C slave
// All commands should be representable by a single byte

#ifndef I2C_DEV_COMMANDS_H
#define I2C_DEV_COMMANDS_H

// Arbitraily chosen. Will find better solution later.
#define DEVICE_STATE_SIZE 64 

#define GET_MAX_USB_DEV_SIG 0
#define UPDATE_DEVICE_STATES_SIG 1
#define POLL_DEIVCES_SIG 2

#endif