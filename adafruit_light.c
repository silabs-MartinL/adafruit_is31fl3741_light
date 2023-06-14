/***************************************************************************//**
 * @file
 * @brief Light display for Adafruit is31fl3741 RGB LED Matrix.
 *******************************************************************************
 * # License
 * <b>Copyright 2023 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
// Includes
#include "em_common.h"
#include "sl_i2cspm_instances.h"
#include "adafruit_is31fl3741.h"
#include "adafruit_light.h"

// Defines
#define LED_SCALING    0xFF
#define CURRENT        0xFF
#define COLS           13
#define ROWS           9
// Colors, red channel lowered for this board
// See: http://www.rinkydinkelectronics.com/calc_rgb565.php
#define BLACK          0x0000
#define WHITE          0x6FFF
#define DGRAY          0x0882


// Local constants
static const char off[ROWS][COLS + 1] =
{
    "  GGGGG      ",
    " G     G     ",
    "G       GGGG ",
    "G       G   G",
    "G       G   G",
    "G       G   G",
    "G       GGGG ",
    " G     G     ",
    "  GGGGG      "
};
static const char on[ROWS][COLS + 1] =
{
    "  WWWWW      ",
    " WWWWWWW     ",
    "WWWWWWWWWWWW ",
    "WWWWWWWWW   W",
    "WWWWWWWWW   W",
    "WWWWWWWWW   W",
    "WWWWWWWWWWWW ",
    " WWWWWWW     ",
    "  WWWWW      "
};

// Local variables
static sl_status_t status = SL_STATUS_FAIL;
static bool state = false;

// -----------------------------------------------------------------------------
// Public Functions
// -----------------------------------------------------------------------------
sl_status_t adafruit_light_init(void)
{
  // Initialise comms
  status = adafruit_is31fl3741_init(sl_i2cspm_qwiic);
  if (SL_STATUS_OK == status)
  {
    // Initialise device
    adafruit_is31fl3741_set_global_led_scaling(LED_SCALING);
    adafruit_is31fl3741_set_global_current(CURRENT);
    adafruit_is31fl3741_enable(true);
    // Initialise display
    adafruit_light_set(state);
  }
  return status;
}

void adafruit_light_set(bool new_state)
{
  // Initialised ?
  if (SL_STATUS_OK == status)
  {
    uint16_t x;
    uint16_t y;
    char c;
    uint16_t color;

    // Loop through pixels
    for (x = 0; x < COLS; x++)
    {
      for (y = 0; y < ROWS; y++)
      {
        // Get color character
        if (true == new_state)
          c = on[y][x];
        else
          c = off[y][x];
        // Get color for character (rgb565)
        switch (c)
        {
        case 'W':
          color = WHITE;
          break;
        case 'G':
          color = DGRAY;
          break;
        default:
          color = BLACK;
          break;
        }
        // Draw pixel
        adafruit_is31fl3741_draw_pixel(x, y, color);
      }
    }
    // Update display
    adafruit_is31fl3741_show();
    // Retain state
    state = new_state;
  }
}

bool adafruit_light_get(void)
{
  return state;
}
