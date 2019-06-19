#include "NRF24.hpp"
#include "NRF24_register.hpp"
#include "hwlib.hpp"

NRF24::NRF24( hwlib::spi_bus & bus, uint16_t ce, uint16_t csn ):
   bus( bus ), ce( ce ), csn( csn )
{} 
	
uint8_t NRF24::read_register( uint8_t reg ){
   uint8_t result;
   bus.write( R_REGISTER | ( 0x1F & reg ) );
   result = bus.read_byte(NOP);
   return result;
}
	
uint8_t NRF24::write_register( uint8_t register, uint8_t value ){
   bus.write( R_REGISTER | ( 0x1F & reg ) );
   bus.write(value);
   return 1;
}
	
void NRF24::setChannel( uint8_t channel ){
   channel = ( channel < 125 ) ? channel : 125;
   write_register( RF_CH, channel );
}

uint8_t NRF24::getChannel(){
   return read_register( RF_CH );
}
