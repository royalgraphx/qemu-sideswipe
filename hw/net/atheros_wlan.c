/**
 * QEMU WLAN device emulation
 *
 * Copyright (c) 2008 Clemens Kolbitsch
 * Copyright (c) 2008-2020 Stefan Weil
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
 *
 * Modifications:
 *  2008-February-24  Clemens Kolbitsch :
 *                                  New implementation based on ne2000.c
 *
 */

#include "qemu/osdep.h"

#if defined(CONFIG_WIN32)
#warning("not compiled for Windows host")
#else

#include "hw.h"
#include "migration/register.h"  /* register_savevm_live */
#include "pci/pci.h"
#include "pc.h"
#include "net/net.h"

#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <netdb.h>



/*
 * PCI and EEPROM definitions
 */
#include "hw/atheros_wlan.h"
#include "hw/atheros_wlan_io.h"
#include "hw/atheros_wlan_ap.h"
#include "hw/atheros_wlan_eeprom.h"

/*
 * MadWifi OPENHAL atheros constants
 */
#include "hw/ath5k_hw.h"
#include "hw/ath5kreg.h"
#include "hw/ath5k.h"


static void Atheros_WLAN_reset(NICInfo *nd, Atheros_WLANState *s)
{
    DEBUG_PRINT(("reset\n"));

    /*
     * Restore mac address
     */
    memcpy(s->macaddr, nd->macaddr.a, 6);

    /*
     * data from my local AR5212 device
     */
    SET_MEM_L(s->mem, 12, 0);
    SET_MEM_L(s->mem, AR5K_SREV, 86);
    SET_MEM_L(s->mem, AR5K_PCICFG, 0x00010014);
    SET_MEM_L(s->mem, AR5K_PHY_CHIP_ID, 65);
    SET_MEM_L(s->mem, AR5K_SLEEP_CTL, 0x00010000);
    SET_MEM_L(s->mem, 0x9820, 0x02020200);

    Atheros_WLAN_update_irq(s);
}

static void Atheros_WLAN_setup_type(NICInfo *nd, PCIAtheros_WLANState *d)
{
    // create buffer large enough to
    // do all checks
    char *device_name;
    char nd_model[128];
    uint8_t *pci_conf;
    Atheros_WLANState *s;

    device_name = nd_model;
    pci_conf = d->dev.config;
    s = &d->Atheros_WLAN;

    snprintf(nd_model, sizeof(nd_model), "%s", nd->model);


    // skip "atheros_wlan"
    // if it had not been part of nd->model, this
    // module would not be loaded anyways!!
    device_name += 12;
    DEBUG_PRINT_AP(("Loading virtual wlan-pci device...\n"));
    if (strncmp(device_name, "_winxp", 6) == 0) {
        s->device_driver_type = WINXP_DRIVER;
        DEBUG_PRINT_AP((" * Make sure you are using a MS Windows driver!!\n"));

        // skip "_winxp"
        device_name += 6;
    } else if (strncmp(device_name, "_linux", 6) == 0) {
        s->device_driver_type = LINUX_DRIVER;
        DEBUG_PRINT_AP((" * Make sure you are using a MadWifi driver!!\n"));

        // skip "_linux"
        device_name += 6;
    } else {
        s->device_driver_type = LINUX_DRIVER;
        DEBUG_PRINT_AP((" * Unknown driver type '%s'... defaulting to Linux... Make sure you are using a MadWifi driver!!\n", nd->model));
    }

    if (strncmp(device_name, "_HPW400", 7) == 0) {
        s->eeprom_data = (uint32_t *)Atheros_WLAN_eeprom_data_HPW400;
        s->eeprom_size = sizeof(Atheros_WLAN_eeprom_data_HPW400);

        memcpy(pci_conf, Atheros_WLAN_pci_config_HPW400, 256);

        DEBUG_PRINT_AP((" * Using EEPROM and device configuration of HP W400!!\n"));

        // skip "_HPW400"
        device_name += 7;
    } else if (strncmp(device_name, "_MacBook", 8) == 0) {
        s->eeprom_data = (uint32_t *)Atheros_WLAN_eeprom_data_MacBook;
        s->eeprom_size = sizeof(Atheros_WLAN_eeprom_data_MacBook);

        memcpy(pci_conf, Atheros_WLAN_pci_config_MacBook, 256);

        DEBUG_PRINT_AP((" * Using EEPROM and device configuration of Mac Book!!\n"));

        // skip "_MacBook"
        device_name += 8;
    } else if (strncmp(device_name, "_AR5001XPlus", 12) == 0) {
        s->eeprom_data = (uint32_t *)Atheros_WLAN_eeprom_data_HPW400;
        s->eeprom_size = sizeof(Atheros_WLAN_eeprom_data_HPW400);

        memcpy(pci_conf, Atheros_WLAN_pci_config_AR5001XPlus, 256);

        DEBUG_PRINT_AP((" * Using EEPROM and device configuration of AR5001X+ (e.g. Toshiba A100)!!\n"));

        // skip "_AR5001XPlus"
        device_name += 12;
    } else if (strncmp(device_name, "_John", 5) == 0) {
        s->eeprom_data = (uint32_t *)Atheros_WLAN_eeprom_data_HPW400;
        s->eeprom_size = sizeof(Atheros_WLAN_eeprom_data_HPW400);

        memcpy(pci_conf, Atheros_WLAN_pci_config_JOHN, 256);

        DEBUG_PRINT_AP((" * Using EEPROM and device configuration of John!!\n"));

        // skip "_John"
        device_name += 5;
    } else if (strncmp(device_name, "_TPLinkWN651G", 13) == 0) {
        s->eeprom_data = (uint32_t *)Atheros_WLAN_eeprom_data_HPW400;
        s->eeprom_size = sizeof(Atheros_WLAN_eeprom_data_HPW400);

        memcpy(pci_conf, Atheros_WLAN_pci_config_TP_Link_WN651G, 64);

        DEBUG_PRINT_AP((" * Using EEPROM and device configuration of TP-Link WN651G!!\n"));

        // skip "_TPLinkWN651G"
        device_name += 13;
    } else {
        s->eeprom_data = (uint32_t *)Atheros_WLAN_eeprom_data_HPW400;
        s->eeprom_size = sizeof(Atheros_WLAN_eeprom_data_HPW400);

        memcpy(pci_conf, Atheros_WLAN_pci_config_HPW400, 256);

        DEBUG_PRINT_AP((" * Unknown EEPROM type '%s'... defaulting to HP W400!!\n", nd->model));
    }
}

