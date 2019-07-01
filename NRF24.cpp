#include "NRF24.hpp"
#include "NRF24_register.hpp"
#include "hwlib.hpp"

/************************************************************************************************/

NRF24::NRF24( hwlib::spi_bus & bus, hwlib::pin_out & ce, hwlib::pin_out & csn ):
   bus( bus ), ce( ce ), csn( csn ), payload_size( 5 ), addr_width( 5 )
{} 

/************************************************************************************************/

NRF24::NRF24( hwlib::spi_bus & bus, hwlib::pin_out & ce, hwlib::pin_out & csn, uint8_t payload_size, uint8_t addr_width ):
   bus( bus ), ce( ce ), csn( csn ), payload_size( payload_size ), addr_width( addr_width )
{}

/************************************************************************************************/

void NRF24::transfer( uint8_t reg ){

   set_csn( 0 );                                               //begin the transaction
   bus.transaction( hwlib::pin_out_dummy ).write( reg );       //write the register
   set_csn( 1 );                                               //stop transaction
}

/************************************************************************************************/

uint8_t NRF24::read_byte(){

   set_csn( 0 );                                                               //begin transaction
   uint8_t value = bus.transaction( hwlib::pin_out_dummy ).read_byte();        //reads a byte into value
   set_csn( 1 );                                                               //stop transaction

   return value;                       
}

/************************************************************************************************/
	
uint8_t NRF24::read_register( uint8_t reg ){

   set_csn( 0 );                                                                    //begin transaction
   bus.transaction( hwlib::pin_out_dummy ).write( R_REGISTER | ( 0x1F & reg ) );    //uses the read command to read specific registers
   uint8_t result = bus.transaction( hwlib::pin_out_dummy ).read_byte();            //reads the byte that is send back
   set_csn( 1 );                                                                    //stop transaction

   return result;
}

/************************************************************************************************/

void NRF24::read_register( uint8_t reg, uint8_t* value, uint8_t len){

   set_csn( 0 );                                                                      //starts the transaction
   bus.transaction( hwlib::pin_out_dummy ).write( ( R_REGISTER | ( 0x1F & reg ) ) );  //uses the read command to read the value
   
   while( len-- ){
      *value++ = bus.transaction( hwlib::pin_out_dummy ).read_byte();                 //reads multiple bytes and saves it in value
   }

   set_csn( 1 );                                                                      //stops the transaction
}

/************************************************************************************************/
	
void NRF24::write_register( uint8_t reg, uint8_t value ){

   set_csn( 0 );                                                                      //starts the transaction
   bus.transaction( hwlib::pin_out_dummy ).write( ( W_REGISTER | ( 0x1F & reg ) ) );  //uses the write command to write values to certain registers
   bus.transaction( hwlib::pin_out_dummy ).write(value);                              //writes a value of 8 bits to the register
   set_csn( 1 );                                                                      //stops the transaction
}

/************************************************************************************************/

void NRF24::write_register( uint8_t reg, uint8_t* value, uint8_t len ){

   set_csn( 0 );                                                                      //starts the transaction
   bus.transaction( hwlib::pin_out_dummy ).write( ( W_REGISTER | ( 0x1F & reg ) ) );  //uses the write command to write a value to a register

   while( len-- ){
      bus.transaction( hwlib::pin_out_dummy ).write( *value++ );                      //writes multiple bytes to a register
   }

   set_csn( 1 );                                                                      //stops the transaction
}

/************************************************************************************************/
	
void NRF24::setChannel( uint8_t channel ){
   channel = ( channel < 125 ) ? channel : 125;             //checks of the given channel is lower than 125
   write_register( RF_CH, channel );                        //writes a channel on which to communicate
}

/************************************************************************************************/

uint8_t NRF24::getChannel(){
   return read_register( RF_CH );                           //reads the register that saves the channel
}

/************************************************************************************************/

