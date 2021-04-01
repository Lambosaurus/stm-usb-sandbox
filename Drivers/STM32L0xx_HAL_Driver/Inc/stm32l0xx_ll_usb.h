/**
  ******************************************************************************
  * @file    stm32l0xx_ll_usb.h
  * @author  MCD Application Team
  * @brief   Header file of USB Low Layer HAL module.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32L0xx_LL_USB_H
#define STM32L0xx_LL_USB_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal_def.h"

#if defined (USB)



typedef enum
{
  USB_DEVICE_MODE  = 0
} USB_ModeTypeDef;

/**
@brief  USB Initialization Structure definition
typedef struct
{
  uint32_t dev_endpoints;           !< Device Endpoints number.
                                         This parameter depends on the used USB core.
                                         This parameter must be a number between Min_Data = 1 and Max_Data = 15
  uint32_t speed;                   !< USB Core speed.
                                         This parameter can be any value of @ref USB_Core_Speed
  uint32_t ep0_mps;                 !< Set the Endpoint 0 Max Packet size.
  uint32_t phy_itface;              !< Select the used PHY interface.
                                         This parameter can be any value of @ref USB_Core_PHY
  uint32_t Sof_enable;              !< Enable or disable the output of the SOF signal.
  uint32_t low_power_enable;        !< Enable or disable Low Power mode
  uint32_t lpm_enable;              !< Enable or disable Battery charging.
  uint32_t battery_charging_enable; !< Enable or disable Battery charging.
} USB_CfgTypeDef;
*/

typedef struct
{
  uint8_t   num;             /*!< Endpoint number
                                  This parameter must be a number between Min_Data = 1 and Max_Data = 15    */

  uint8_t   is_in;           /*!< Endpoint direction
                                  This parameter must be a number between Min_Data = 0 and Max_Data = 1     */

  uint8_t   is_stall;        /*!< Endpoint stall condition
                                  This parameter must be a number between Min_Data = 0 and Max_Data = 1     */

  uint8_t   type;            /*!< Endpoint type
                                  This parameter can be any value of @ref USB_EP_Type                       */

  uint8_t   data_pid_start;  /*!< Initial data PID
                                  This parameter must be a number between Min_Data = 0 and Max_Data = 1     */

  uint16_t  pmaadress;       /*!< PMA Address
                                  This parameter can be any value between Min_addr = 0 and Max_addr = 1K    */

  uint16_t  pmaaddr0;        /*!< PMA Address0
                                  This parameter can be any value between Min_addr = 0 and Max_addr = 1K    */

  uint16_t  pmaaddr1;        /*!< PMA Address1
                                  This parameter can be any value between Min_addr = 0 and Max_addr = 1K    */

  uint8_t   doublebuffer;    /*!< Double buffer enable
                                  This parameter can be 0 or 1                                              */

  uint16_t  tx_fifo_num;     /*!< This parameter is not required by USB Device FS peripheral, it is used
                                  only by USB OTG FS peripheral
                                  This parameter is added to ensure compatibility across USB peripherals    */

  uint32_t  maxpacket;       /*!< Endpoint Max packet size
                                  This parameter must be a number between Min_Data = 0 and Max_Data = 64KB  */

  uint8_t   *xfer_buff;      /*!< Pointer to transfer buffer                                                */

  uint32_t  xfer_len;        /*!< Current transfer length                                                   */

  uint32_t  xfer_count;      /*!< Partial transfer length in case of multi packet transfer                  */

  uint32_t  xfer_len_db;      /*!< double buffer transfer length used with bulk double buffer in           */

  uint8_t   xfer_fill_db;     /*!< double buffer Need to Fill new buffer  used with bulk_in                */

} USB_EPTypeDef;



#endif /* defined (USB) */

#ifdef __cplusplus
}
#endif


#endif /* STM32L0xx_LL_USB_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
