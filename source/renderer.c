
#include <malloc.h>
#include <stdlib.h>
#include <stdbool.h>

#include <coreinit/screen.h>

#include "renderer.h"

static void *tvBuffer = NULL;
static void *drcBuffer = NULL;
static bool rendererInitialized = false;

bool Renderer_Init(void)
{
    if (rendererInitialized)
        return true;

    OSScreenInit();

    size_t tvSize = OSScreenGetBufferSizeEx(SCREEN_TV);
    size_t drcSize = OSScreenGetBufferSizeEx(SCREEN_DRC);

    tvBuffer = memalign(0x100, tvSize);
    drcBuffer = memalign(0x100, drcSize);

    if (!tvBuffer || !drcBuffer)
    {
        free(tvBuffer);
        free(drcBuffer);

        tvBuffer = NULL;
        drcBuffer = NULL;

        OSScreenShutdown();
        return false;
    }

    OSScreenSetBufferEx(SCREEN_TV, tvBuffer);
    OSScreenSetBufferEx(SCREEN_DRC, drcBuffer);

    OSScreenEnableEx(SCREEN_TV, true);
    OSScreenEnableEx(SCREEN_DRC, true);

    rendererInitialized = true;
    return true;
}

void Renderer_Shutdown(void)
{
    if (!rendererInitialized)
        return;

    OSScreenEnableEx(SCREEN_TV, false);
    OSScreenEnableEx(SCREEN_DRC, false);

    OSScreenShutdown();

    free(tvBuffer);
    free(drcBuffer);

    tvBuffer = NULL;
    drcBuffer = NULL;

    rendererInitialized = false;
}

void Renderer_BeginFrame(void)
{
    if (!rendererInitialized)
        return;

    OSScreenClearBufferEx(SCREEN_TV, 0x00000000);
    OSScreenClearBufferEx(SCREEN_DRC, 0x00000000);
}

void Renderer_DrawTextTV(unsigned int x, unsigned int y, const char *text)
{
    if (rendererInitialized && text)
        OSScreenPutFontEx(SCREEN_TV, x, y, text);
}

void Renderer_DrawTextDRC(unsigned int x, unsigned int y, const char *text)
{
    if (rendererInitialized && text)
        OSScreenPutFontEx(SCREEN_DRC, x, y, text);
}

void Renderer_DrawText(unsigned int x, unsigned int y, const char *text)
{
    Renderer_DrawTextTV(x, y, text);
    Renderer_DrawTextDRC(x, y, text);
}

void Renderer_EndFrame(void)
{
    if (!rendererInitialized)
        return;

    OSScreenFlipBuffersEx(SCREEN_TV);
    OSScreenFlipBuffersEx(SCREEN_DRC);
}

RendererDisplay Renderer_GetTV(void)
{
    RendererDisplay display = { 80, 45 };
    return display;
}

RendererDisplay Renderer_GetDRC(void)
{
    RendererDisplay display = { 80, 45 };
    return display;
}
