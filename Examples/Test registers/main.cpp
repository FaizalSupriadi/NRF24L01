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

   auto spi_bus = hwlib::spi_bus_bit_banged_sclk_mosi_miso(         //creates a SPI object
      sclk, mosi, miso );

   uint8_t address[5] = { 0x00, 0x00, 0x00, 0x00, 0x01 };           //the address on which we are going to listen

   auto nrf = NRF24( spi_bus, ce, csn );                            //creates a object of the nrf24l01+
   
   nrf.setChannel( 100 );                     //sets the channel to 100
   nrf.setRetries( 5, 15 );                   //sets the delay to 5 and the amount of retries to 15
   nrf.setDataRate( 1 );                      //sets the data rate to 1Mbps
   nrf.setAddressWidth( 5 );                  //sets the address width to 5 bytes
   nrf.setOutputPower( 1 );                   //sets the output power to minimum
   nrf.setPayloadSize( 5 );                   //sets the payload size to 5 bytes
   
   nrf.readAllRegisters();                                                                  //reads almost all registers
   hwlib::cout << "payload_size: " << nrf.getPayloadSize() << "\n" << hwlib::flush();       //displays the payload_size value
   hwlib::cout << "addr_width:: " << nrf.getAddressWidth() << "\n" << hwlib::flush();       //displays the addr_width value
   
}
