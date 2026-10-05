#ifndef FLASH_DATA_CONTROLLER_HPP
#define FLASH_DATA_CONTROLLER_HPP

#include <cstdint>
#include "project_config.hpp"

class FlashDataController{
public:
    FlashDataController() = delete;
    FlashDataController(ProjectConfig* config, const std::uint16_t *VirtAddrVarTab);
public:
    void WriteFlashData();
    void ResetFlashData();
    void ProcessUartCommand(const UartCommand cmd);

    void SetFlashCurrentTurns(const std::uint16_t turns_number);
    void SetFlashFeederCounts(const std::uint16_t feeder_counts);
    void SetMaxFlashFeederCounts(const std::uint16_t max_feeder_counts);
    void SetKpScaled(const std::uint8_t Kp);
    void SetKiScaled(const std::uint8_t Ki);
    void SetKdScaled(const std::uint8_t Kd);

    std::uint16_t GetScaledFlashWireDiam() const;
    std::uint8_t  GetScaleOfWireDiam()const;
    std::uint16_t GetFlashTotalTurns()const;
    std::uint16_t GetFlashCurrentTurns()const;
    std::uint16_t GetFlashFeederCounts()const;
    std::uint16_t GetMaxFlashFeederCounts()const;
    std::uint16_t GetMaxFlashFeederPosScaled();
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
