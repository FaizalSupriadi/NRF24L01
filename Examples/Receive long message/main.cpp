#include "hwlib.hpp"
#include "NRF24.hpp"
#include "NRF24_register.hpp"

int main( void ){

   namespace target = hwlib::target;
   
   //defines the pins that will be used for the SPI transactions
   auto sclk = target::pin_out( target::pins::d9 );
   auto mosi = target::pin_out( target::pins::d12 );
   auto miso = target::pin_in( target::pins::d13 );
   auto csn  = target::pin_out( target::pins::d11 );
   auto ce   = target::pin_out( target::pins::d10 );

   hwlib::wait_ms( 100 );

   auto spi_bus = hwlib::spi_bus_bit_banged_sclk_mosi_miso(                  //makes a SPI object
      sclk, mosi, miso );

   uint8_t address[5] = { 0x00, 0x00, 0x00, 0x00, 0x01 };                    //sets the address on which to communicate

   auto nrf = NRF24( spi_bus, ce, csn );          //creates a nrf24l01+ object
   nrf.start();                                   //sets default values into the register
   nrf.read_pipe( address );                      //sets the pipe, address and the payload length
   nrf.powerUp_rx();                              //powers up rx mode

   uint8_t value[5] = {};                         //in this array the data will be received
   uint8_t len = 5;                               //amount of bytes we are going to read (must be same as transmitter)
   
   for(;;){                                                         //starts a loop
      
      if( nrf.checkRXfifo() ){                                       //checks if there is data in the RX FIFO
         nrf.read( value, len );                                     //reads the data out of the FIFO into the value array

         for(int i = 0; i < len; i++){                               //loops through all bytes
            hwlib::cout << (char)value[i] << hwlib::flush;           //displays the bytes
 
            if(i == 4 && value[4] == 0){                             //checks if the last byte is a 0 to get a new line
               hwlib::cout << "\n" << hwlib::flush;
            }
         }

         nrf.flush_rx();                                             //empties the RX FIFO
         
      }
   }
}
