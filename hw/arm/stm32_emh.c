/*
 */

#include "qemu/osdep.h" //os dependency is the most important include
#include "hw/boards.h"  //defines the machine creation macros


static void stm32_emh_init(MachineState *machine)
{

}

static void stm32_emh_class_init(ObjectClass *oc, void *data)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "STM32 emh";
    mc->init = stm32_emh_init;
}

static const TypeInfo stm32_emh_type = {
    .name = MACHINE_TYPE_NAME("stm32_emh"),
    .parent = TYPE_MACHINE,
    .class_init = stm32_emh_class_init,
};

static void stm32_emh_machine_init(void)
{
    type_register_static(&stm32_emh_type);
}

type_init(stm32_emh_machine_init)
