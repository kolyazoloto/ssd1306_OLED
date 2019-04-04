#ifndef OLED_SSD1306_H_
#define OLED_SSD1306_H_

#define RES PB1
#define DC PB4
#define CS PB2
#define SCL PB5

class OLED_SSD1306{
public:
	OLED_SSD1306(){
		init_display();
	}
void update_display(){
	send_command(0x21); //set columm address
	send_command(32);  //set low col address
	send_command(96);  //set high col address
	
	send_command(0x22); //set page address
	send_command(0);
	send_command(5);
	for(uint16_t i = 0; i < 384; i++) send_data(scr_buffer[i]);
}
void clear_screen()
{
	uint16_t k;
	for(k=0;k<384;k++)
	{
		scr_buffer[k] = 0;
	}
}
void set_invert(bool state){
	if(state) send_command(0xA7);
	else send_command(0xA6);
}
void print_5x8(uint8_t column, uint8_t string, uint8_t sign) // 0..63 / 0..5 / Символ
{
	
	if(column <= 64 && string < 6) // Проверяем установку столбца и строки
	{
		if((sign >= 0x20) && (sign <= 0x7F))
		sign -= 32;// Смещение в таблице для символов ASCII[0x20-0x7F]
		else if (sign >= 0xC0)
		sign -= 96; // Смещение в таблице для символов CP1251[0xC0-0xFF]
		else  sign = 85; // Остальные игнорируем (их просто нет в таблице для экономии памяти)
		
		for(uint8_t i = 0; i < 5; i++)
		{
			scr_buffer[65*string + column + i] = pgm_read_byte(&font_5x8[5*sign + i]);
		}
	}
}
void print_5x8(uint8_t column, uint8_t string, char *str) // 0..120 / 0..7 / Строка
{
	while(*str)
	{
		print_5x8(column, string, *str++);
		column+=6;
		if (column > 64) {
			column-=64;
			string++;
		}
	}
}
unsigned char print_k(uint8_t column, uint8_t page, uint8_t sign) // 0..63 / 0..5 / Символ
{
	if(sign >= 0x30 && sign <= 0x39) {
		sign -= 34;
	}
	else{
		sign = 85;
	}
	unsigned char fontHeight = pgm_read_byte(&microsoftTaiLe_25ptBitmaps[1]); // Высота
	unsigned char amountOfElems = pgm_read_byte(&microsoftTaiLe_25ptBitmaps[3]); //Количество симфолов в шрифте
	unsigned char fontWidth = pgm_read_byte(&microsoftTaiLe_25ptBitmaps[sign - amountOfElems]);  //Ширина
	// Находим координату в массиве. Т.к начинаеются с сотого и пока не совпадет с нашим суммируем
	unsigned char l=4+amountOfElems;      // Начальное значение откуда начинаются символы
	int widthSum = 0;		// Переменная для вычисления суммы сколько до этого в массиве шрифта было значений
	int widthStartAddres = 4;  // Данные о величине шрифта
	while(l != sign){
		widthSum += pgm_read_byte(&microsoftTaiLe_25ptBitmaps[widthStartAddres]);
		widthStartAddres++;
		l++;
	}
	widthSum *= fontHeight/8;
	int charStartPos = 4+amountOfElems+widthSum;
	if(column <= 64 && page < 6) // Проверяем установку столбца и строки
	{
		for(uint8_t k = 0;k<fontHeight/8;k++){
			for(uint8_t i = 0; i < fontWidth; i++){
				scr_buffer[65*(page+k) + column + i] = pgm_read_byte(&microsoftTaiLe_25ptBitmaps[charStartPos+(fontWidth*k) + i]);
			}
		}
	}
	return fontWidth;
}
private:
uint8_t scr_buffer[384];
void init_display(){
	
	DDRB |= (1<<PB1)|(1<<PB2)|(1<<PB3)|(1<<PB4)|(1<<PB5);
	_delay_us(100);
	restart();
	send_command(0xAE); //Выключаем экран
	
	send_command(0xD5); //Set Display Clock Divide Ratio/ Oscillator Frequency
	send_command(0x80);
	
	send_command(0xA8); //Set Multiplex Ratio
	send_command(0x2F); //64 x 48  koroche vibor razmerov
	
	send_command(0xD3);// Set Display Offset
	send_command(0x00);
	
	send_command(0x40); //Начало памяти
	
	send_command(0x8D);// Управление внутреним преобразователем
	send_command(0x14);// 0x10 - отключить (VCC подается извне) 0x14 - запустить внутрений
	
	send_command(0xA1); // Режим разверки по странице (по X)
	
	send_command(0xC8); // Режим сканирования озу дисплея
	
	send_command(0xDA); // Аппаратная конфигурация COM
	send_command(0x12);
	
	send_command(0x81); //
	send_command(0xCF);
	
	send_command(0xD9);
	send_command(0x22);
	
	send_command(0xDB);
	send_command(0x0);
	
	send_command(0x20); // Режим автоматической адресации
	send_command(0x00); // 0-по горизонтали с переходом на новую страницу (строку)
						// 1 - по вертикали с переходом на новую строку
						// 2 - только по выбранной странице без перехода
	send_command(0xA4); //Set entire display on/off
	
	send_command(0xA6); //set normal/inverse display
	
	send_command(0xAF);
}	
void send_command(unsigned char com){
	PORTB &= ~(1<<DC); //DC0
	PORTB &= ~(1<<CS);
	for(uint8_t bit = 0x80; bit; bit >>= 1) {
		PORTB &= ~(1<<SCL);
		if(com & bit) PORTB |=  (1<<3);
		else        PORTB &= ~(1<<3);
		PORTB |=  (1<<SCL);
	}
	PORTB |= (1<<CS);
	PORTB |= (1<<DC);
}
void send_data(unsigned char data){
	PORTB |= (1<<DC); //DC1
	PORTB &= ~(1<<CS);
	for(uint8_t bit = 0x80; bit; bit >>= 1) {
		PORTB &= ~(1<<SCL);
		if(data & bit) PORTB |=  (1<<3);
		else        PORTB &= ~(1<<3);
		PORTB |=  (1<<SCL);
	}
	PORTB |= (1<<CS);
}
void restart(){
	PORTB &= ~(1<<RES);
	_delay_us(10);
	PORTB |= (1<<RES);
	_delay_us(10);
}
};

#endif /* MAIN_H_ */