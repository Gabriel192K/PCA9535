#ifndef __PCA9535_H__
#define __PCA9535_H__

/* Dependencies */
#include "../TWI/TWI.h"
#include "PCA9535 Mnemonics.h"

/* Macros */
#ifndef INPUT
#define INPUT        (const uint8_t)0
#endif
#ifndef OUTPUT
#define OUTPUT       (const uint8_t)1
#endif
#ifndef LOW
#define LOW          (const uint8_t)0
#endif
#ifndef HIGH
#define HIGH         (const uint8_t)1
#endif
#ifndef TOGGLE
#define TOGGLE       (const uint8_t)2
#endif

class PCA9535
{
    public:
        PCA9535(__TWI__* twi, const uint8_t address);
        ~PCA9535();
        const uint8_t  begin      (void);
        const uint8_t  isConnected(void);
        const uint8_t  setMode    (const uint16_t mode);
        const uint8_t  setMode    (const uint8_t bit, const uint8_t mode);
        const uint16_t read       (void);
        const uint8_t  read       (const uint8_t bit);
        const uint8_t  write      (const uint16_t state);
        const uint8_t  write      (const uint8_t bit, const uint8_t state);
        const uint8_t  end        (void);
    private:
    __TWI__* twi;
    uint8_t address;
    uint8_t began;
    uint16_t modeBuffer;
    uint16_t readBuffer;
    uint16_t writeBuffer;
};

#endif
