#include "NRF24.hpp"
#include "NRF24_register.hpp"
#include "hwlib.hpp"

NRF24::NRF24( hwlib::spi_bus & bus, hwlib::pin_out & ce, hwlib::pin_out & csn ):
   bus( bus ), ce( ce ), csn( csn ), payload_size( 5 )
{} 

void NRF24::transfer( uint8_t reg ){
   set_csn( 0 );
   bus.transaction( hwlib::pin_out_dummy ).write( reg );
   set_csn( 1 );
}

uint8_t NRF24::read_byte(){
   set_csn( 0 );
   uint8_t value = bus.transaction( hwlib::pin_out_dummy ).read_byte();
   set_csn( 1 );

   return value;
}
	
uint8_t NRF24::read_register( uint8_t reg ){
   set_csn(0);
   bus.transaction( hwlib::pin_out_dummy ).write( R_REGISTER | ( 0x1F & reg ) );
   uint8_t result = bus.transaction( hwlib::pin_out_dummy ).read_byte();
   set_csn(1);

   return result;
}
	
void NRF24::write_register( uint8_t reg, uint8_t value ){
   set_csn( 0 );
   bus.transaction( hwlib::pin_out_dummy ).write( (W_REGISTER | ( 0x1F & reg )) );
   bus.transaction( hwlib::pin_out_dummy ).write(value);
   set_csn( 1 );
}
	
void NRF24::setChannel( uint8_t channel ){
   channel = ( channel < 125 ) ? channel : 125;
   write_register( RF_CH, channel );
}

uint8_t NRF24::getChannel(){
   return read_register( RF_CH );
}

void NRF24::set_csn( bool x ){
   csn.write( x );
   csn.flush();
}

void NRF24::set_ce( bool x ){
   ce.write( x );
   ce.flush();
}

void NRF24::start(){
   set_ce( 0 );
   set_csn( 1 );

   hwlib::wait_ms( 100 );

   setDataRate(1);

   transfer( 0x50 );
   transfer( 0x73 );

   write_register( FEATURE, 0 );
   write_register( DYNPD, 0 );

   write_register( STATUS, ( 1 << RX_DR ) |( 1 << TX_DS ) | ( 1 << MAX_RT ) );

   setChannel( 100 );

   flush_rx();
   flush_tx();

   powerup();

   write_register( CONFIG, read_register( CONFIG ) & ~( 1 << PRIM_RX ) );
}

void NRF24::write_pipe( uint8_t address ){
   write_register( RX_ADDR_P0, address );
   write_register( TX_ADDR, address );

   write_register( RX_PW_P0, payload_size );
}

void NRF24::read_pipe( uint8_t address ){
   write_register( RX_ADDR_P0, address );
   write_register( RX_PW_P0, payload_size );
   write_register( EN_RXADDR, read_register( EN_RXADDR ) | ( 1 << ERX_P0 ) );
}

void NRF24::powerUp_tx(){
   set_ce( 0 );
   powerup();
   write_register( CONFIG, read_register( CONFIG ) & ~( 1 << PRIM_RX ) );
   set_ce( 1 );
   write_register( STATUS, ( 1 << RX_DR ) |( 1 << TX_DS ) | ( 1 << MAX_RT ) );
}

void NRF24::powerUp_rx(){
   set_ce( 0 );
   powerup();
   write_register( CONFIG, read_register( CONFIG ) | ( 1 << PRIM_RX ) );
   write_register( STATUS, ( 1 << RX_DR ) | ( 1 << TX_DS ) | ( 1 << MAX_RT ) );
   write_register( EN_RXADDR, ( read_register( EN_RXADDR ) & ~( 1 << ERX_P0 ) ) );
   set_ce( 1 );
}

void NRF24::powerDown_rx(){
   set_ce( 0 );
   powerdown();
   write_register( CONFIG, read_register( CONFIG ) & ~( 1 << PRIM_RX ) );
   write_register( EN_RXADDR, read_register( EN_RXADDR ) | ( 1 << ERX_P0 ) );    //testing purpose (does stand in other function, but I am not sure what they meant in the datasheets so I try it out)
   powerup();
}

void NRF24::powerup(){
   write_register( CONFIG, read_register( CONFIG ) | ( 1 << PWR_UP ) );
}

void NRF24::powerdown(){
   set_ce( 0 );
   write_register( CONFIG, read_register( CONFIG ) & ~( 1 << PWR_UP ) );
}

void NRF24::flush_rx(){
   bus.transaction( hwlib::pin_out_dummy ).write( FLUSH_RX );
}

void NRF24::flush_tx(){
   bus.transaction( hwlib::pin_out_dummy ).write( FLUSH_TX );
}

uint8_t NRF24::read_setup(){
   return read_register( RF_SETUP );
}

void NRF24::setDataRate( uint8_t speed ){
   if(speed == 0){
      write_register( RF_SETUP, ( read_register(RF_SETUP) | ( 1 << RF_DR_LOW  ) ) & ~( 1 << RF_DR_HIGH ) );
   }else if(speed == 1){
      write_register( RF_SETUP,  read_register(RF_SETUP) & ~( ( 1 << RF_DR_LOW  )  | ( 1 << RF_DR_HIGH ) ) );
   }else if(speed == 2){
      write_register( RF_SETUP, ( read_register(RF_SETUP) & ~( 1 << RF_DR_LOW  ) ) | ( 1 << RF_DR_HIGH ) );
   }else{
      write_register( RF_SETUP, ( read_register(RF_SETUP) | ( 1 << RF_DR_LOW  ) ) & ~( 1 << RF_DR_HIGH ) );
   }
}

hwlib::string<8> NRF24::getDataRate(){
   uint8_t value = ( read_register(RF_SETUP) & 0x28 );

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

hwlib::string<8> NRF24::getOutputPower(){
   uint8_t value = read_register( RF_SETUP ) & 0x06;

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

void NRF24::write( uint8_t value ){
   write_payload( value );
   set_ce( 1 );
   hwlib::wait_ns( 10000 );
   set_ce( 0 );
   write_register( STATUS, ( 1 << RX_DR ) | ( 1 << TX_DS ) | ( 1 << MAX_RT ) );
}

uint8_t NRF24::read(){
   uint8_t result = read_payload();
   write_register( STATUS, ( 1 << RX_DR ) | ( 1 << MAX_RT ) | ( 1 << TX_DS ) );

   return result;
}

void NRF24::write_payload( uint8_t value ){
   set_csn( 0 );
   bus.transaction( hwlib::pin_out_dummy ).write( W_TX_PAYLOAD_NOACK );
   bus.transaction( hwlib::pin_out_dummy ).write( value );
   set_csn( 1 );
}

uint8_t NRF24::read_payload(){
   set_csn( 0 );
   bus.transaction( hwlib::pin_out_dummy ).write( R_RX_PAYLOAD );
   uint8_t result = bus.transaction( hwlib::pin_out_dummy ).read_byte();
   set_csn( 1 );

   return result;
}
