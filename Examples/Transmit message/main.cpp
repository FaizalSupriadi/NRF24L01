#include "hwlib.hpp"
#include "NRF24.hpp"
#include "NRF24_register.hpp"

int main( void ){

   namespace target = hwlib::target;

   auto sclk = target::pin_out( target::pins::d9 );
   auto mosi = target::pin_out( target::pins::d12 );
   auto miso = target::pin_in( target::pins::d13 );
   auto csn  = target::pin_out( target::pins::d11 );
   auto ce   = target::pin_out( target::pins::d10 );

   hwlib::wait_ms( 100 );

   auto spi_bus = hwlib::spi_bus_bit_banged_sclk_mosi_miso( 
      sclk, mosi, miso );

   auto nrf = NRF24( spi_bus, ce, csn );
   nrf.start();
   nrf.setChannel( 100 );                   //sets frequency to 2500 MHz(2400 + 100)
   nrf.write_pipe( 1 );                     
   nrf.setOutputPower( 0 );                 //sets the output power to the lowest (-18 dBm) see NRF24.cpp and NRF24.hpp for more information about this
   nrf.powerDown_rx();
   
   for(;;){
      nrf.write( 1 );                       //tries to send 1 (can only be 8 bits max for now)
   }
}