#if 0
static void Atheros_WLAN_save(QEMUFile *f, void *opaque)
{
    int i;
    uint32_t direct_value;
    Atheros_WLANState *s = (Atheros_WLANState *)opaque;

    pci_device_save(&s->pci_dev, f);

    qemu_put_be32s(f, &s->device_driver_type);

    qemu_put_buffer(f, s->ipaddr, 4);
    qemu_put_buffer(f, s->macaddr, 6);

    qemu_put_buffer(f, s->ap_ipaddr, 4);
    qemu_put_buffer(f, s->ap_macaddr, 6);


    qemu_put_be32s(f, &s->interrupt_p_mask);
    for (i = 0; i < 5; qemu_put_be32s(f, &s->interrupt_s_mask[i++])) {
    }
    qemu_put_8s(f, &s->interrupt_enabled);

    qemu_put_be32s(f, &s->current_frequency);

    direct_value = (uint32_t)s->receive_queue_address;
    qemu_put_be32s(f, &direct_value);
    qemu_put_be32s(f, &s->receive_queue_count);

    qemu_put_be32s(f, &s->transmit_queue_size);
    for (i = 0; i < 16; i++) {
        qemu_put_8s(f, &s->transmit_queue_enabled[i]);
        direct_value = (uint32_t)s->transmit_queue_address[i];
        qemu_put_be32s(f, &direct_value);
        qemu_put_be32s(f, &s->transmit_queue_processed[i]);
    }

    qemu_put_be32s(f, &s->ap_state);
    qemu_put_be32s(f, &s->inject_sequence_number);

    qemu_put_buffer(f, (uint8_t *)s->mem, Atheros_WLAN_MEM_SIZE);
}

