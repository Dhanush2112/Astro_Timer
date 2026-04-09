/* Config.h
 * Placeholder for configuration storage and retrieval
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

typedef struct {
    double latitude;
    double longitude;
    int timezone_offset_minutes;
    int astro_offset_minutes; // ±120
} SystemConfig;

void Config_Load(SystemConfig *cfg);
void Config_Save(const SystemConfig *cfg);

#endif /* CONFIG_H */
