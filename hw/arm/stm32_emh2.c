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


#include "hw/char/pl011.h"
#include "sysemu/sysemu.h"

static struct arm_boot_info versatile_binfo;

static void stm32_emh_init(MachineState *machine)
{
	DeviceState * dev, *sysctl;

    printf("!!!! %s\n",  machine->cpu_type);
    printf("!!!! %s\n",  machine->kernel_filename);
    printf("!!!! %s\n",  machine->kernel_cmdline);
    printf("!!!! %s\n",  machine->initrd_filename);
    printf("!!!! %ld\n", machine->ram_size);

printf("create cpu\n");
    //create cpu
    Object *cpuobj = object_new(machine->cpu_type);
printf("create cpu 1\n");
    object_property_set_bool(cpuobj, true, "realized", &error_fatal);
printf("create cpu 2\n");
    ARMCPU *cpu = ARM_CPU(cpuobj);
printf("create cpu 3\n");

printf("create arm\n");
    //create ram
    MemoryRegion *address_space_mem = get_system_memory();
    MemoryRegion *ram = g_new(MemoryRegion, 1);
    memory_region_allocate_system_memory(ram, NULL, "emh.ram", machine->ram_size);
    memory_region_add_subregion(address_space_mem, 0, ram);
    //

printf("create sysctl\n");
    sysctl = qdev_create(NULL, "emh_sysctl");
    qdev_init_nofail(sysctl);


printf("create uart\n");
    dev = sysbus_create_varargs("pl190", 0x10140000,
                                qdev_get_gpio_in(DEVICE(cpu), ARM_CPU_IRQ),
                                qdev_get_gpio_in(DEVICE(cpu), ARM_CPU_FIQ),
                                NULL);

    qemu_irq pic =  qdev_get_gpio_in(dev, 0);;
    pl011_create(0x101f1000, pic, serial_hd(0));

printf("start\n");
    versatile_binfo.ram_size = machine->ram_size;
    arm_load_kernel(cpu, machine, &versatile_binfo);

}

static void stm32_emh_class_init(ObjectClass *oc, void *data) {
    MachineClass *mc = MACHINE_CLASS(oc);
    mc->desc = "STM32 emh";
    mc->init = stm32_emh_init;
    mc->default_cpu_type = ARM_CPU_TYPE_NAME("cortex-m4");
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
