# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

set(CMAKE_C_STANDARD 11)

add_compile_options(-Wall -Werror -fdata-sections -ffunction-sections)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>)

if(PICO_BOARD STREQUAL "pico")

  #main application
  project(picorx)
  pico_sdk_init()
  add_executable(picorx
      picorx.cpp
      rx/rx.cpp
      rx/rx_dsp.cpp
      rx/half_band_filter.cpp
      rx/half_band_filter2.cpp
      ui/gc9a01a.cpp
      ui/ui.cpp
      ui/logo.cpp
      )
  pico_generate_pio_header(picorx ${CMAKE_CURRENT_LIST_DIR}/rx/nco.pio)
  pico_generate_pio_header(picorx ${CMAKE_CURRENT_LIST_DIR}/ui/quadrature_encoder.pio)
  pico_add_extra_outputs(picorx)
  # enable usb output, disable uart output
  pico_enable_stdio_usb(picorx 1)
  pico_enable_stdio_uart(picorx 0)
  target_link_libraries(picorx pico_stdlib pico_multicore hardware_clocks hardware_flash hardware_pio hardware_adc hardware_pwm hardware_dma hardware_i2c hardware_spi)

elseif(PICO_BOARD STREQUAL "pico2")


  if(PICO_PLATFORM STREQUAL "rp2350-riscv")

    #main application
    project(pico2rx-riscv)
    pico_sdk_init()
    add_executable(pico2rx-riscv
        picorx.cpp
        rx/rx.cpp
        rx/rx_dsp.cpp
        rx/half_band_filter.cpp
        rx/half_band_filter2.cpp
        ui/gc9a01a.cpp
        ui/ui.cpp
        ui/logo.cpp
        )
    pico_generate_pio_header(pico2rx-riscv ${CMAKE_CURRENT_LIST_DIR}/rx/nco.pio)
    pico_generate_pio_header(pico2rx-riscv ${CMAKE_CURRENT_LIST_DIR}/ui/quadrature_encoder.pio)
    pico_add_extra_outputs(pico2rx-riscv)
    # enable usb output, disable uart output
    pico_enable_stdio_usb(pico2rx-riscv 1)
    pico_enable_stdio_uart(pico2rx-riscv 0)
    target_link_libraries(pico2rx-riscv pico_stdlib pico_multicore hardware_clocks hardware_flash hardware_pio hardware_adc hardware_pwm hardware_dma hardware_i2c hardware_spi)

  else()

    #main application
    project(pico2rx)
    pico_sdk_init()
    add_executable(pico2rx
        picorx.cpp
        rx/rx.cpp
        rx/rx_dsp.cpp
        rx/half_band_filter.cpp
        rx/half_band_filter2.cpp
        ui/gc9a01a.cpp
        ui/ui.cpp
        ui/logo.cpp
        )
    pico_generate_pio_header(pico2rx ${CMAKE_CURRENT_LIST_DIR}/rx/nco.pio)
    pico_generate_pio_header(pico2rx ${CMAKE_CURRENT_LIST_DIR}/ui/quadrature_encoder.pio)
    pico_add_extra_outputs(pico2rx)
    # enable usb output, disable uart output
    pico_enable_stdio_usb(pico2rx 1)
    pico_enable_stdio_uart(pico2rx 0)
    target_link_libraries(pico2rx pico_stdlib pico_multicore hardware_clocks hardware_flash hardware_pio hardware_adc hardware_pwm hardware_dma hardware_i2c hardware_spi)

  endif()

endif()