void NRF24::set_csn( bool x ){
   csn.write( x );                          //writes a 0 or 1 to the csn pin
   csn.flush();                             //flushes the pin
}

/************************************************************************************************/

void NRF24::set_ce( bool x ){
   ce.write( x );                           //writes a 0 or 1 to the ce pin
   ce.flush();                              //flushes the pin
}

/************************************************************************************************/

void NRF24::start(){
   set_ce( 0 );                           //Sets it low, because we are not trying to transmit yet
   set_csn( 1 );                          //Sets it high, because the SPI works with active low, and will react when csn is set high and than low

   hwlib::wait_ms( 200 );                 //Wait some time

   write_register( STATUS, 0x0C );        //Sets the STATUS register with specific values

   setRetries( 5, 15 );                   //Sets the retransmit values max, with a delay of 5 micro seconds and it will repeat 15 times unless there is an ack packet send back

   setDataRate( 1 );                      //Sets the data rate to 1Mbps

   setAddressWidth( 5 );                  //Sets the address width to 5 bytes

   setOutputPower( 0 );                   //Sets the output power to minimum

   write_register( FEATURE, 0 );          //Sets the FEATURE register off
   write_register( DYNPD, 0 );            //Sets the dynamic payload off

   write_register( STATUS, ( 1 << RX_DR ) |( 1 << TX_DS ) | ( 1 << MAX_RT ) );

   setChannel( 100 );                     //sets the channel to 100

   flush_rx();                            //flushes rx in case there is still something in it
   flush_tx();                            //flushes tx in case there is still something in it

   powerup();                             //powers up the chip

   write_register( CONFIG, read_register( CONFIG ) & ~( 1 << PRIM_RX ) );             //sets PRIM_RX in the CONFIG register off
}

/************************************************************************************************/

void NRF24::write_pipe( uint8_t *address ){

   write_register( RX_ADDR_P0, address, addr_width );            //needs to be the same as the TX_ADDR in case you use ACK
   write_register( TX_ADDR, address, addr_width );               //on which address you are writing

   write_register( RX_PW_P0, payload_size );                     //writes the size of the payload
}

/************************************************************************************************/

void NRF24::read_pipe( uint8_t *address ){

   write_register( RX_ADDR_P0, address, addr_width );                            //writes the address on which we are going to listen into pipe 0
   write_register( RX_PW_P0, payload_size );                                     //writes the static size of the packet we want to read (needs to be the same as that of the transmitter)
   write_register( EN_RXADDR, read_register( EN_RXADDR ) | ( 1 << ERX_P0 ) );    //Enables pipe 0 
}

/************************************************************************************************/

void NRF24::powerUp_rx(){

   powerup();                                                                    //makes sure the power is on
   set_ce( 0 );                                                                  //sets ce LOW in case it is HIGH
   write_register( CONFIG, read_register( CONFIG ) | ( 1 << PRIM_RX ) );         //writes PRIM_RX HIGH in the CONFIG register to enable rx mode
   write_register( STATUS, ( 1 << RX_DR ) | ( 1 << TX_DS ) | ( 1 << MAX_RT ) );  //writes some values to STATUS that will reset some things
   set_ce( 1 );                                                                  //makes the ce pin HIGH and enables rx mode
   //write_register( RX_ADDR_P0, , addr_width );                                   //restores the pipe address 
}

/************************************************************************************************/

void NRF24::powerDown_rx(){

   set_ce( 0 );

   hwlib::wait_ns( 20000 );  //200 microseconds

   write_register( CONFIG, read_register( CONFIG ) & ~( 1 << PRIM_RX ) );         //Turn PRIM_RX off so TX mode can be enabled if ce is HIGH
   powerup();                                                                     //makes sure that the power is on
   write_register( EN_RXADDR, read_register( EN_RXADDR ) | ( 1 << ERX_P0 ) );     //Enables the receive address on pipe 0
}

/************************************************************************************************/

