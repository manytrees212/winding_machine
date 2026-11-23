#ifndef FLASH_DATA_CONTROLLER_HPP
#define FLASH_DATA_CONTROLLER_HPP

#include "project_config.hpp"

#include <cstdint>
#include <eeprom.h>

class FlashDataController final{
public:
    FlashDataController() = delete;
    FlashDataController(ProjectConfig* config, const std::uint16_t (&VirtAddrVarTab)[NB_OF_VAR]);

    FlashDataController(const FlashDataController&)=delete;
    FlashDataController& operator=(const FlashDataController&)=delete;
    FlashDataController(FlashDataController&&)=delete;
    FlashDataController& operator=(FlashDataController&&)=delete;
public:
    void WriteFlashData();
    void ResetFlashData();
    void ProcessUartCommand(const UartCommand cmd);

    void SetFlashCurrentTurnsScaled(const std::uint32_t turns_number_scaled);
    void SetFlashFeederCounts(const std::uint16_t feeder_counts);
    void SetFlashBaseLayerTurnsScaled(std::uint16_t base_layer_turns);
    void SetKpScaled(const std::uint8_t Kp);
    void SetKiScaled(const std::uint8_t Ki);
    void SetKdScaled(const std::uint8_t Kd);

    std::uint16_t GetScaledFlashWireDiam() const;
    std::uint8_t  GetScale()const;
    std::uint16_t GetFlashTotalTurns()const;
    std::uint32_t GetFlashCurrentTurnsScaled()const;
    std::uint16_t GetFlashBaseLayerTurnsScaled()const;
    std::uint16_t GetFlashFeederCounts()const;
    std::uint8_t  GetKpScaled()const;
    std::uint8_t  GetKiScaled()const;
    std::uint8_t  GetKdScaled()const;
    bool GetPidCoefUpdFlag()const;
    void SetPidCoefUpdFlag();
    void ResetPidCoefUpdFlag();
private:
    static constexpr std::uint8_t SCALE = 100;
    ProjectConfig *const config_;
    const std::uint16_t* const virt_addr_var_tab_;
    bool PID_coef_updated_flag{false};
};

#endif //FLASH_DATA_CONTROLLER_HPP
