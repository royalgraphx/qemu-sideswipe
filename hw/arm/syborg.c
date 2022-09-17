/*
 * Syborg (Symbian Virtual Platform) reference board
 *
 * Copyright (c) 2009 CodeSourcery
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "qemu/osdep.h"
#include "cpu.h"
#include "hw/sysbus.h"
#include "hw/boards.h"
#include "hw/arm/boot.h"
#include "exec/address-spaces.h"
#include "net/net.h"

static struct arm_boot_info syborg_binfo;

static void syborg_init(MachineState *machine)
{
    ARMCPU *cpu;
    MemoryRegion *sysmem = get_system_memory();
    MemoryRegion *ram = g_new(MemoryRegion, 1);
    qemu_irq pic[64];
    DeviceState *dev;
    int i;

    if (!machine->cpu_model) {
        machine->cpu_model = "cortex-a8";
    }
    cpu = ARM_CPU(cpu_create(machine->cpu_type));

    /* RAM at address zero. */
    memory_region_allocate_system_memory(ram, NULL, "syborg.ram", machine->ram_size);
    memory_region_add_subregion(sysmem, 0, ram);

    dev = sysbus_create_simple("syborg,interrupt", 0xC0000000,
                               qdev_get_gpio_in(DEVICE(cpu), ARM_CPU_IRQ));
    for (i = 0; i < 64; i++) {
        pic[i] = qdev_get_gpio_in(dev, i);
    }

    sysbus_create_simple("syborg,rtc", 0xC0001000, NULL);

    dev = qdev_create(NULL, "syborg,timer");
    qdev_prop_set_uint32(dev, "frequency", 1000000);
    qdev_init_nofail(dev);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, 0xC0002000);
    sysbus_connect_irq(SYS_BUS_DEVICE(dev), 0, pic[1]);

    sysbus_create_simple("syborg,keyboard", 0xC0003000, pic[2]);
    sysbus_create_simple("syborg,pointer", 0xC0004000, pic[3]);
    sysbus_create_simple("syborg,framebuffer", 0xC0005000, pic[4]);
    sysbus_create_simple("syborg,serial", 0xC0006000, pic[5]);
    sysbus_create_simple("syborg,serial", 0xC0007000, pic[6]);
    sysbus_create_simple("syborg,serial", 0xC0008000, pic[7]);
    sysbus_create_simple("syborg,serial", 0xC0009000, pic[8]);

    if (nd_table[0].used || nd_table[0].netdev) {
        DeviceState *dev;
        SysBusDevice *s;

        qemu_check_nic_model(&nd_table[0], "virtio");
        dev = qdev_create(NULL, "syborg,virtio-net");
        qdev_set_nic_properties(dev, &nd_table[0]);
        qdev_init_nofail(dev);
        s = SYS_BUS_DEVICE(dev);
        sysbus_mmio_map(s, 0, 0xc000c000);
        sysbus_connect_irq(s, 0, pic[9]);
    }

    syborg_binfo.ram_size = machine->ram_size;
    syborg_binfo.kernel_filename = machine->kernel_filename;
    syborg_binfo.kernel_cmdline = machine->kernel_cmdline;
    syborg_binfo.initrd_filename = machine->initrd_filename;
    syborg_binfo.board_id = 0;
    arm_load_kernel(cpu, &syborg_binfo);
}

static void syborg_machine_init(MachineClass *mc)
{
    mc->desc = "Syborg (Symbian Virtual Platform)";
    mc->init = syborg_init;
};

DEFINE_MACHINE("syborg", syborg_machine_init)
