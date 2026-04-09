#include <stdio.h>
#include "Astro/Astro.h"
#include "Channels/Channels.h"
#include "Config/Config.h"

int app_main(void) {
    printf("App_Main for Astro_Timer\n");

    Astro_Init();
    int sr = Astro_GetSunriseMinutes();
    int ss = Astro_GetSunsetMinutes();
    printf("(App) Sunrise: %d min, Sunset: %d min\n", sr, ss);

    Channels_Init();
    Channels_SetState(0, false);
    printf("(App) Channel 0 state: %d\n", Channels_GetState(0));

    SystemConfig cfg;
    Config_Load(&cfg);
    printf("(App) Config lat=%.6f lon=%.6f tz=%d offset=%d\n", cfg.latitude, cfg.longitude, cfg.timezone_offset_minutes, cfg.astro_offset_minutes);

    return 0;
}

#ifndef EMBEDDED
/* Standard C entry point for the application variant */
int main(void) {
    return app_main();
}
#endif