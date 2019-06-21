#include "NRF24.hpp"
#include "NRF24_register.hpp"
#include "hwlib.hpp"

NRF24::NRF24( hwlib::spi_bus & bus, hwlib::pin_out & ce, hwlib::pin_out & csn ):
   bus( bus ), ce( ce ), csn( csn )
{} 
	
uint8_t NRF24::read_register( uint8_t reg){
   set_csn(0);
   bus.transaction( hwlib::pin_out_dummy ).write( R_REGISTER | ( 0x1F & reg ) );
   uint8_t result = bus.transaction( hwlib::pin_out_dummy ).read_byte();
   set_csn(1);
   return result;
}
	
void NRF24::write_register( uint8_t reg, uint8_t value ){
   set_csn(0);
   bus.transaction( hwlib::pin_out_dummy ).write( (W_REGISTER | ( 0x1F & reg )) );
   bus.transaction( hwlib::pin_out_dummy ).write(value);
   set_csn(1);
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

   setDataRate(0);

   write_register( FEATURE, 0 );
   write_register( DYNPD, 0 );

   write_register( STATUS, ( 1 << RX_DR) |( 1 << TX_DS ) | ( 1 << MAX_RT ) );

   flush_rx();
   flush_tx();

   powerup();

   write_register( CONFIG, read_register(CONFIG) & ~( 1 << PRIM_RX ) );
}

void NRF24::write_pipe(uint8_t value){
   write_register( RX_ADDR_P0, value );
   write_register( TX_ADDR, value );
}

void NRF24::read_pipe(){
   uint8_t rx_addr = read_register( RX_ADDR_P0 );
   uint8_t tx_addr = read_register( TX_ADDR );

   hwlib::cout << "rx_addr: " << rx_addr << "\n" << hwlib::flush;
   hwlib::cout << "tx_addr: " << tx_addr << "\n" << hwlib::flush;
}

void NRF24::powerUp_tx(){
   set_ce( 0 );
   powerup();
   write_register( CONFIG, read_register(CONFIG) & ~( 1 << PRIM_RX ) );
   set_ce( 1 );
   write_register( STATUS, ( 1 << RX_DR) |( 1 << TX_DS ) | ( 1 << MAX_RT ) );
}

void NRF24::powerUp_rx(){
   set_ce( 0 );
   powerup();
   write_register( CONFIG, read_register(CONFIG) | ( 1 << PRIM_RX ) );
   write_register( STATUS, ( 1 << RX_DR ) | ( 1 << TX_DS ) | ( 1 << MAX_RT ) );
   set_ce( 1 );
}

void NRF24::powerDown_rx(){
   set_ce( 0 );
   powerdown();
   write_register( CONFIG, read_register(CONFIG) & ~( 1 << PRIM_RX ) );
   powerdown();
}

void NRF24::powerup(){
   write_register( CONFIG, read_register(CONFIG) | ( 1 << PWR_UP ) );
}

void NRF24::powerdown(){
   set_ce( 0 );
   write_register( CONFIG, read_register(CONFIG) & ~( 1 << PWR_UP ) );
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

void NRF24::setDataRate(uint8_t speed){
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
