#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    x = x > 0 ? (x <= im.w ? x : im.w - 1) : 0;
    y = y > 0 ? (y <= im.h ? y : im.h - 1) : 0;
    c = c > 0 ? (c <= im.c ? c : im.c - 1) : 0;

    int p = im.w * im.h * c + im.w * y + x;

    return im.data[p];
}

void set_pixel(image im, int x, int y, int c, float v)
{
    x = x > 0 ? (x <= im.w ? x : im.w - 1) : 0;
    y = y > 0 ? (y <= im.h ? y : im.h - 1) : 0;
    c = c > 0 ? (c <= im.c ? c : im.c - 1) : 0;

    int p = im.w * im.h * c + im.w * y + x;

    im.data[p] = v;
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    
    int l = im.w * im.h * im.c;

    for(int i=0; i<l; i++)
    {
        copy.data[i] = im.data[i];
    }

    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);
    
    int offset = im.w * im.h;

    for(int i=0; i<offset; i++)
    {
        gray.data[i] = 0.3 * im.data[i] + 0.59 * im.data[i+offset] + 0.11 * im.data[i+offset*2];
    }

    return gray;
}

void shift_image(image im, int c, float v)
{
    int offset = im.w * im.h * c;

    for(int i=0; i<offset; i++)
    {
        im.data[i+offset] += v;
    }
}

void clamp_image(image im)
{
    int l = im.w * im.h * im.c;

    for(int i=0; i<l; i++)
    {
        im.data[i] = im.data[i] < 0 ? 0 : im.data[i] > 255 ? 255 : im.data[i];
    }
}


// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    int offset = im.w * im.h;

    for(int i=0; i<offset; i++)
    {
        float R = im.data[i];
        float G = im.data[i+offset];
        float B = im.data[i+offset*2];
        

        float V = three_way_max(R,  G,  B);
        float m = three_way_min(R,  G,  B);
        float C = V - m;
        float S = V != 0 ? C / V : 0;

        if(C == 0)
        {
            im.data[i] = 0;
        } else
        {
            float Hl;
            if(V == R)
            {
                Hl = (G-B)/C;
            } else if(V == G)
            {
                Hl = (B-R)/C + 2;
            } else {
                Hl = (R-G)/C + 4;
            }

            float H = Hl/6;
            if(Hl < 0)
            {
                H += 1;
            }

            im.data[i] = H;
        }
        
        im.data[i+offset] = S;
        im.data[i+offset*2] = V;
    }
}

void hsv_to_rgb(image im)
{
    int offset = im.w * im.h;

    for(int i=0; i<offset; i++)
    {
        float H = im.data[i];
        float S = im.data[i+offset];
        float V = im.data[i+offset*2];

        float C = S*V;
        
        float Hl = H*6;
        float X = C * (1 - fabs(fmodf(Hl, 2) - 1));
        float m = V - C;

        float Rl, Gl, Bl;
        if(0 <= Hl && Hl <= 1)
        {
            Rl = C; Gl = X; Bl = 0;
        } else if(1 < Hl && Hl <= 2)
        {
            Rl = X; Gl = C; Bl = 0;
        } else if(2 < Hl && Hl <= 3)
        {
            Rl = 0; Gl = C; Bl = X;
        } else if(3 < Hl && Hl <= 4)
        {
            Rl = 0; Gl = X; Bl = C;
        } else if(4 < Hl && Hl <= 5)
        {
            Rl = X; Gl = 0; Bl = C;
        } else
        {
            Rl = C; Gl = 0; Bl = X;
        }

        im.data[i] = Rl+m;
        im.data[i+offset] = Gl+m;
        im.data[i+offset*2] = Bl+m;
    }
}
