/*
 * QEMU VLYNQ Serial Interface support.
 *
 * Copyright (C) 2009-2012 Stefan Weil
 *
 * Portions of the code are copies from ssi.c.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) version 3 or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "qemu/osdep.h"
#include "vlynq.h"

#if 0 // TODO: Fix code for QOM API.

struct _VLYNQBus {
    BusState qbus;
};

static struct BusInfo vlynq_bus_info = {
    .name = "VLYNQ",
    .size = sizeof(VLYNQBus),
};

static int vlynq_qdev_init(DeviceState *qdev, DeviceInfo *base)
{
    VLYNQDevice *vlynq_dev = (VLYNQDevice *)qdev;
    VLYNQDeviceInfo *info = container_of(base, VLYNQDeviceInfo, qdev);
    //~ int devfn;
    int rc;

    //~ VLYNQBus *bus = FROM_QBUS(VLYNQBus, qdev_get_parent_bus(qdev));
    //~ devfn = pci_dev->devfn;
    //~ vlynq_dev = do_pci_register_device(pci_dev, bus, base->name, devfn,
                                     //~ info->config_read, info->config_write,
                                     //~ info->header_type);
    //~ if (vlynq_dev == NULL) {
        //~ return -1;
    //~ }
    rc = info->init(vlynq_dev);
    if (rc != 0) {
        //~ do_pci_unregister_device(pci_dev);
    }

    return rc;
}

static int vlynq_unregister_device(DeviceState *dev)
{
    // TODO: missing implementation.

    //~ VLYNQDevice *vlynq_dev = DO_UPCAST(VLYNQDevice, qdev, dev);
    //~ VLYNQDeviceInfo *info = DO_UPCAST(VLYNQDeviceInfo, qdev, dev->info);
    int ret = 0;

    //~ if (info->exit) {
        //~ ret = info->exit(vlynq_dev);
    //~ }
    if (ret) {
        return ret;
    }

    //~ pci_unregister_io_regions(pci_dev);
    //~ do_pci_unregister_device(pci_dev);
    return 0;
}

void vlynq_qdev_register(VLYNQDeviceInfo *info)
{
    info->qdev.init = vlynq_qdev_init;
    //~ info->qdev.unplug = vlynq_unplug_device;
    info->qdev.exit = vlynq_unregister_device;
    info->qdev.bus_info = &vlynq_bus_info;
    qdev_register(&info->qdev);
}

#if 0
static int vlynq_slave_init(DeviceState *dev, DeviceInfo *base_info)
{
    VLYNQSlaveInfo *info = container_of(base_info, VLYNQSlaveInfo, qdev);
    VLYNQSlave *s = VLYNQ_SLAVE_FROM_QDEV(dev);
    VLYNQBus *bus;

    bus = FROM_QBUS(VLYNQBus, qdev_get_parent_bus(dev));
    if (QLIST_FIRST(&bus->qbus.children) != dev
        || QLIST_NEXT(dev, sibling) != NULL) {
        hw_error("Too many devices on VLYNQ bus");
    }

    s->info = info;
    return info->init(s);
}

void vlynq_register_slave(VLYNQSlaveInfo *info)
{
    assert(info->qdev.size >= sizeof(VLYNQSlave));
    info->qdev.init = vlynq_slave_init;
    info->qdev.bus_info = &vlynq_bus_info;
    qdev_register(&info->qdev);
}

#endif

DeviceState *vlynq_create_slave(VLYNQBus *bus, const char *name)
{
    DeviceState *dev;
    dev = qdev_create(&bus->qbus, name);
    qdev_init_nofail(dev);
    return dev;
}

VLYNQBus *vlynq_create_bus(DeviceState *parent, const char *name)
{
    BusState *bus;
    bus = qbus_create(&vlynq_bus_info, parent, name);
    return FROM_QBUS(VLYNQBus, bus);
}

#if 0
uint32_t vlynq_transfer(VLYNQBus *bus, uint32_t val)
{
    DeviceState *dev;
    VLYNQSlave *slave;
    dev = QLIST_FIRST(&bus->qbus.children);
    if (!dev) {
        return 0;
    }
    slave = VLYNQ_SLAVE_FROM_QDEV(dev);
    return slave->info->transfer(slave, val);
}
#endif
#endif // Fix code for QOM API.

