

#include "main.h"

#include "Core.h"
#include "USB.h"

#include <string.h>

const char * msgs[] = {
		"One\n",
		"Two\n",
		"Three\n",
		"Four\n",
		"Five\n",
		"Six\n",
		"Seven\n",
		"Eight\n",
		"Nine\n",
		"Zero\n",
};

int main(void)
{
  CORE_Init();
  USB_Init();

  int msgn = 0;

  while (1)
  {
	uint32_t now = HAL_GetTick();

	static uint32_t tide = 0;
	if (now - tide > 1000)
	{
		const char * msg = msgs[msgn++];
		if (msgn >= sizeof(msgs) / sizeof(*msgs)) { msgn = 0; }
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




