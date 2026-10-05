#ifndef DEBOUNCE_HPP
#define DEBOUNCE_HPP

#include <cstdint>
#include <stdbool.h>
#include <stm32f103xb.h>

class Debounce{
public:
    Debounce(GPIO_TypeDef *port = nullptr,
             uint16_t pin_mask = 0,
             uint16_t filter_ms = 50);
    ~Debounce();

    void Update(uint32_t current_tick);
    bool IsPressed() const;
    bool WasReleased() const;

private:
    enum class State : uint8_t { StableHigh,
                                 BouncingDown,
                                 StableLow,
                                 BouncingUp };
private:
    GPIO_TypeDef *gpiox_;
    uint16_t pin_mask_;
    uint16_t filter_ticks_;
    uint32_t last_change_tick_;
    uint32_t system_tick_;
    State state_;
    bool released_flag_;
};

#endif //DEBOUNCE_HPP

/*
typedef struct
{
  __IO uint32_t CRL;
  __IO uint32_t CRH;
  __IO uint32_t IDR;
  __IO uint32_t ODR;
  __IO uint32_t BSRR;
  __IO uint32_t BRR;
  __IO uint32_t LCKR;
} GPIO_TypeDef;
*/