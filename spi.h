#ifndef SPI_H_
#define SPI_H_
#include "main.h"
void SPI_MasterInit(){
	/* Set MOSI and SCK output, all others input */
	DDRB = (1<<PB3)|(1<<PB5);
	/* Enable SPI, Master, set clock rate fck/64 */
	SPCR = (1<<SPE)|(1<<MSTR)|(SPR1);
}
void SPI_MasterTransmit(unsigned char data){
	/* Start transmission */
	SPDR = data;
	/* Wait for transmission complete */
	while(!SPSR & (1<SPIF));
}
void SPI_SlaveInit(){
	/* Set MISO output, all others input */
	DDRB = (1<<PB4);
	/* Enable SPI */
	SPCR = (1<<SPE);
}
unsigned char SPI_SlaveReceive(){
	/* Wait for reception complete */
	while(!(SPSR & (1<<SPIF)));
	return SPDR;
}


#endif /* SPI_H_ */