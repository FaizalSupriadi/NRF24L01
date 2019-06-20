#include "NRF24.hpp"
#include "NRF24_register.hpp"
#include "hwlib.hpp"
#include <array>

NRF24::NRF24( hwlib::spi_bus & bus, uint16_t ce, uint16_t csn ):
   bus( bus ), ce( ce ), csn( csn )
{} 
	
uint8_t NRF24::read_register( uint8_t reg){
   uint8_t result = 0;
   auto cs = hwlib::target::pin_out( hwlib::target::pins::d11 );
   auto cs1 = hwlib::target::pin_out( hwlib::target::pins::d8 );
   cs.write(0); cs.flush();
   bus.transaction( cs1 ).write( R_REGISTER | ( 0x1F & reg ) );
   result = bus.transaction( cs1 ).read_byte();
   cs.write(1); cs.flush();
   if(result > 0){
      return result;
   }else{ 
      return 255; 
   }
}
	
void NRF24::write_register( uint8_t reg, uint8_t value ){
   auto cs = hwlib::target::pin_out( hwlib::target::pins::d11 );
   auto cs1 = hwlib::target::pin_out( hwlib::target::pins::d8 );
   cs.write(1); cs.flush();
   cs.write(0); cs.flush();
   bus.transaction( cs1 ).write( (W_REGISTER | ( 0x1F & reg )) );
   bus.transaction( cs1 ).write(value);
   cs.write(1); cs.flush();
}
	
void NRF24::setChannel( uint8_t channel ){
   channel = ( channel < 125 ) ? channel : 125;
   write_register( RF_CH, channel );
}

uint8_t NRF24::getChannel(){
   return read_register( RF_CH );
}

void NRF24::csn_LOW(){
   auto csn = hwlib::target::pin_out( hwlib::target::pins::d11 );
   csn.write( 0 );
   csn.flush();
}

void NRF24::ce_LOW(){
   auto ce = hwlib::target::pin_out( hwlib::target::pins::d10 );
   ce.write( 0 );
   ce.flush();
}

