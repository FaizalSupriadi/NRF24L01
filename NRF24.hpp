#ifndef NRF24_HPP
#define NRF24_HPP

#include "hwlib.hpp"

class NRF24 {
protected:
	uint16_t ce;
	uint16_t csn;

public:
    
    NRF24( uint16_t ce, uint16_t csn);     

};

#endif // NRF24_HPP
