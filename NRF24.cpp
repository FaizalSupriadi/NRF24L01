#include "NRF24.hpp"
#include "NRF24_register.hpp"
#include "hwlib.hpp"
#include <array>

NRF24::NRF24( hwlib::spi_bus & bus, hwlib::pin_out & ce, hwlib::pin_out & csn ):
   bus( bus ), ce( ce ), csn( csn )
{} 
	
uint8_t NRF24::read_register( uint8_t reg){
   set_csn(0);
   bus.transaction( hwlib::pin_out_dummy ).write( R_REGISTER | ( 0x1F & reg ) );
   uint8_t result = bus.transaction( hwlib::pin_out_dummy ).read_byte();
   set_csn(1);
   return result;
}
	
void NRF24::write_register( uint8_t reg, uint8_t value ){
   set_csn(0);
   bus.transaction( hwlib::pin_out_dummy ).write( (W_REGISTER | ( 0x1F & reg )) );
   bus.transaction( hwlib::pin_out_dummy ).write(value);
   set_csn(1);
}
	
void NRF24::setChannel( uint8_t channel ){
   channel = ( channel < 125 ) ? channel : 125;
   write_register( RF_CH, channel );
}

uint8_t NRF24::getChannel(){
   return read_register( RF_CH );
}

void NRF24::set_csn( bool x ){
   csn.write( x );
   csn.flush();
}

void NRF24::set_ce( bool x ){
   ce.write( x );
   ce.flush();
}
