#include "STM32NUCLEO_F4x1RE.h"

using namespace codal;

void STM32NUCLEO_F4x1RE_dmesg_flush();

STM32NUCLEO_F4x1RE* codal::default_device_instance = nullptr;

/**
 * Constructor.
 *
 * Create a representation of a device, which includes member variables
 * that represent various device drivers used to control aspects of the STM32 IOT node.
 */
STM32NUCLEO_F4x1RE::STM32NUCLEO_F4x1RE() : lowLevelTimer(TIM5, TIM5_IRQn), timer(lowLevelTimer)
// buttonUSER(io.btnUser, DEVICE_ID_BUTTON_A, DEVICE_BUTTON_ALL_EVENTS, ACTIVE_LOW)
{
    // Clear our status
    status                  = 0;
    default_device_instance = this;
    // this->init();
}

/**
 * Post constructor initialisation method.
 *
 * This call will initialised the scheduler, memory allocator and Bluetooth stack.
 *
 * This is required as the Bluetooth stack can't be brought up in a
 * static context i.e. in a constructor.
 *
 * @code
 * stm32NUCLEO_F4x1RE.init();
 * @endcode
 *
 * @note This method must be called before user code utilises any functionality
 *       contained within the STM32NUCLEO_F4x1RE class.
 */
int STM32NUCLEO_F4x1RE::init()
{
    if ((status & DEVICE_INITIALIZED) != 0) {
        return DEVICE_NOT_SUPPORTED;
    }

    status |= DEVICE_INITIALIZED;

    // codal_dmesg_set_flush_fn(STM32NUCLEO_F4x1RE_dmesg_flush);

    // Bring up fiber scheduler.
    scheduler_init(messageBus);

    for (int i = 0; i < DEVICE_COMPONENT_COUNT; i++) {
        if (CodalComponent::components[i] != nullptr) {
            CodalComponent::components[i]->init();
        }
    }

    status |= DEVICE_COMPONENT_STATUS_IDLE_TICK;

    return DEVICE_OK;
}

WEAK int __io_putchar(int ch);

void STM32NUCLEO_F4x1RE_dmesg_flush()
{
#if CONFIG_ENABLED(DMESG_SERIAL_DEBUG)
#if DEVICE_DMESG_BUFFER_SIZE > 0
    if (codalLogStore.ptr > 0 && default_device_instance != nullptr) {
        for (uint32_t i = 0; i < codalLogStore.ptr; i++) {
            __io_putchar(codalLogStore.buffer[i]);
        }
        codalLogStore.ptr = 0;
    }
#endif
#endif
}
