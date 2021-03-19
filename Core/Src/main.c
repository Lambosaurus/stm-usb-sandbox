

#include "main.h"

#include "Core.h"
#include "USB.h"

#include <string.h>

int main(void)
{
  CORE_Init();
  USB_Init();

  while (1)
  {
	uint32_t now = HAL_GetTick();

	static uint32_t tide = 0;
	if (now - tide > 1000)
	{
		char * msg = "Mark\n";
		USB_Tx((uint8_t *)msg, strlen(msg));
		tide = now;
	}

	uint8_t bfr[128];
	uint32_t read = USB_Rx(bfr, sizeof(bfr));
	if (read)
	{
		USB_Tx(bfr, read);
	}

	CORE_Idle();
  }
}


void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}




