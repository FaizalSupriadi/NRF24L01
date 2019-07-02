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

   auto spi_bus = hwlib::spi_bus_bit_banged_sclk_mosi_miso(          //creates a SPI object
      sclk, mosi, miso );

   uint8_t address[5] = { 0x00, 0x00, 0x00, 0x00, 0x01 };            //the address on which we are going to send data

   auto nrf = NRF24( spi_bus, ce, csn );                             //creates a object of nrf24l01+
   nrf.start();                                                      //sets default values into the register of the rf chip
   nrf.write_pipe( address );                                        //sets the pipe, address and payload size we are going to use 
   nrf.powerDown_rx();                                               //powers up TX mode

   //A bigger value than our payload (which is 5 bytes and not 18 bytes)
   uint8_t value[18] = { 'H', 'a', 'l', 'l', 'o', ' ', 'i', 'k', ' ', 'b', 'e', 'n', ' ', 'M', 'e', 'n', 'n', 'o' };
   uint8_t len = 18;                       //the length of our value in bytes
   
   for(;;){                                //infinity loop
      hwlib::wait_ms( 200 );               //wait 200 miliseconds
      
      nrf.writeLong( value, len );         //transmits a longer value than the payload size is
   }
}
