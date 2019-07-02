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

   auto spi_bus = hwlib::spi_bus_bit_banged_sclk_mosi_miso(           //creates a SPI object
      sclk, mosi, miso );

   uint8_t address[5] = { 0x00, 0x00, 0x00, 0x00, 0x01 };             //the address we are going to use to transmit data

   auto nrf = NRF24( spi_bus, ce, csn );                              //creates a nrf24l01+ object
   nrf.start();                                                       //set default values into the register of the rf chip
   nrf.write_pipe( address );                                         //sets the pipe, address and payload size
   nrf.powerDown_rx();                                                //starts tx mode

   uint8_t value[5] = { 1, 2, 3, 4, 5 };                              //the value we are going to transmit
   uint8_t len = 5;                                                   //the length of the value in bytes
   
   for(;;){                                                           //infinity loop
      nrf.write( value, len );                                        //transmitting data
   }
}
