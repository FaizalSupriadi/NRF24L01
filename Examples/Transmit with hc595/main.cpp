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

   auto next = target::pin_in( target::pins::d40 );            //define button for next led         
   auto send = target::pin_in( target::pins::d41 );            //define button to send data
   auto set  = target::pin_in( target::pins::d42 );            //define button to set value in led
   auto back = target::pin_in( target::pins::d43 );            //define value to go back to led

   hwlib::wait_ms( 100 );

   auto spi_bus = hwlib::spi_bus_bit_banged_sclk_mosi_miso( 
      sclk, mosi, miso );



   auto ds   = target::pin_out( target::pins::d46 );
   auto shcp = target::pin_out( target::pins::d47 );
   auto stcp = target::pin_out( target::pins::d48 );
   
   auto spi_bus1 = hwlib::spi_bus_bit_banged_sclk_mosi_miso(
      shcp, ds, hwlib::pin_in_dummy );
   auto leds = hwlib::hc595( spi_bus1, stcp );

   leds.write( 0 );                         //sets all the leds to 0
   leds.flush();

   uint8_t address[5] = { 0x00, 0x00, 0x00, 0x00, 0x01 };     //The 5 byte long address we are going to use

   auto nrf = NRF24( spi_bus, ce, csn );
   nrf.start();                                               //sets the default values in the register  
   nrf.setPayloadSize( 8 );                                   //makes the payload 8 bytes long (must be done before write_pipe)
   nrf.write_pipe( address );                                 //writes the address to the register and sets the payload of the FIFO
   nrf.powerDown_rx();                                        //starts the TX mode

   uint8_t value[8] = {};               //The value we are going to send
   uint8_t len = 8;                     //The length of the value we are going to send in bytes
   uint8_t count = 0;                   //saves where we are on the 8 leds
   uint8_t output = 0;                  //the output for the leds

   for(;;){
      hwlib::wait_ms( 75 );             //wait for a short time, so the buttons won't react to much

      if(set.read() == 0){                                //checks if we pressed the set button
         if(value[count] == 0){                           //checks if the value has been set or not
            value[count] = 1;                             //if the value is not set change it to a 1
            output = output | (0x01 << count);            //add it to the output
         }else{
            value[count] = 0;                             //if the output has been set change it back to 0
            output = output & ~(0x01 << count);           //erases the 1 on that position
         }
      
         leds.write(output);                              //writes the output to the h595 chip
         leds.flush();
      }

      if(back.read() == 0 && count != 0){                 //checks if the go back button has been pressed
         count--;                                         //takes one away from count to go one back
      }

      if(next.read() == 0 && count < 7){                  //checks if the go next button has been pressed
         count++;                                         //goes to the next led
         value[count] = 0;                                //sets the next led to 0 in case it already has been written
      }

      if(send.read() == 0){                               //checks if the send button has been pressed
         if(count != 7){                                  //if the count did not set all the values to a 0 or 1
            for(int i = count+1; i < len; i++){           //sets all next values after the count to a 0
               value[i] = 0;                  
               output = output & ~(0x01 << i);            //erases values that are 1's after the count
            }
         }

         nrf.write( value, len );             //sends the value and the length to the rf chip
         count = 0;                           //reset the count
         output = 0;                          //reset the leds of the hc595
         value[0] = 0;                        //sets the first value in the array to 0, because otherwise it can't be set to a 0
         leds.write( 0 );                     //resets all the leds on the hc595
         leds.flush();
      }
   }
}
