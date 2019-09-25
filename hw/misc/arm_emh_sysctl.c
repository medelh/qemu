/*
 * Status and system control registers for ARM RealView/Versatile boards.
 *
 * Copyright (c) 2006-2007 CodeSourcery.
 * Written by Paul Brook
 *
 * This code is licensed under the GPL.
 */

#include "qemu/osdep.h"
#include "hw/irq.h"
#include "hw/qdev-properties.h"
#include "qemu/timer.h"
#include "sysemu/runstate.h"
#include "qemu/bitops.h"
#include "hw/sysbus.h"
#include "migration/vmstate.h"
#include "hw/arm/primecell.h"
#include "qemu/log.h"
#include "qemu/module.h"

#define LOCK_VALUE 0xa05f

#define TYPE_ARM_SYSCTL "emh_sysctl"
#define ARM_SYSCTL(obj) \
    OBJECT_CHECK(arm_sysctl_state, (obj), TYPE_ARM_SYSCTL)

typedef struct {
    SysBusDevice parent_obj;

    uint32_t *db_clock;
    uint32_t *db_voltage;
    uint32_t db_num_clocks;
    uint32_t *db_clock_reset;
} arm_sysctl_state;

static const VMStateDescription vmstate_arm_sysctl = {
    .name = "emh_sysctl",
    .fields = (VMStateField[]) {
        VMSTATE_VARRAY_UINT32(db_clock, arm_sysctl_state, db_num_clocks,
                              4, vmstate_info_uint32, uint32_t),
        VMSTATE_END_OF_LIST()
    }
};

static void arm_sysctl_reset(DeviceState *d)
{
    arm_sysctl_state *s = ARM_SYSCTL(d);

    for (int i = 0; i < s->db_num_clocks; i++) {
        s->db_clock[i] = s->db_clock_reset[i];
    }
}


static void arm_sysctl_init(Object *obj)
{

}

static void arm_sysctl_realize(DeviceState *d, Error **errp)
{
    arm_sysctl_state *s = ARM_SYSCTL(d);
    s->db_clock = g_new0(uint32_t, s->db_num_clocks);
}

static void arm_sysctl_finalize(Object *obj)
{
    arm_sysctl_state *s = ARM_SYSCTL(obj);

    g_free(s->db_voltage);
    g_free(s->db_clock);
    g_free(s->db_clock_reset);
}

static void arm_sysctl_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = arm_sysctl_realize;
    dc->reset = arm_sysctl_reset;
    dc->vmsd = &vmstate_arm_sysctl;
}

static const TypeInfo arm_sysctl_info = {
    .name          = TYPE_ARM_SYSCTL,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(arm_sysctl_state),
    .instance_init = arm_sysctl_init,
    .instance_finalize = arm_sysctl_finalize,
    .class_init    = arm_sysctl_class_init,
};

static void arm_sysctl_register_types(void)
{  printf("emh_sysctl\n");
    type_register_static(&arm_sysctl_info);
}

type_init(arm_sysctl_register_types)
