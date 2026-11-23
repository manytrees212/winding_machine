#ifndef UART_CMD_PARSER_HPP
#define UART_CMD_PARSER_HPP

#include <cstdint>
#include "project_config.hpp"

class UartCmdParser final{
public:
    UartCmdParser()=default;
    ~UartCmdParser()=default;

    UartCmdParser(const UartCmdParser&) = delete;
    UartCmdParser& operator=(const UartCmdParser&) = delete;
    UartCmdParser(UartCmdParser&&) = delete;
    UartCmdParser& operator=(UartCmdParser&&) = delete;

    void ParseUartBuffer();
    bool IsCmdReady()const;

    UartCommand GetUartCmd();

private:
    void UpdateCmd();
private:
    std::uint32_t last_time_{0};
    UartCommand uart_cmd_t_ {0,0};
    bool uart_cmd_ready_=false;
    char uart_cmd_buffer_[16]={0};
    std::uint8_t pos_{0};
};

#endif //UART_CMD_PARSER_HPP