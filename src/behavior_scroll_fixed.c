// SPDX-License-Identifier: MIT
// Copyright (c) 2025 User
//
// Custom ZMK behavior: Fixed-amount scroll (one-shot)
// Sends a single HID scroll report with a configured value on key press.
// Does not repeat while held.

#define DT_DRV_COMPAT zmk_behavior_scroll_fixed

#include <zephyr/device.h>

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

#include <zephyr/logging/log.h>
#include <drivers/behavior.h>
#include <zmk/behavior.h>
#if IS_ENABLED(CONFIG_ZMK_POINTING) || IS_ENABLED(CONFIG_ZMK_MOUSE)
#include <zmk/hid.h>
#include <zmk/endpoints.h>
#endif

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

struct behavior_scroll_fixed_config {
    int16_t scroll_x;
    int16_t scroll_y;
};

static int scroll_fixed_binding_pressed(struct zmk_behavior_binding *binding,
                                         struct zmk_behavior_binding_event event) {
    const struct device *dev = device_get_binding(binding->behavior_dev);

    if (!dev) {
        LOG_ERR("scroll_fixed: device not found: %s", binding->behavior_dev);
        return -ENODEV;
    }

    const struct behavior_scroll_fixed_config *cfg = dev->config;

    LOG_DBG("scroll_fixed pressed: x=%d, y=%d", cfg->scroll_x, cfg->scroll_y);

#if IS_ENABLED(CONFIG_ZMK_POINTING) || IS_ENABLED(CONFIG_ZMK_MOUSE)
    // 1. Set scroll values in the HID mouse report
    zmk_hid_mouse_scroll_set(cfg->scroll_x, cfg->scroll_y);

    // 2. Send the mouse report to the host
    zmk_endpoints_send_mouse_report();

    // 3. Clear scroll values to prevent them from being sent again
    //    in subsequent reports triggered by other input events
    zmk_hid_mouse_scroll_set(0, 0);
    zmk_endpoints_send_mouse_report();
#else
    LOG_WRN("scroll_fixed: pointing not enabled on this build");
#endif

    return ZMK_BEHAVIOR_OPAQUE;
}

static int scroll_fixed_binding_released(struct zmk_behavior_binding *binding,
                                          struct zmk_behavior_binding_event event) {
    // One-shot behavior: nothing to do on release
    return ZMK_BEHAVIOR_OPAQUE;
}

static int behavior_scroll_fixed_init(const struct device *dev) { return 0; }

static const struct behavior_driver_api scroll_fixed_driver_api = {
    .binding_pressed = scroll_fixed_binding_pressed,
    .binding_released = scroll_fixed_binding_released,
};

#define SCROLL_FIXED_INST(n)                                                                       \
    static const struct behavior_scroll_fixed_config scroll_fixed_config_##n = {                   \
        .scroll_x = DT_INST_PROP_OR(n, scroll_x, 0),                                              \
        .scroll_y = DT_INST_PROP_OR(n, scroll_y, 0),                                              \
    };                                                                                             \
    BEHAVIOR_DT_INST_DEFINE(n, behavior_scroll_fixed_init, NULL, NULL,                             \
                            &scroll_fixed_config_##n, POST_KERNEL,                                 \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &scroll_fixed_driver_api);

DT_INST_FOREACH_STATUS_OKAY(SCROLL_FIXED_INST)

#endif /* DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT) */

