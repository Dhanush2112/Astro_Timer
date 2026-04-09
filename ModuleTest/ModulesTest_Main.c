#include <stdio.h>
#include "Astro/Astro.h"
#include "Channels/Channels.h"
#include "Config/Config.h"

int modules_test_main(void) {
    printf("ModulesTest_Main for Astro_Timer\n");

    Astro_Init();
    int sr = Astro_GetSunriseMinutes();
    int ss = Astro_GetSunsetMinutes();
    printf("(Test) Sunrise: %d min, Sunset: %d min\n", sr, ss);

    Channels_Init();
    Channels_SetState(0, true);
    printf("(Test) Channel 0 state: %d\n", Channels_GetState(0));

    SystemConfig cfg;
    Config_Load(&cfg);
    printf("(Test) Config lat=%.6f lon=%.6f tz=%d offset=%d\n", cfg.latitude, cfg.longitude, cfg.timezone_offset_minutes, cfg.astro_offset_minutes);

    return 0;
}

#ifndef EMBEDDED
/* Standard C entry point for the test runner */
int main(void) {
    return modules_test_main();
}
#endif