/**
  ******************************************************************************
  * @file    stm32l0xx_ll_usb.c
  * @author  MCD Application Team
  * @brief   USB Low Layer HAL module driver.
  *
  *          This file provides firmware functions to manage the following
  *          functionalities of the USB Peripheral Controller:
  *           + Initialization/de-initialization functions
  *           + I/O operation functions
  *           + Peripheral Control functions
  *           + Peripheral State functions
  *
  @verbatim
  ==============================================================================
                    ##### How to use this driver #####
  ==============================================================================
    [..]
      (#) Fill parameters of Init structure in USB_OTG_CfgTypeDef structure.

      (#) Call USB_CoreInit() API to initialize the USB Core peripheral.

      (#) The upper HAL HCD/PCD driver will call the right routines for its internal processes.

  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
#include "usb/USB_EP.h"

/** @addtogroup STM32L0xx_LL_USB_DRIVER
  * @{
  */

#if defined (HAL_PCD_MODULE_ENABLED) || defined (HAL_HCD_MODULE_ENABLED)
#if defined (USB)
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  USB_EPStartXfer setup and starts a transfer over an EP
  * @param  USBx Selected device
  * @param  ep pointer to endpoint structure
  * @retval HAL status
  */
HAL_StatusTypeDef USB_EPStartXfer(USB_TypeDef *USBx, USB_EPTypeDef *ep)
{
  uint32_t len;
  uint16_t pmabuffer;
  uint16_t wEPVal;

  /* IN endpoint */
  if (ep->is_in == 1U)
  {
    /*Multi packet transfer*/
    if (ep->xfer_len > ep->maxpacket)
    {
      len = ep->maxpacket;
    }
    else
    {
      len = ep->xfer_len;
    }

    /* configure and validate Tx endpoint */
    if (ep->doublebuffer == 0U)
    {
      USB_PMA_Write(ep->pmaadress, ep->xfer_buff, len);
      PCD_SET_EP_TX_CNT(USBx, ep->num, len);
    }
    else
    {
      /* double buffer bulk management */
      if (ep->type == EP_TYPE_BULK)
      {
        if (ep->xfer_len_db > ep->maxpacket)
        {
          /* enable double buffer */
          PCD_SET_EP_DBUF(USBx, ep->num);

          /* each Time to write in PMA xfer_len_db will */
          ep->xfer_len_db -= len;

          /* Fill the two first buffer in the Buffer0 & Buffer1 */
          if ((PCD_GET_ENDPOINT(USBx, ep->num) & USB_EP_DTOG_TX) != 0U)
          {
            /* Set the Double buffer counter for pmabuffer1 */
            PCD_SET_EP_DBUF1_CNT(USBx, ep->num, ep->is_in, len);
            pmabuffer = ep->pmaaddr1;

            /* Write the user buffer to USB PMA */
            USB_PMA_Write(pmabuffer, ep->xfer_buff, (uint16_t)len);
            ep->xfer_buff += len;

            if (ep->xfer_len_db > ep->maxpacket)
            {
              ep->xfer_len_db -= len;
            }
            else
            {
              len = ep->xfer_len_db;
              ep->xfer_len_db = 0U;
            }

            /* Set the Double buffer counter for pmabuffer0 */
            PCD_SET_EP_DBUF0_CNT(USBx, ep->num, ep->is_in, len);
            pmabuffer = ep->pmaaddr0;

            /* Write the user buffer to USB PMA */
            USB_PMA_Write(pmabuffer, ep->xfer_buff, len);
          }
          else
          {
            /* Set the Double buffer counter for pmabuffer0 */
            PCD_SET_EP_DBUF0_CNT(USBx, ep->num, ep->is_in, len);
            pmabuffer = ep->pmaaddr0;

            /* Write the user buffer to USB PMA */
            USB_PMA_Write(pmabuffer, ep->xfer_buff, len);
            ep->xfer_buff += len;

            if (ep->xfer_len_db > ep->maxpacket)
            {
              ep->xfer_len_db -= len;
            }
            else
            {
              len = ep->xfer_len_db;
              ep->xfer_len_db = 0U;
            }

            /* Set the Double buffer counter for pmabuffer1 */
            PCD_SET_EP_DBUF1_CNT(USBx, ep->num, ep->is_in, len);
            pmabuffer = ep->pmaaddr1;

            /* Write the user buffer to USB PMA */
            USB_PMA_Write(pmabuffer, ep->xfer_buff, (uint16_t)len);
          }
        }
        /* auto Switch to single buffer mode when transfer <Mps no need to manage in double buffer */
        else
        {
          len = ep->xfer_len_db;

          /* disable double buffer mode */
          PCD_CLEAR_EP_DBUF(USBx, ep->num);

          /* Set Tx count with nbre of byte to be transmitted */
          PCD_SET_EP_TX_CNT(USBx, ep->num, len);
          pmabuffer = ep->pmaaddr0;

          /* Write the user buffer to USB PMA */
          USB_PMA_Write(pmabuffer, ep->xfer_buff, (uint16_t)len);
        }
      }/* end if bulk double buffer */

      /* manage isochronous double buffer IN mode */
      else
      {
        /* Write the data to the USB endpoint */
        if ((PCD_GET_ENDPOINT(USBx, ep->num) & USB_EP_DTOG_TX) != 0U)
        {
          /* Set the Double buffer counter for pmabuffer1 */
          PCD_SET_EP_DBUF1_CNT(USBx, ep->num, ep->is_in, len);
          pmabuffer = ep->pmaaddr1;
        }
        else
        {
          /* Set the Double buffer counter for pmabuffer0 */
          PCD_SET_EP_DBUF0_CNT(USBx, ep->num, ep->is_in, len);
          pmabuffer = ep->pmaaddr0;
        }

        USB_PMA_Write(pmabuffer, ep->xfer_buff, (uint16_t)len);
        PCD_FreeUserBuffer(USBx, ep->num, ep->is_in);
      }
    }

    PCD_SET_EP_TX_STATUS(USBx, ep->num, USB_EP_TX_VALID);
  }
  else /* OUT endpoint */
  {
    if (ep->doublebuffer == 0U)
    {
      /* Multi packet transfer */
      if (ep->xfer_len > ep->maxpacket)
      {
        len = ep->maxpacket;
        ep->xfer_len -= len;
      }
      else
      {
        len = ep->xfer_len;
        ep->xfer_len = 0U;
      }
      /* configure and validate Rx endpoint */
      PCD_SET_EP_RX_CNT(USBx, ep->num, len);
    }
    else
    {
      /* First Transfer Coming From HAL_PCD_EP_Receive & From ISR */
      /* Set the Double buffer counter */
      if (ep->type == EP_TYPE_BULK)
      {
        PCD_SET_EP_DBUF_CNT(USBx, ep->num, ep->is_in, ep->maxpacket);

        /* Coming from ISR */
        if (ep->xfer_count != 0U)
        {
          /* update last value to check if there is blocking state */
          wEPVal = PCD_GET_ENDPOINT(USBx, ep->num);

          /*Blocking State */
          if ((((wEPVal & USB_EP_DTOG_RX) != 0U) && ((wEPVal & USB_EP_DTOG_TX) != 0U)) ||
              (((wEPVal & USB_EP_DTOG_RX) == 0U) && ((wEPVal & USB_EP_DTOG_TX) == 0U)))
          {
            PCD_FreeUserBuffer(USBx, ep->num, 0U);
          }
        }
      }
      /* iso out double */
      else if (ep->type == EP_TYPE_ISOC)
      {
        /* Multi packet transfer */
        if (ep->xfer_len > ep->maxpacket)
        {
          len = ep->maxpacket;
          ep->xfer_len -= len;
        }
        else
        {
          len = ep->xfer_len;
          ep->xfer_len = 0U;
        }
        PCD_SET_EP_DBUF_CNT(USBx, ep->num, ep->is_in, len);
      }
      else
      {
        return HAL_ERROR;
      }
    }

    PCD_SET_EP_RX_STATUS(USBx, ep->num, USB_EP_RX_VALID);
  }

  return HAL_OK;
}


/**
  * @}
  */

/**
  * @}
  */
#endif /* defined (USB) */
#endif /* defined (HAL_PCD_MODULE_ENABLED) || defined (HAL_HCD_MODULE_ENABLED) */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
