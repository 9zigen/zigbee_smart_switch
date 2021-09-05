project(ssr_relay C ASM)
cmake_minimum_required(VERSION 3.7)

# specify the cross compiler
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-c++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

set(AS arm-none-eabi-as)
set(AR arm-none-eabi-ar)
set(OBJCOPY arm-none-eabi-objcopy)
set(OBJDUMP arm-none-eabi-objdump)
set(SIZE arm-none-eabi-size)

SET(SDK_ROOT ../../../SDK/nRF5_SDK_for_Thread_and_Zigbee_v4.1.0)

set(CMAKE_OSX_SYSROOT "/")
set(CMAKE_OSX_DEPLOYMENT_TARGET "")

# language standard/version settings
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 98)

list(APPEND USER_SOURCES
    "${SDK_ROOT}/modules/nrfx/mdk/gcc_startup_nrf52840.S"
    "${SDK_ROOT}/components/libraries/log/src/nrf_log_backend_rtt.c"
    "${SDK_ROOT}/components/libraries/log/src/nrf_log_backend_serial.c"
    "${SDK_ROOT}/components/libraries/log/src/nrf_log_backend_uart.c"
    "${SDK_ROOT}/components/libraries/log/src/nrf_log_default_backends.c"
    "${SDK_ROOT}/components/libraries/log/src/nrf_log_frontend.c"
    "${SDK_ROOT}/components/libraries/log/src/nrf_log_str_formatter.c"
    "${SDK_ROOT}/components/boards/boards.c"
    "${SDK_ROOT}/external/zboss/zb_error/zb_error_to_string.c"
    "${SDK_ROOT}/external/zboss/osif/zb_nrf52_common.c"
    "${SDK_ROOT}/external/zboss/osif/zb_nrf52_nrf_logger.c"
    "${SDK_ROOT}/external/zboss/osif/zb_nrf52_nvram.c"
    "${SDK_ROOT}/external/zboss/osif/zb_nrf52_sdk_config_deps.c"
    "${SDK_ROOT}/external/zboss/osif/zb_nrf52_timer.c"
    "${SDK_ROOT}/external/zboss/osif/zb_nrf52_transceiver.c"
    "${SDK_ROOT}/external/zboss/addons/zcl/zb_zcl_common_addons.c"
    "${SDK_ROOT}/external/zboss/addons/zcl/zb_zcl_ota_upgrade_addons.c"
    "${SDK_ROOT}/components/libraries/button/app_button.c"
    "${SDK_ROOT}/components/libraries/util/app_error.c"
    "${SDK_ROOT}/components/libraries/util/app_error_handler_gcc.c"
    "${SDK_ROOT}/components/libraries/gpiote/app_gpiote.c"
    "${SDK_ROOT}/components/libraries/pwm/app_pwm.c"
    "${SDK_ROOT}/components/libraries/scheduler/app_scheduler.c"
    "${SDK_ROOT}/components/libraries/timer/app_timer2.c"
    "${SDK_ROOT}/components/libraries/util/app_util_platform.c"
    "${SDK_ROOT}/components/libraries/assert/assert.c"
    "${SDK_ROOT}/components/libraries/timer/drv_rtc.c"
    "${SDK_ROOT}/components/libraries/util/nrf_assert.c"
    "${SDK_ROOT}/components/libraries/atomic_fifo/nrf_atfifo.c"
    "${SDK_ROOT}/components/libraries/atomic/nrf_atomic.c"
    "${SDK_ROOT}/components/libraries/balloc/nrf_balloc.c"
    "${SDK_ROOT}/external/fprintf/nrf_fprintf.c"
    "${SDK_ROOT}/external/fprintf/nrf_fprintf_format.c"
    "${SDK_ROOT}/components/libraries/fstorage/nrf_fstorage.c"
    "${SDK_ROOT}/components/libraries/fstorage/nrf_fstorage_nvmc.c"
    "${SDK_ROOT}/components/libraries/memobj/nrf_memobj.c"
    "${SDK_ROOT}/components/libraries/pwr_mgmt/nrf_pwr_mgmt.c"
    "${SDK_ROOT}/components/libraries/queue/nrf_queue.c"
    "${SDK_ROOT}/components/libraries/ringbuf/nrf_ringbuf.c"
    "${SDK_ROOT}/components/libraries/experimental_section_vars/nrf_section_iter.c"
    "${SDK_ROOT}/components/libraries/sortlist/nrf_sortlist.c"
    "${SDK_ROOT}/components/libraries/strerror/nrf_strerror.c"
    "${SDK_ROOT}/integration/nrfx/legacy/nrf_drv_clock.c"
    "${SDK_ROOT}/integration/nrfx/legacy/nrf_drv_ppi.c"
    "${SDK_ROOT}/integration/nrfx/legacy/nrf_drv_rng.c"
    "${SDK_ROOT}/integration/nrfx/legacy/nrf_drv_uart.c"
    "${SDK_ROOT}/modules/nrfx/hal/nrf_ecb.c"
    "${SDK_ROOT}/components/drivers_nrf/nrf_soc_nosd/nrf_nvic.c"
    "${SDK_ROOT}/modules/nrfx/hal/nrf_nvmc.c"
    "${SDK_ROOT}/components/drivers_nrf/nrf_soc_nosd/nrf_soc.c"
    "${SDK_ROOT}/modules/nrfx/soc/nrfx_atomic.c"
    "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_clock.c"
    "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_gpiote.c"
    "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_ppi.c"
    "${SDK_ROOT}/modules/nrfx/drivers/src/prs/nrfx_prs.c"
    "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_pwm.c"
    "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_rng.c"
    "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_systick.c"
    "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_timer.c"
    "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_uart.c"
    "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_uarte.c"
    "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_spi.c"
    "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_spim.c"
    "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_twi.c"
    "${SDK_ROOT}/modules/nrfx/drivers/src/nrfx_twim.c"
    "${SDK_ROOT}/components/libraries/bsp/bsp.c"
    "main.c"
    "pwm.c"
    "relay.c"
    "button.c"
    "storage.c"
    "${SDK_ROOT}/modules/nrfx/mdk/system_nrf52840.c"
    "${SDK_ROOT}/components/zigbee/common/zigbee_helpers.c"
    "${SDK_ROOT}/components/zigbee/common/zigbee_logger_eprxzcl.c")

