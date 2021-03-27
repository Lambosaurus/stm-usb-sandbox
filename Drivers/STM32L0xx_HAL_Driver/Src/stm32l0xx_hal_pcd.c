#include "stm32l0xx_hal.h"


#ifdef HAL_PCD_MODULE_ENABLED

#if defined (USB)

#define PCD_MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define PCD_MAX(a, b)  (((a) > (b)) ? (a) : (b))


HAL_StatusTypeDef HAL_PCD_Init(PCD_HandleTypeDef *hpcd)
{
  uint8_t i;

  /* Check the PCD handle allocation */
  if (hpcd == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_PCD_ALL_INSTANCE(hpcd->Instance));

  if (hpcd->State == HAL_PCD_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hpcd->Lock = HAL_UNLOCKED;

#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
    hpcd->SOFCallback = HAL_PCD_SOFCallback;
    hpcd->SetupStageCallback = HAL_PCD_SetupStageCallback;
    hpcd->ResetCallback = HAL_PCD_ResetCallback;
    hpcd->SuspendCallback = HAL_PCD_SuspendCallback;
    hpcd->ResumeCallback = HAL_PCD_ResumeCallback;
    hpcd->ConnectCallback = HAL_PCD_ConnectCallback;
    hpcd->DisconnectCallback = HAL_PCD_DisconnectCallback;
    hpcd->DataOutStageCallback = HAL_PCD_DataOutStageCallback;
    hpcd->DataInStageCallback = HAL_PCD_DataInStageCallback;
    hpcd->ISOOUTIncompleteCallback = HAL_PCD_ISOOUTIncompleteCallback;
    hpcd->ISOINIncompleteCallback = HAL_PCD_ISOINIncompleteCallback;
    hpcd->LPMCallback = HAL_PCDEx_LPM_Callback;
    hpcd->BCDCallback = HAL_PCDEx_BCD_Callback;

    if (hpcd->MspInitCallback == NULL)
    {
      hpcd->MspInitCallback = HAL_PCD_MspInit;
    }

    /* Init the low level hardware */
    hpcd->MspInitCallback(hpcd);
#else
    /* Init the low level hardware : GPIO, CLOCK, NVIC... */
    HAL_PCD_MspInit(hpcd);
#endif /* (USE_HAL_PCD_REGISTER_CALLBACKS) */
  }

  hpcd->State = HAL_PCD_STATE_BUSY;

  /* Disable the Interrupts */
  __HAL_PCD_DISABLE(hpcd);

  /* Init endpoints structures */
  for (i = 0U; i < hpcd->Init.dev_endpoints; i++)
  {
    /* Init ep structure */
    hpcd->IN_ep[i].is_in = 1U;
    hpcd->IN_ep[i].num = i;
    hpcd->IN_ep[i].tx_fifo_num = i;
    /* Control until ep is activated */
    hpcd->IN_ep[i].type = EP_TYPE_CTRL;
    hpcd->IN_ep[i].maxpacket = 0U;
    hpcd->IN_ep[i].xfer_buff = 0U;
    hpcd->IN_ep[i].xfer_len = 0U;
  }

  for (i = 0U; i < hpcd->Init.dev_endpoints; i++)
  {
    hpcd->OUT_ep[i].is_in = 0U;
    hpcd->OUT_ep[i].num = i;
    /* Control until ep is activated */
    hpcd->OUT_ep[i].type = EP_TYPE_CTRL;
    hpcd->OUT_ep[i].maxpacket = 0U;
    hpcd->OUT_ep[i].xfer_buff = 0U;
    hpcd->OUT_ep[i].xfer_len = 0U;
  }

  /* Init Device */
  (void)USB_DevInit(hpcd->Instance, hpcd->Init);

  hpcd->USB_Address = 0U;
  hpcd->State = HAL_PCD_STATE_READY;
  
  /* Activate LPM */
  if (hpcd->Init.lpm_enable == 1U)
  {
    (void)HAL_PCDEx_ActivateLPM(hpcd);
  }

  return HAL_OK;
}

HAL_StatusTypeDef HAL_PCD_DeInit(PCD_HandleTypeDef *hpcd)
{
  /* Check the PCD handle allocation */
  if (hpcd == NULL)
  {
    return HAL_ERROR;
  }

  hpcd->State = HAL_PCD_STATE_BUSY;

  /* Stop Device */
  if (USB_StopDevice(hpcd->Instance) != HAL_OK)
  {
    return HAL_ERROR;
  }

#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
  if (hpcd->MspDeInitCallback == NULL)
  {
    hpcd->MspDeInitCallback = HAL_PCD_MspDeInit; /* Legacy weak MspDeInit  */
  }

  /* DeInit the low level hardware */
  hpcd->MspDeInitCallback(hpcd);
#else
  /* DeInit the low level hardware: CLOCK, NVIC.*/
  HAL_PCD_MspDeInit(hpcd);
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */

  hpcd->State = HAL_PCD_STATE_RESET;

  return HAL_OK;
}

__weak void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd)
{
  UNUSED(hpcd);
}

__weak void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd)
{
  UNUSED(hpcd);
}

HAL_StatusTypeDef HAL_PCD_Start(PCD_HandleTypeDef *hpcd)
{
  __HAL_LOCK(hpcd);
  __HAL_PCD_ENABLE(hpcd);
  (void)USB_DevConnect(hpcd->Instance);
  __HAL_UNLOCK(hpcd);

  return HAL_OK;
}

HAL_StatusTypeDef HAL_PCD_Stop(PCD_HandleTypeDef *hpcd)
{
  __HAL_LOCK(hpcd);
  __HAL_PCD_DISABLE(hpcd);
  (void)USB_DevDisconnect(hpcd->Instance);
  __HAL_UNLOCK(hpcd);

  return HAL_OK;
}

HAL_StatusTypeDef HAL_PCD_EP_Receive(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint8_t *pBuf, uint32_t len)
{
  PCD_EPTypeDef *ep;

  ep = &hpcd->OUT_ep[ep_addr & EP_ADDR_MSK];

  /*setup and start the Xfer */
  ep->xfer_buff = pBuf;
  ep->xfer_len = len;
  ep->xfer_count = 0U;
  ep->is_in = 0U;
  ep->num = ep_addr & EP_ADDR_MSK;

  if ((ep_addr & EP_ADDR_MSK) == 0U)
  {
    (void)USB_EP0StartXfer(hpcd->Instance, ep);
  }
  else
  {
    (void)USB_EPStartXfer(hpcd->Instance, ep);
  }

  return HAL_OK;
}

HAL_StatusTypeDef HAL_PCD_ActivateRemoteWakeup(PCD_HandleTypeDef *hpcd)
{
  return (USB_ActivateRemoteWakeup(hpcd->Instance));
}

HAL_StatusTypeDef HAL_PCD_DeActivateRemoteWakeup(PCD_HandleTypeDef *hpcd)
{
  return (USB_DeActivateRemoteWakeup(hpcd->Instance));
}

PCD_StateTypeDef HAL_PCD_GetState(PCD_HandleTypeDef *hpcd)
{
  return hpcd->State;
}

#endif
#endif


