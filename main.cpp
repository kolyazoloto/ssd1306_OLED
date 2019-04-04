#include "main.h"



int main(void)
{
	OLED_SSD1306 led;
	_delay_us(10);
	led.clear_screen();
	//led.print_5x8(0,0,"asdafa");
	led.print_k(0,0,'1');
	led.update_display();
	while (1)
	{
	
	}
}


