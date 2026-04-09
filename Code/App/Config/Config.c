/* Config.c
 * Placeholder implementation for configuration storage
 */

#include "Config.h"
#include <string.h>

static SystemConfig current_cfg;

void Config_Load(SystemConfig *cfg) {
    if (!cfg) return;
    // default values
    current_cfg.latitude = 0.0;
    current_cfg.longitude = 0.0;
    current_cfg.timezone_offset_minutes = 0;
    current_cfg.astro_offset_minutes = 0;
    memcpy(cfg, &current_cfg, sizeof(SystemConfig));
}

void Config_Save(const SystemConfig *cfg) {
    if (!cfg) return;
    memcpy(&current_cfg, cfg, sizeof(SystemConfig));
}
