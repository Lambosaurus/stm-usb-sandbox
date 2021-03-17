#ifndef __USBD_CONF__H__
#define __USBD_CONF__H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "stm32l0xx.h"
#include "stm32l0xx_hal.h"

 extern PCD_HandleTypeDef hpcd_USB_FS;



#define USBD_MAX_NUM_INTERFACES     	1U
#define USBD_MAX_NUM_CONFIGURATION     	1U
#define USBD_MAX_STR_DESC_SIZ     		512U
#define USBD_DEBUG_LEVEL     			0U
#define USBD_SELF_POWERED     			1U

#define DEVICE_FS 						0



#endif /* __USBD_CONF__H__ */
