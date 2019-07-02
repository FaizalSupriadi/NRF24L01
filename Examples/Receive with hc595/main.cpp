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



   auto ds = target::pin_out( target::pins::d22 );
   auto shcp = target::pin_out( target::pins::d23 );
   auto stcp = target::pin_out( target::pins::d24 );
   
   auto spi_bus1 = hwlib::spi_bus_bit_banged_sclk_mosi_miso(
      shcp, ds, hwlib::pin_in_dummy );
   auto leds = hwlib::hc595( spi_bus1, stcp );

   leds.write( 0 );       //sets all the leds to 0
   leds.flush();


   uint8_t address[5] = { 0x00, 0x00, 0x00, 0x00, 0x01 };     //The 5 byte long address we are going to use    
  
   auto nrf = NRF24( spi_bus, ce, csn );
   nrf.start();                                             //sets the default values in the register                    
   nrf.setPayloadSize( 8 );                                 //makes the payload 8 bytes long (must be done before write_pipe)
   nrf.read_pipe( address );                                //writes the address to the register and sets the payload of the FIFO
   nrf.powerUp_rx();                                        //starts the TX mode

   uint8_t value[8] = {};                //The value we are going to send
   uint8_t len = 8;                      //The length of the value we are going to send in bytes
   uint8_t output = 0;                   //saves where we are on the 8 leds
   
   for(;;){
      
      if( nrf.checkRXfifo() ){                                     //checks if there is data in the RX FIFO      
         nrf.read( value, len );                                   //reads the data into value

         for(int i = 0; i < len; i++){                             //loops through the bytes
            hwlib::cout << (char)value[i] << hwlib::flush;         //displays the data on the screen
            output = output | (0x01 << value[i]);                  //makes from mutiple bytes 1 byte (because all the values are either 0 or 1)
         }

         leds.write( output );                    //writes the 0 and 1 values in one byte to the hc595 chip
         leds.flush();
         hwlib::wait_ms( 100 );                   //waits 100 miliseconds before setting next data

         nrf.flush_rx();                          //flushes the RX FIFO in case there has been send to much data at a time
         
      }
   }
}
