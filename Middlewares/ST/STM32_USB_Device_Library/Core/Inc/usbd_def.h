#ifndef __USBD_DEF_H
#define __USBD_DEF_H

#include <stdint.h>
#include <string.h>

#include "usb\USB_Defs.h"


/* USB Device handle structure */
typedef struct
{
	uint32_t                total_length;
	uint32_t                rem_length;
	uint32_t                maxpacket;
} USBD_EndpointTypeDef;

/* USB Device handle structure */
typedef struct _USBD_HandleTypeDef
{
  USBD_EndpointTypeDef    ep_in[16];
  USBD_EndpointTypeDef    ep_out[16];
  //uint32_t                ep0_data_len;
} USBD_HandleTypeDef;

#endif