void NRF24::powerup(){
   write_register( CONFIG, read_register( CONFIG ) | ( 1 << PWR_UP ) );           //writes a 1 to the PWR_UP in the CONFIG register
}

/************************************************************************************************/

void NRF24::powerdown(){
   set_ce( 0 );
   write_register( CONFIG, read_register( CONFIG ) & ~( 1 << PWR_UP ) );          //writes a 0 to the PWR_UP in the CONFIG register
}

/************************************************************************************************/

void NRF24::flush_rx(){
   transfer( FLUSH_RX );                   //uses the flush_rx command to empty the rx FIFO
}

/************************************************************************************************/

void NRF24::flush_tx(){
   transfer( FLUSH_TX );                   //uses the flush_tx command to empty the tx FIFO
}

/************************************************************************************************/

uint8_t NRF24::read_setup(){
   return read_register( RF_SETUP );       //reads the value of the RF_SETUP register
}

/************************************************************************************************/

void NRF24::setDataRate( uint8_t speed ){

   if(speed == 0){         //250Kbps
      write_register( RF_SETUP, ( read_register(RF_SETUP) | ( 1 << RF_DR_LOW  ) ) & ~( 1 << RF_DR_HIGH ) );
   }else if(speed == 1){   //1Mpbs
      write_register( RF_SETUP,  read_register(RF_SETUP) & ~( ( 1 << RF_DR_LOW  )  | ( 1 << RF_DR_HIGH ) ) );
   }else if(speed == 2){   //2Mbps
      write_register( RF_SETUP, ( read_register(RF_SETUP) & ~( 1 << RF_DR_LOW  ) ) | ( 1 << RF_DR_HIGH ) );
   }else{                  //in case or number that does not fit the data rate will be set to 250Kbps
      write_register( RF_SETUP, ( read_register(RF_SETUP) | ( 1 << RF_DR_LOW  ) ) & ~( 1 << RF_DR_HIGH ) );
   }
}

/************************************************************************************************/

hwlib::string<8> NRF24::getDataRate(){

   uint8_t value = ( read_register(RF_SETUP) & 0x28 );      //reads the values of RF_SETUP that change the data rate speed

   //checks to which value the data rate is equal
   if( value == 0x20 ){
      return "250Kbps";
   }else if( value == 0x00 ){
      return "1Mbps";
   }else if( value == 0x10 ){
      return "2Mbps";
   }else{
      return "reserved";
   }
}

/************************************************************************************************/

void NRF24::setOutputPower( uint8_t value ){

   if( value == 0 ){
      write_register( RF_SETUP, read_register( RF_SETUP ) & ~( 1 <<  RF_PWR_HIGH | 1 << RF_PWR_LOW ) );
   }else if( value == 1 ){
      write_register( RF_SETUP, ( read_register( RF_SETUP ) | 1 << RF_PWR_LOW ) & ~( 1 <<  RF_PWR_HIGH ) );
   }else if( value == 2 ){
      write_register( RF_SETUP, ( read_register( RF_SETUP ) |  1 <<  RF_PWR_HIGH ) & ~( 1 << RF_PWR_LOW ) );
   }else if( value == 3 ){
      write_register( RF_SETUP, read_register( RF_SETUP ) | ( 1 <<  RF_PWR_HIGH | 1 << RF_PWR_LOW ) );
   }else{
      write_register( RF_SETUP, read_register( RF_SETUP ) & ~( 1 <<  RF_PWR_HIGH | 1 << RF_PWR_LOW ) );
   }
}

/************************************************************************************************/

hwlib::string<8> NRF24::getOutputPower(){

   uint8_t value = read_register( RF_SETUP ) & 0x06;      //reads the values of RF_SETUP that change the output power 

   //checks what the PA is and gives a string of 8 bits back
   if( value == 0x00 ){
      return "-18dBm";
   }else if( value == 0x02 ){
      return "-12dBm";
   }else if( value == 0x04 ){
      return "-6dBm";
   }else if( value == 0x06 ){
      return "0dBm";
   }else{
      return "???";      
   }
}

