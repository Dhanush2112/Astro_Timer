#include <stdio.h>
#include <assert.h>
#include "Astro/Astro.h"
#include "Channels/Channels.h"
#include "Config/Config.h"

int test_channels(void) {
    Channels_Init();
    Channels_SetState(0, true);
    assert(Channels_GetState(0) == true);
    Channels_SetState(0, false);
    assert(Channels_GetState(0) == false);
    return 0;
}

int test_config(void) {
    SystemConfig cfg;
    Config_Load(&cfg);
    // Defaults are zeros in placeholder implementation
    assert(cfg.latitude == 0.0);
    assert(cfg.longitude == 0.0);
    return 0;
}

int test_astro(void) {
    Astro_Init();
    Astro_UpdateForDate(2026, 4, 9);
    int sr = Astro_GetSunriseMinutes();
    int ss = Astro_GetSunsetMinutes();
    // Based on placeholder values in Astro.c: 360 and 1080
    assert(sr == 360);
    assert(ss == 1080);
    return 0;
}

int main(void) {
    printf("Running unit tests...\n");
    test_channels();
    printf("test_channels passed\n");
    test_config();
    printf("test_config passed\n");
    test_astro();
    printf("test_astro passed\n");
    printf("All unit tests passed\n");
    return 0;
}
