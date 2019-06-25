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
   nrf.setChannel( 100 );
   nrf.read_pipe();                     
   nrf.setOutputPower( 0 );
   nrf.powerUp_rx();

   uint8_t value; 

   for(;;){
      value = nrf.read_register( RX_ADDR_P0 );
      hwlib::cout << "value: " << value << "\n" << hwlib::flush;
   }   
}
