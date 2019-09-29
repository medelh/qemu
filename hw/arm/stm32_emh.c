/*
 */

#include "qemu/osdep.h" //os dependency is the most important include
#include "qapi/error.h"
#include "hw/boards.h"  //defines the machine creation macros
#include "hw/sysbus.h"
#include "hw/qdev-properties.h"
#include "target/arm/cpu.h" //defines ARM_CPU_TYPE_NAME
#include "hw/arm/boot.h"	//arm_load_kernel
#include "exec/address-spaces.h" //get_system_memory

#include "hw/intc/armv7m_nvic.h"
#include "hw/loader.h" // reset rom
#include "sysemu/reset.h"

#include "hw/char/pl011.h"
#include "sysemu/sysemu.h"

#include "elf.h"
#include "migration/vmstate.h"

#include "exec/cpu-common.h"

#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
#pragma GCC diagnostic ignored "-Wmissing-prototypes"

#include "hw/arm/armv7m.h"


static struct arm_boot_info boot_binfo;

static void stm32_emh_init(MachineState *machine)
{
	int flash_size = 64;
	int ram_size = 512;
	DeviceState *nvic;

	// create nvic object
    nvic = qdev_create(NULL, "armv7m_nvic");

    //create cpu
    Object *cpuobj = object_new(machine->cpu_type);
    ARMCPU *cpu = ARM_CPU(cpuobj);
    //set cpu env
    cpu->env.nvic = nvic;
    object_property_set_bool(cpuobj, true, "realized", &error_fatal);

    ((NVICState *)nvic)->cpu = cpu;
    qdev_init_nofail(nvic);

    //--------------------- Memory ------------------------------------
printf("create memory\n");
    MemoryRegion *sysmem = get_system_memory();
    MemoryRegion *sram  = g_new(MemoryRegion, 1);
    MemoryRegion *flash = g_new(MemoryRegion, 1);
    MemoryRegion *hack  = g_new(MemoryRegion, 1);

    memory_region_init_ram(flash, NULL, "emh.flash", machine->ram_size, NULL);
    memory_region_set_readonly(flash, true);
    memory_region_add_subregion(sysmem, 0, flash);

    boot_binfo.ram_size = machine->ram_size;
    armv7m_load_kernel(cpu, machine->kernel_filename, machine->ram_size);
    //
}

static void stm32_emh_class_init(ObjectClass *oc, void *data) {
    MachineClass *mc = MACHINE_CLASS(oc);
    mc->desc = "STM32 emh";
    mc->init = stm32_emh_init;
    mc->default_cpu_type = ARM_CPU_TYPE_NAME("cortex-m3");
//    mc->default_cpu_type = ARM_CPU_TYPE_NAME("arm926");
}

static const TypeInfo stm32_emh_type = {
    .name = MACHINE_TYPE_NAME("stm32_emh"),
    .parent = TYPE_MACHINE,
    .class_init = stm32_emh_class_init,
};

static void stm32_emh_machine_init(void) {
    type_register_static(&stm32_emh_type);
}

type_init(stm32_emh_machine_init)
