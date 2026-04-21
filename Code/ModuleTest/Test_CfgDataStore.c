/* Test_CfgDataStore.c
 * Unit tests for CfgDataStore_ParameterBlock (get/set param, get/set/update block,
 * write-check validation).
 */

#include <unity_fixture.h>

#include "CfgDataStore/ParameterBlock.h"

/* ---- Helper ---- */
static void ReInit(void)
{
  ParameterBlock_DefaultParameters();
}

TEST_GROUP(CfgDataStore);

TEST_SETUP(CfgDataStore)
{
  ParameterBlock_Init();
}

TEST_TEAR_DOWN(CfgDataStore)
{
}

/* =========================================================================
 * Test: default values after Init
 * ========================================================================= */
TEST(CfgDataStore, DefaultValues)
{
  LocationBlock_t loc;
  memset(&loc, 0xFF, sizeof(loc));
  LocalBlock_t lb = {0u, &loc};
  ParameterBlock_GetBlock(BLOCK_LOCATION, &lb);

  TEST_ASSERT_EQUAL_FLOAT(0.0f, loc.latitude);
  TEST_ASSERT_EQUAL_FLOAT(0.0f, loc.longitude);
  TEST_ASSERT_EQUAL_INT16(0, loc.timezone_offset_minutes);
  TEST_ASSERT_EQUAL_INT16(0, loc.astro_offset_minutes);

  ModeBlock_t mb;
  LocalBlock_t lbm = {0u, &mb};
  ParameterBlock_GetBlock(BLOCK_MODE, &lbm);
  TEST_ASSERT_EQUAL_INT(MODE_AUTO, mb.mode);
}

/* =========================================================================
 * Test: SetParam / GetParam round-trip
 * ========================================================================= */
TEST(CfgDataStore, SetAndGetParam)
{
  ReInit();

  float32_t lat = 51.5f;
  WriteError_t err = ParameterBlock_SetParam(PARAM_LATITUDE, &lat, sizeof(lat));
  TEST_ASSERT_EQUAL_INT(WRITE_ERROR_NONE, err);

  float32_t readLat = 0.0f;
  bool ok = ParameterBlock_GetParam(PARAM_LATITUDE, &readLat, sizeof(readLat));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_FLOAT(51.5f, readLat);

  OperatingMode_t m = MODE_MANUAL;
  err = ParameterBlock_SetParam(PARAM_MODE, &m, sizeof(m));
  TEST_ASSERT_EQUAL_INT(WRITE_ERROR_NONE, err);

  OperatingMode_t readMode = MODE_AUTO;
  ok = ParameterBlock_GetParam(PARAM_MODE, &readMode, sizeof(readMode));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_INT(MODE_MANUAL, readMode);
}

/* =========================================================================
 * Test: SetBlock / GetBlock round-trip
 * ========================================================================= */
TEST(CfgDataStore, SetAndGetBlock)
{
  ReInit();

  LocationBlock_t write = {51.5f, -0.12f, 60, 30};
  LocalBlock_t lbw = {0u, &write};
  WriteError_t err = ParameterBlock_SetBlock(BLOCK_LOCATION, &lbw);
  TEST_ASSERT_EQUAL_INT(WRITE_ERROR_NONE, err);

  LocationBlock_t read;
  memset(&read, 0, sizeof(read));
  LocalBlock_t lbr = {0u, &read};
  ParameterBlock_GetBlock(BLOCK_LOCATION, &lbr);

  TEST_ASSERT_EQUAL_FLOAT(51.5f, read.latitude);
  TEST_ASSERT_EQUAL_FLOAT(-0.12f, read.longitude);
  TEST_ASSERT_EQUAL_INT16(60, read.timezone_offset_minutes);
  TEST_ASSERT_EQUAL_INT16(30, read.astro_offset_minutes);
}

/* =========================================================================
 * Test: UpdateBlock fires only when a parameter has changed
 * ========================================================================= */
TEST(CfgDataStore, UpdateBlock)
{
  ReInit();

  ModeBlock_t mb;
  LocalBlock_t lb = {0u, &mb};

  /* First call must return true (initial version 0xEFFFFFFF != 0) */
  TEST_ASSERT_TRUE(ParameterBlock_UpdateBlock(BLOCK_MODE, &lb));

  /* No writes since last update - must return false */
  TEST_ASSERT_FALSE(ParameterBlock_UpdateBlock(BLOCK_MODE, &lb));

  /* Write triggers version increment - UpdateBlock must fire */
  OperatingMode_t m = MODE_HOLIDAY;
  ParameterBlock_SetParam(PARAM_MODE, &m, sizeof(m));

  TEST_ASSERT_TRUE(ParameterBlock_UpdateBlock(BLOCK_MODE, &lb));
  TEST_ASSERT_EQUAL_INT(MODE_HOLIDAY, mb.mode);

  /* No further write - must not fire */
  TEST_ASSERT_FALSE(ParameterBlock_UpdateBlock(BLOCK_MODE, &lb));
}

/* =========================================================================
 * Test: write-check rejects out-of-range / invalid values
 * ========================================================================= */
TEST(CfgDataStore, WriteCheckRejectsInvalid)
{
  ReInit();

  float32_t badLat = 91.0f;
  TEST_ASSERT_EQUAL_INT(WRITE_ERROR_INVALID,
                        ParameterBlock_SetParam(PARAM_LATITUDE, &badLat, sizeof(badLat)));

  float32_t badLon = -181.0f;
  TEST_ASSERT_EQUAL_INT(WRITE_ERROR_INVALID,
                        ParameterBlock_SetParam(PARAM_LONGITUDE, &badLon, sizeof(badLon)));

  int16_t badOffset = 121;
  TEST_ASSERT_EQUAL_INT(WRITE_ERROR_INVALID,
                        ParameterBlock_SetParam(PARAM_ASTRO_OFFSET,
                                                &badOffset,
                                                sizeof(badOffset)));

  OperatingMode_t badMode = (OperatingMode_t)99;
  TEST_ASSERT_EQUAL_INT(WRITE_ERROR_INVALID,
                        ParameterBlock_SetParam(PARAM_MODE, &badMode, sizeof(badMode)));
}

/* =========================================================================
 * Test: size mismatch returns WRITE_ERROR_SIZE_MISMATCH
 * ========================================================================= */
TEST(CfgDataStore, SizeMismatch)
{
  ReInit();

  float32_t lat = 10.0f;
  TEST_ASSERT_EQUAL_INT(WRITE_ERROR_SIZE_MISMATCH,
                        ParameterBlock_SetParam(PARAM_LATITUDE, &lat, sizeof(double)));
}

void RunAllTests(void)
{
  RUN_TEST_CASE(CfgDataStore, DefaultValues);
  RUN_TEST_CASE(CfgDataStore, SetAndGetParam);
  RUN_TEST_CASE(CfgDataStore, SetAndGetBlock);
  RUN_TEST_CASE(CfgDataStore, UpdateBlock);
  RUN_TEST_CASE(CfgDataStore, WriteCheckRejectsInvalid);
  RUN_TEST_CASE(CfgDataStore, SizeMismatch);
}
