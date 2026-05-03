#ifndef _WXUI
#define _WXUI

#include <M5GFX.h> 
#include <WxCommon.h>
#include <WxUI.h>
#include "gfxfont.h"

typedef int TFT_COLOUR;

extern unsigned long getUTC();
extern char *getHHMMapm(uint32_t utc);
extern char *getDDMMYY(uint32_t utc);


#if 0
extern void WxWindDrawItem(const char *valueName,
                    TFT_COLOUR gfxColour, 
                    float value, 
                    float valueMin, 
                    float valueMax);
#endif

void WxDrawWindDisplay(WIND_ITEM &item);

#endif