include_directories(
    "${SDK_ROOT}/external/zboss/include/zcl"
    "${SDK_ROOT}/components/libraries/gpiote"
    "${SDK_ROOT}/external/fprintf"
    "${SDK_ROOT}/integration/nrfx/legacy"
    "${SDK_ROOT}/components/libraries/experimental_section_vars"
    "${SDK_ROOT}/external/zboss/osif"
    "${SDK_ROOT}/components/libraries/atomic_fifo"
    "${SDK_ROOT}/external/nRF-IEEE-802.15.4-radio-driver/src"
    "${SDK_ROOT}/external/nRF-IEEE-802.15.4-radio-driver/src/fem/three_pin_gpio"
    "${SDK_ROOT}/components/libraries/delay"
    "${SDK_ROOT}/external/zboss/include"
    "${SDK_ROOT}/components/toolchain/cmsis/include"
    "${SDK_ROOT}/components/libraries/balloc"
    "${SDK_ROOT}/components/libraries/log"
    "${SDK_ROOT}/components/libraries/memobj"
    "${SDK_ROOT}/components/libraries/atomic"
    "${SDK_ROOT}/components"
    "${SDK_ROOT}/modules/nrfx/mdk"
    "${SDK_ROOT}/components/libraries/scheduler"
    "${SDK_ROOT}/components/libraries/strerror"
    "${SDK_ROOT}/integration/nrfx"
    "${SDK_ROOT}/modules/nrfx/drivers/include"
    "${SDK_ROOT}/components/zigbee/common"
    "${SDK_ROOT}/components/libraries/pwm"
    "${SDK_ROOT}/components/libraries/ringbuf"
    "${SDK_ROOT}/external/zboss/include/ha"
    "${SDK_ROOT}/modules/nrfx"
    "${SDK_ROOT}/components/drivers_nrf/nrf_soc_nosd"
    "${SDK_ROOT}/components/libraries/log/src"
    "${SDK_ROOT}/external/zboss/addons"
    "${SDK_ROOT}/external/zboss/include/osif"
    "${SDK_ROOT}/components/libraries/sortlist"
    "${SDK_ROOT}/external/zboss/zb_error"
    "${SDK_ROOT}/modules/nrfx/hal"
    "${SDK_ROOT}/components/libraries/mutex"
    "${SDK_ROOT}/components/libraries/queue"
    "${SDK_ROOT}/components/libraries/pwr_mgmt"
    "${SDK_ROOT}/components/libraries/bsp"
    "${SDK_ROOT}/components/libraries/fstorage"
    "${SDK_ROOT}/components/boards"
    "${SDK_ROOT}/components/libraries/timer"
    "${SDK_ROOT}/components/libraries/button"
    "${SDK_ROOT}/external/nRF-IEEE-802.15.4-radio-driver/src/fem"
    "${SDK_ROOT}/components/libraries/util"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Libraries common to all targets
#list(APPEND USER_SOURCES
#    "${SDK_ROOT}/external/zboss/lib/gcc/libzboss.a"
#    "${SDK_ROOT}/external/zboss/lib/gcc/nrf52840/nrf_radio_driver.a")

# CPU config
include_directories("pca10040/s132/config")
include_directories("${SDK_ROOT}/modules/nrfx/mdk")

add_definitions(-DBLE_STACK_SUPPORT_REQD
    -DAPP_TIMER_V2
    -DAPP_TIMER_V2_RTC1_ENABLED
    -DBL_SETTINGS_ACCESS_ONLY
    -DBOARD_CUSTOM
    -DCONFIG_GPIO_AS_PINRESET
    -DENABLE_FEM
    -DFLOAT_ABI_HARD
    -DNRF52840_XXAA
    -DZB_TRACE_LEVEL=0
    -DZB_TRACE_MASK=0
#    -DZB_ED_ROLE
    -mcpu=cortex-m4
    -mthumb -mabi=aapcs
    -Wall -Werror
    -mfloat-abi=hard -mfpu=fpv4-sp-d16
    -ffunction-sections -fdata-sections -fno-strict-aliasing
    -fno-builtin -fshort-enums -Wno-packed-bitfield-compat
    -DCLION)

# nRF52
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/nrf52840.ld")
set(CPU_FLAGS "-mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -D__HEAP_SIZE=8192 -D__STACK_SIZE=8192")

# Compiler/assambler/linker flags
set(COMMON_FLAGS "${CPU_FLAGS} -mthumb -mabi=aapcs -Wall -Werror -O3 -g3 -ffunction-sections -fdata-sections -fno-strict-aliasing -fno-builtin -fshort-enums")

# compiler/assambler/linker flags
set(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu99")
set(CMAKE_CXX_FLAGS "-O3 -g3")
set(CMAKE_ASM_FLAGS "-g3 -mthumb -mabi=aapcs -T${LINKER_SCRIPT} ${CPU_FLAGS} -Wl,--gc-sections --specs=nano.specs")
set(CMAKE_EXE_LINKER_FLAGS "-mthumb -mabi=aapcs -std=gnu++98 -std=c99 -L ${NRF5_SDK_PATH}/components/toolchain/gcc -T${LINKER_SCRIPT} ${CPU_FLAGS} -Wl,--gc-sections --specs=nano.specs -lc -lnosys -lm")

# note: we must override the default cmake linker flags so that CMAKE_C_FLAGS are not added implicitly
#set(CMAKE_C_LINK_EXECUTABLE "${CMAKE_C_COMPILER} <LINK_FLAGS> <OBJECTS> -o <TARGET>")
#set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_C_COMPILER} <LINK_FLAGS> <OBJECTS> -lstdc++ -o <TARGET>")

#SET(CMAKE_CXX_FLAGS_INIT "${COMMON_FLAGS} -std=c++11")
#SET(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS} -std=gnu99")
#SET(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections -flto -T ${LINKER_SCRIPT}")

#set(CMAKE_CXX_STANDARD 11)
#
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")

# executable
add_executable(${PROJECT_NAME}.elf ${USER_SOURCES} ${LINKER_SCRIPT})

#set_target_properties(${PROJECT_NAME} PROPERTIES ${CMAKE_EXE_LINKER_FLAGS} "-Wl,-Map=${EXECUTABLE_NAME}.map")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")

set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)
set(SOFT_DEVICE ../../components/softdevice/s132/hex/s132_nrf52_7.0.1_softdevice.hex)
set(FIRMWARE ${PROJECT_NAME}_softdevice.hex)
message( ${SOFT_DEVICE} )

add_custom_command(TARGET ${PROJECT_NAME}.elf
    POST_BUILD
    COMMAND ${SIZE} ${PROJECT_NAME}.elf
    COMMAND ${OBJCOPY} -Oihex ${PROJECT_NAME}.elf ${HEX_FILE}
    COMMAND ${OBJCOPY} -Obinary ${PROJECT_NAME}.elf ${BIN_FILE}
    COMMAND mergehex --merge ${SOFT_DEVICE} ${HEX_FILE} --output ${PROJECT_BINARY_DIR}/${FIRMWARE}
    COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}")


add_custom_target(UPLOAD
    arm-none-eabi-gdb -iex "target remote tcp:127.0.0.1:2331"
    -iex "monitor reset"
    -iex "file ${PROJECT_NAME}.elf"
    -iex "load"
    -iex "continue"
    -iex "disconnect" -iex "quit")

#add_custom_target(NRF52 COMMAND make
#    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})