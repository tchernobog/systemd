/* SPDX-License-Identifier: LGPL-2.1+ */
#pragma once

/***
  This file is part of systemd.

  Copyright 2010 Lennart Poettering
***/

#include "unit.h"

typedef struct Device Device;

/* A mask specifying where we have seen the device currently. This is a bitmask because the device might show up
 * asynchronously from each other at various places. For example, in very common case a device might already be mounted
 * before udev finished probing it (think: a script setting up a loopback block device, formatting it and mounting it
 * in quick succession). Hence we need to track precisely where it is already visible and where not. */
typedef enum DeviceFound {
        DEVICE_NOT_FOUND   = 0,
        DEVICE_FOUND_UDEV  = 1U << 1, /* The device has shown up in the udev database */
        DEVICE_FOUND_MOUNT = 1U << 2, /* The device has shown up in /proc/self/mountinfo */
        DEVICE_FOUND_SWAP  = 1U << 3, /* The device has shown up in /proc/swaps */
        DEVICE_FOUND_MASK  = DEVICE_FOUND_UDEV|DEVICE_FOUND_MOUNT|DEVICE_FOUND_SWAP,
} DeviceFound;

struct Device {
        Unit meta;

        char *sysfs;

        /* In order to be able to distinguish dependencies on different device nodes we might end up creating multiple
         * devices for the same sysfs path. We chain them up here. */
        LIST_FIELDS(struct Device, same_sysfs);

        DeviceState state, deserialized_state;
        DeviceFound found, deserialized_found, enumerated_found;

        bool bind_mounts;
};

extern const UnitVTable device_vtable;

void device_found_node(Manager *m, const char *node, DeviceFound found, DeviceFound mask);
bool device_shall_be_bound_by(Unit *device, Unit *u);

DEFINE_CAST(DEVICE, Device);