/************************************************************************************************/

void NRF24::write( uint8_t* value, uint8_t len ){

   write_payload( value, len );                                                   //writes the payload into the FIFO
   hwlib::wait_ns(20000);                                                         //waits 20 microseconds to send the data
   set_ce( 0 );                                                                   //turns TX mode off
   write_register( STATUS, ( 1 << RX_DR ) | ( 1 << TX_DS ) | ( 1 << MAX_RT ) );
}

/************************************************************************************************/

void NRF24::writeLong( uint8_t* value, uint8_t len ){

   uint8_t tmp[payload_size] = {};                                 //sends a tmp uint8_t array of the length of the payload 
   uint8_t count = 0;                                              //counts where in the array we are working
   uint8_t len1 = len;                                             //temporarly save for len (should be deleted if I find a better way)

   while( len > 0 ){                                               //as long as the byte length is higher than 0 it will stay writing
      
      for(int i = 0; i < payload_size; i++){

         if( i < len ){
            tmp[i] = value[i + (payload_size * count)]; 
         }else{
            tmp[i] = 0;
         }
      }

      //flush_tx();

      write( tmp, payload_size );                                  //writes the bytes and the payload_size and if there are less bytes to write it will write less bytes

      len -= payload_size;                                         //makes the length shorter
      count++;                                                     //adds one to the counter

      if(len > len1){
         break;
      }

      hwlib::wait_ms( 100 );
   }
}

/************************************************************************************************/

void NRF24::read( uint8_t* value, uint8_t len ){

   read_payload( value, len );                                                    //reads the value in the FIFO

   write_register( STATUS, ( 1 << RX_DR ) | ( 1 << MAX_RT ) | ( 1 << TX_DS ) );

   flush_tx();

}

/************************************************************************************************/

void NRF24::write_payload( uint8_t* value, uint8_t len ){

   set_csn( 0 );
   bus.transaction( hwlib::pin_out_dummy ).write( W_TX_PAYLOAD );

   uint8_t empty = len - payload_size;

   while( len-- ){
      bus.transaction( hwlib::pin_out_dummy ).write( *value++ );                  //writes al the bytes to the FIFO
   }

   while( empty-- ){
         bus.transaction( hwlib::pin_out_dummy ).write( 0 );                      //in case the length is shorter than the payload length 
   }

   set_csn( 1 );
   set_ce( 1 );               //start transmitting                                               
}

/************************************************************************************************/

void NRF24::read_payload( uint8_t* value, uint8_t len ){

   set_csn( 0 );
   bus.transaction( hwlib::pin_out_dummy ).write( R_RX_PAYLOAD );                 //starts the reading of the FIFO

   while( len-- ){
      *value++ = bus.transaction( hwlib::pin_out_dummy ).read_byte();             //add the bytes to value
   }

   set_csn( 1 );
}

/************************************************************************************************/

uint8_t NRF24::check_fifo(){
   return read_register( FIFO_STATUS );            //reads the value of the FIFO_STATUS register
}

/************************************************************************************************/

void NRF24::setRetries( uint8_t delay, uint8_t count ){
   write_register( SETUP_RETR, (delay & 0xF) << ARD | (count & 0xF) << ARC );       //writes the amount of retries you want to use and what the delay must be
}

/************************************************************************************************/

void NRF24::getRetries(){

   uint8_t setup = read_register( SETUP_RETR );          //reads whats in the SETUP_RETR register
   uint8_t delay = ( ( setup & 0xF0 ) >> 4 );            //only allows the values of the last 4 bits to exist and places them on the place of the 1st 4 bits
   uint8_t retransmit = setup & 0x0F;                    //only allows the values of the first 4 bits to exist
   
   //Every posible value you can make with the delay (which is 16) adds 250 microseconds so 0 is 250 microseconds and 1111 is 4000 microseconds
   //Retransmits does not have to change because it is the same value
   hwlib::cout << "Delay:         " << hwlib::dec << 250 * ( delay + 1 ) << " microseconds\n" << hwlib::flush;
   hwlib::cout << "Retransmits:   " << retransmit << "\n" << hwlib::flush;
   
}