static int Atheros_WLAN_load(QEMUFile *f, void *opaque, int version_id)
{
    int i, ret;
    uint32_t direct_value;
    Atheros_WLANState *s = (Atheros_WLANState *)opaque;

    // everyone has version 3... and the pci
    // stuff should be there as well, I think
    //
    // let's just claim this has been around
    // for quite some time ;-)
    if (version_id != 3) {
        return -EINVAL;
    }

    if (version_id >= 3) {
        ret = pci_device_load(&s->pci_dev, f);
        if (ret < 0) {
            return ret;
        }
    }

    qemu_get_be32s(f, &s->device_driver_type);

    qemu_get_buffer(f, s->ipaddr, 4);
    qemu_get_buffer(f, s->macaddr, 6);

    qemu_get_buffer(f, s->ap_ipaddr, 4);
    qemu_get_buffer(f, s->ap_macaddr, 6);


    qemu_get_be32s(f, &s->interrupt_p_mask);
    for (i = 0; i < 5; qemu_get_be32s(f, &s->interrupt_s_mask[i++])) {
    }
    qemu_get_8s(f, &s->interrupt_enabled);

    qemu_get_be32s(f, &s->current_frequency);
    qemu_get_be32s(f, &direct_value);
    s->receive_queue_address = (hwaddr)direct_value;
    qemu_get_be32s(f, &s->receive_queue_count);

    qemu_get_be32s(f, &s->transmit_queue_size);
    for (i = 0; i < 16; i++) {
        qemu_get_8s(f, &s->transmit_queue_enabled[i]);
        qemu_get_be32s(f, &direct_value);
        s->transmit_queue_address[i] = (hwaddr)direct_value;
        qemu_get_be32s(f, &s->transmit_queue_processed[i]);
    }

    qemu_get_be32s(f, &s->ap_state);
    qemu_get_be32s(f, &s->inject_sequence_number);

    qemu_get_buffer(f, (uint8_t *)s->mem, Atheros_WLAN_MEM_SIZE);


    s->inject_timer_running = 0;
    s->inject_queue_size = 0;
    s->inject_queue = NULL;

    return 0;
}
#endif

static int pci_Atheros_WLAN_init(PCIDevice *pci_dev)
{
    PCIAtheros_WLANState *d = (PCIAtheros_WLANState *)pci_dev;
    Atheros_WLANState *s = &d->Atheros_WLAN;

#if 0
    /*
     * currently, we have to use this mac-address.
     * it is hardcoded in the eeprom/io-stuff
     */
    nd->macaddr[0] = 0x00;
    nd->macaddr[1] = 0x11;
    nd->macaddr[2] = 0x0a;
    nd->macaddr[3] = 0x80;
    nd->macaddr[4] = 0x2e;
    nd->macaddr[5] = 0x9e;
#endif

    // s->irq = 9; /* PCI interrupt */
    s->irq = d->dev.irq[0];
    s->pending_interrupts = NULL;
    qemu_format_nic_info_str(qemu_get_queue(s->nic), s->macaddr);

#define nd 0 // TODO: hack to allow compilation, fix it
    Atheros_WLAN_setup_type(nd, d);
    Atheros_WLAN_setup_io(d);
    Atheros_WLAN_setup_ap(nd, d);

    /* TODO: we don't support multiple instance yet!! */
    /* TODO: replace NULL by &dev->qdev. */
#if 0
    register_savevm_live("Atheros_WLAN", 0, 3, Atheros_WLAN_save, Atheros_WLAN_load, s);
#endif

    Atheros_WLAN_reset(nd, s);
    return 0;
}

static void Atheros_WLAN_exit(PCIDevice *pci_dev)
{
    Atheros_WLANState *s = DO_UPCAST(Atheros_WLANState, dev, pci_dev);
    memory_region_destroy(&s->mmio_bar);
    qemu_del_nic(s->nic);
}

static void atheros_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    PCIDeviceClass *k = PCI_DEVICE_CLASS(klass);

    dc->desc = "Atheros WLAN";
    //~ dc->props = atheros_properties;
    //~ dc->reset = qdev_atheros_reset;
    //~ dc->vmsd = &vmstate_atheros;
    //~ k->vendor_id = PCI_VENDOR_ID_INTEL;
    k->class_id = PCI_CLASS_NETWORK_ETHERNET;
    //~ k->romfile = "pxe-atheros.rom";
    k->init = pci_Atheros_WLAN_init;
    k->exit = Atheros_WLAN_exit;
    //~ k->device_id = info->device_id;
    //~ k->revision = 0x01;
    //~ k->subsystem_vendor_id = info->subsystem_vendor_id;
    //~ k->subsystem_id = info->subsystem_id;
}

static const TypeInfo atheros_info = {
    .name = "Atheros_WLAN",
    .parent = TYPE_PCI_DEVICE,
    .instance_size = sizeof(PCIAtheros_WLANState),
    .class_init = atheros_class_init,
};

static void Atheros_WLAN_register_types(void)
{
    type_register_static(&atheros_info);
}

type_init(Atheros_WLAN_register_types)

#endif /* CONFIG_WIN32 */
