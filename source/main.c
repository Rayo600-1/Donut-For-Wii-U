#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <coreinit/thread.h>
#include <coreinit/time.h>
#include <whb/proc.h>

#include "renderer.h"

#define WIDTH 80
#define HEIGHT 22

// CONFIGURACION
#define TARGET_FPS 15

#define DONUT_TEXT_LINES 4

static const char *DONUT_TEXT[DONUT_TEXT_LINES] = {
    "Donut.C for Wii U",
    "Original code by Andy Sloane",
    "https://www.a1k0n.net/2021/01/13/optimizing-donut.html",
    "Ported to Wii U by Rayo600"
};

static float A = 0.0f;
static float B = 0.0f;

static float z[WIDTH * HEIGHT];
static char b[WIDTH * HEIGHT];
static char linea[WIDTH + 1];

static void DrawDonut(float scale, bool gamepad)
{
    memset(b, ' ', sizeof(b));
    memset(z, 0, sizeof(z));

    for (float j = 0.0f; j < 6.28f; j += 0.07f)
    {
        for (float i = 0.0f; i < 6.28f; i += 0.02f)
        {
            float c = sinf(i);
            float d = cosf(j);
            float e = sinf(A);
            float f = sinf(j);
            float g = cosf(A);
            float h = d + 2.0f;

            float D = 1.0f / (c * h * e + f * g + 5.0f);
            float l = cosf(i);
            float m = cosf(B);
            float n = sinf(B);

            float t = c * h * g - f * e;

            int x = (int)(WIDTH / 2 + 30.0f * scale * D *
                (l * h * m - t * n));

            int y = (int)(HEIGHT / 2 + 15.0f * scale * D *
                (l * h * n + t * m));

            int o = x + WIDTH * y;

            int N = (int)(8.0f *
                ((f * e - c * d * g) * m
                - c * d * e - f * g
                - l * d * n));

            if (y >= 0 && y < HEIGHT &&
                x >= 0 && x < WIDTH &&
                D > z[o])
            {
                z[o] = D;

                int brightness = N;

                if (brightness < 0) brightness = 0;
                if (brightness > 11) brightness = 11;

                b[o] = ".,-~:;=!*#$@"[brightness];
            }
        }
    }

    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            linea[x] = b[x + WIDTH * y];
        }

        linea[WIDTH] = '\0';

        if (gamepad)
            Renderer_DrawTextDRC(0, y, linea);
        else
            Renderer_DrawTextTV(0, y, linea);
    }

    A += 0.04f;
    B += 0.02f;
}

int main(void)
{
    WHBProcInit();

    if (!Renderer_Init())
    {
        WHBProcShutdown();
        return 0;
    }

    uint64_t lastFrame = 0;
    uint64_t fpsStart = OSGetTime();

    unsigned int frameCount = 0;
    unsigned int displayedFPS = 0;

    const uint64_t frameTime =
        OSMicrosecondsToTicks(1000000 / TARGET_FPS);

    char fpsText[40];

    while (WHBProcIsRunning())
    {
        uint64_t frameStart = OSGetTime();

        Renderer_BeginFrame();

        DrawDonut(1.0f, false);   // TV: tamaño original
        DrawDonut(0.70f, true);   // GamePad: 70 % de tamaño

        A += 0.04f;
        B += 0.02f;

        for (int i = 0; i < DONUT_TEXT_LINES; i++)
        {
            Renderer_DrawText(0, HEIGHT + i, DONUT_TEXT[i]);
        }

        // Medidor de FPS
        snprintf(fpsText, sizeof(fpsText),
                 "FPS: %u / %d", displayedFPS, TARGET_FPS);

        Renderer_DrawText(0, HEIGHT + DONUT_TEXT_LINES + 1,
                          fpsText);

        Renderer_EndFrame();

        // Calcular FPS real
        frameCount++;

        uint64_t now = OSGetTime();
        uint64_t fpsElapsed = OSTicksToMicroseconds(now - fpsStart);

        if (fpsElapsed >= 1000000)
        {
            displayedFPS = (unsigned int)
                ((frameCount * 1000000ULL) / fpsElapsed);

            frameCount = 0;
            fpsStart = now;
        }

        // Limitar FPS
        if (lastFrame != 0)
        {
            uint64_t elapsed = now - lastFrame;

            if (elapsed < frameTime)
            {
                OSSleepTicks(frameTime - elapsed);
            }
        }

        lastFrame = OSGetTime();
    }

    Renderer_Shutdown();
    WHBProcShutdown();

    return 0;
}
