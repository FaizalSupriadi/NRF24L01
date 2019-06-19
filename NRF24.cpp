#include "NRF24.hpp"
#include "NRF24_register.hpp"
#include "hwlib.hpp"

//bus.transaction( sel ).write( write_buffer );

NRF24::NRF24( hwlib::spi_bus & bus, uint16_t ce, uint16_t csn ):
   bus( bus ), ce( ce ), csn( csn )
{} 
	
uint8_t NRF24::read_register( uint8_t reg ){
   uint8_t result;
   auto cs = hwlib::target::pin_out( hwlib::target::pins::d11 );
   bus.transaction( cs ).write( R_REGISTER | ( 0x1F & reg ) );
   result = bus.transaction( cs ).read_byte();
   if(result > 0){
      return result;
   }else{
      return 255;
   }
}
	
uint8_t NRF24::write_register( uint8_t reg, uint8_t value ){
   auto cs = hwlib::target::pin_out( hwlib::target::pins::d11 );
   bus.transaction( cs ).write( W_REGISTER | ( 0x1F & reg ) );
   bus.transaction( cs ).write(value);
   return 1;
}
	
void NRF24::setChannel( uint8_t channel ){
   channel = ( channel < 125 ) ? channel : 125;
   write_register( RF_CH, channel );
}

uint8_t NRF24::getChannel(){
   return read_register( RF_CH );
}