/************************************************************************************************/

void NRF24::setAddressWidth( uint8_t width ){

   //The -2 I use in writing to the register is because the bits are different from the width
   //if the width is 5 the bits are 3 (11), and it can't be lower than 3 bytes long.
   if( width > 2 && width < 6 ){
      write_register( SETUP_AW, ( width - 2 ) );
      addr_width = width;
   }else{
      addr_width = 5;
      write_register( SETUP_AW, ( addr_width - 2 ) );
   }
}

/************************************************************************************************/

uint8_t NRF24::getAddressWidth(){

   uint8_t aw = read_register( SETUP_AW );         //reads the register where the address width is saved

   //checks if aw is not equal 0 or higher than 3, because those values are illegal
   if(aw != 0 && aw < 4 ){
      return aw + 2;            //returns aw + 2 because, the bit version is different from the real value
   }else{
      return 0;                 //zero indicates that the value that have been read is not legal
   }
}

/************************************************************************************************/

void NRF24::readAllRegisters( void ){

   hwlib::wait_ms( 100 );               //waits some time, because otherwise a part will not be shown

   //first it will display all the registers (expect the ones that have a address)
   hwlib::cout << "CONFIG:        " << hwlib::bin << read_register( CONFIG )       << "\n" << hwlib::flush;
   hwlib::cout << "EN_AA:         " << hwlib::bin << read_register( EN_AA )        << "\n" << hwlib::flush;
   hwlib::cout << "EN_RXADDR:     " << hwlib::bin << read_register( EN_RXADDR )    << "\n" << hwlib::flush;
   hwlib::cout << "SETUP_RETR:    " << hwlib::bin << read_register( SETUP_RETR )   << "\n" << hwlib::flush;
   hwlib::cout << "RF_CH:         " << hwlib::dec << read_register( RF_CH )        << "\n" << hwlib::flush;
   hwlib::cout << "RF_SETUP:      " << hwlib::bin << read_register( RF_SETUP )     << "\n" << hwlib::flush;
   hwlib::cout << "STATUS:        " << hwlib::bin << read_register( STATUS )       << "\n" << hwlib::flush;
   hwlib::cout << "OBSERVE_TX:    " << hwlib::bin << read_register( OBSERVE_TX )   << "\n" << hwlib::flush;
   hwlib::cout << "RPD:           " << hwlib::bin << read_register( RPD )          << "\n" << hwlib::flush;
   hwlib::cout << "FIFO_STATUS:   " << hwlib::bin << read_register( FIFO_STATUS )  << "\n" << hwlib::flush;
   hwlib::cout << "DYNPD:         " << hwlib::bin << read_register( DYNPD )        << "\n" << hwlib::flush;
   hwlib::cout << "FEATURE:       " << hwlib::bin << read_register( FEATURE )      << "\n\n" << hwlib::flush;

   //displays some important specific values out of some registers using other functions
   hwlib::cout << "Output power:  " << getOutputPower() << "\n"   << hwlib::flush;
   hwlib::cout << "Data rate:     " << getDataRate() << "\n"      << hwlib::flush;
   hwlib::cout << "Address width: " << getAddressWidth() << "\n" << hwlib::flush;
   getRetries();
}

bool NRF24::checkRXfifo(){

   if ( ( read_register( FIFO_STATUS ) & ( 1 << RX_EMPTY ) ) == 0 ){
      return 1;                                                               //returns 1 if there is something in the RX fifo
   }else{
      return 0;                                                               //returns 0 if RX fifo is empty
   }
}
