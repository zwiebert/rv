/**
 * \file     peri/i2c.h
 * \brief    Set up and check  IIC Bus
 */

#pragma once

#include <stdbool.h>

/// \brief   Check the IIC bus and restart it if it's dead.
bool i2c2_check();

/// \brief  Set up IIC bus at startup.
void i2c2_setup(void);
