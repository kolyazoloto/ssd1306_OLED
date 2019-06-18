#include "main.h"



int main(void)
{
	OLED_SSD1306_99 led;
	_delay_us(10);
	led.clear_screen();
	led.print_5x8(0,0,"54643443435");
	led.update_display();
	while (1)
	{
	
	}
}


