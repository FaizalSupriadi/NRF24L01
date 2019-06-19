#include "NRF24.hpp"
#include "NRF24_register.hpp"
#include "hwlib.hpp"

NRF24::NRF24( uint16_t ce, uint16_t csn ):
   ce( ce ), csn( csn )
{} 
	
uint8_t NRF24::read_register( uint8_t reg ){
   uint8_t result;
   //use SPI to read a message
   result = readSPI(NOP);
   return result;
}
	
uint8_t NRF24::write_register( uint8_t register, uint8_t value ){
   //use SPI to write a message ( R_REGISTER | ( 0x1F & reg ) )
   //verstuur daarna nog een keer via SPI een bericht maar dan alleen de value die je wilt schrijven
}
	
void NRF24::setChannel( uint8_t channel ){
   channel = ( channel < 125 ) ? channel : 125;
   write_register( RF_CH, channel );
}

uint8_t NRF24::getChannel(){
   return read_register( RF_CH );
}
