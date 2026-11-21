
#ifndef DONUT_RENDERER_H
#define DONUT_RENDERER_H

#include <stdbool.h>

typedef struct
{
    unsigned int width;
    unsigned int height;
} RendererDisplay;

bool Renderer_Init(void);
void Renderer_Shutdown(void);

void Renderer_BeginFrame(void);

void Renderer_DrawText(unsigned int x, unsigned int y, const char *text);
void Renderer_DrawTextTV(unsigned int x, unsigned int y, const char *text);
void Renderer_DrawTextDRC(unsigned int x, unsigned int y, const char *text);

void Renderer_EndFrame(void);

RendererDisplay Renderer_GetTV(void);
RendererDisplay Renderer_GetDRC(void);

#endif
