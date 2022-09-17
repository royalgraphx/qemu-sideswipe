/*
 * QEMU interface to CFI1 und CFI2 flash emulation.
 *
 * Copyright (c) 2006-2011 Stefan Weil
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "qemu/osdep.h"
#include "hw/hw.h"
#include "block/block.h"
#include "pflash.h"

#ifdef PFLASH_DEBUG
static int traceflag;
#endif

PFlash *pflash_device_register(hwaddr base,
                               const char *name,
                               hwaddr size,
                               BlockBackend *bs, int width,
                               uint16_t flash_manufacturer,
                               uint16_t flash_type,
                               int be)
{
    /* The values for blocksize and nblocks are defaults which must be
       replaced by the correct values based on flash manufacturer and type.
       This is done by the cfi1 and cfi2 emulation code. */
    const uint32_t blocksize = 0x10000;
    const uint32_t nblocks = size / blocksize;
    const uint16_t id2 = 0x33;
    const uint16_t id3 = 0x44;
    PFlash *pf;

#ifdef PFLASH_DEBUG
    if (getenv("DEBUG_FLASH")) {
        traceflag = strtoul(getenv("DEBUG_FLASH"), 0, 0);
    }
    //~ DPRINTF("Logging enabled for FLASH in %s\n", __func__);
#endif

    switch (flash_manufacturer) {
        case MANUFACTURER_AMD:
        case MANUFACTURER_FUJITSU:
        case MANUFACTURER_MACRONIX:
#if MANUFACTURER_AMD != MANUFACTURER_SPANSION
        case MANUFACTURER_SPANSION:
#endif
        case MANUFACTURER_004A:  /* Which manufacturer is this? */
#if 0
            pf = pflash_amd_register(base, qdev, name, size, bs,
                                     blocksize, nblocks, width,
                                     flash_manufacturer, flash_type,
                                     id2, id3, be);
#else
            pf = (PFlash *)pflash_cfi02_register(base, name, size, bs,
                                       blocksize, nblocks, 1, width,
                                       flash_manufacturer, flash_type,
                                       id2, id3, 0, 0, be);
#endif
            break;
        case MANUFACTURER_INTEL:
PFlashCFI01 *pflash_cfi01_register(hwaddr base,
                                   const char *name,
                                   hwaddr size,
                                   BlockBackend *blk,
                                   uint32_t sector_len,
                                   int width,
                                   uint16_t id0, uint16_t id1,
                                   uint16_t id2, uint16_t id3,
                                   int be);
            pf = (PFlash *)pflash_cfi01_register(base, name, size, bs,
                                       blocksize, nblocks, width,
                                       flash_manufacturer, flash_type,
                                       id2, id3, be);
            break;
        default:
            /* TODO: fix new parameters (0) */
            pf = (PFlash *)pflash_cfi02_register(base, name, size, bs,
                                       blocksize, nblocks, 1, width,
                                       flash_manufacturer, flash_type,
                                       id2, id3, 0, 0, be);
    }
    return pf;
}
