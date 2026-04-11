#include <M5GFX.h> 

void WxWindDrawWind(float speedKph, uint8_t compassDir);
#if 0
void WxWindDrawItem(char *valueName,
                    uint32_t gfxColour = WHITE, 
                    float value = 0.0, 
                    float valueMin = -99., 
                    float valueMax = +88.);
#endif

typedef int TFT_COLOUR;

void WxWindDrawItem(const char *valueName,
                    TFT_COLOUR gfxColour, 
                    float value, 
                    float valueMin, 
                    float valueMax);

