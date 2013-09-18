//
// Created by Sergei Shubin <s.v.shubin@gmail.com>
//

#include <float.h>
#include "Helpers.h"

#ifdef _WIN32
# define isnan _isnan
# define NAN 0
#else
# include <math.h>
#endif

template<typename t>
t clamp(t v, t min, t max) {
	if (v > max) {
		return max;
	} else if (v < min) {
		return min;
	}
	return v;
}

template<typename t1, typename t2>
t2 lerp(t1 a, t1 b, t2 k) {
	return (t2)(a + (b-a)*k);
}

extern "C"
int clampi(int v, int min, int max) {
	return clamp(v, min, max);
}

extern "C"
unsigned int clampui(unsigned int v, unsigned int min, unsigned int max) {
	return clamp(v, min, max);
}

extern "C"
float clampf(float v, float min, float max) {
	return clamp(v, min, max);
}

extern "C"
double clampd(double v, double min, double max) {
	return clamp(v, min, max);
}

typedef struct {
    double h;       // angle in degrees
    double s;       // percent
    double v;       // percent
} hsv;

static hsv      rgb2hsv(rgb in);
static rgb      hsv2rgb(hsv in);

hsv rgb2hsv(rgb in)
{
    hsv         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    out.v = max;                                // v
    delta = max - min;
    if( max > 0.0 ) {
        out.s = (delta / max);                  // s
    } else {
        // r = g = b = 0                        // s = 0, v is undefined
        out.s = 0.0;
        out.h = NAN;                            // its now undefined
        return out;
    }
    if( in.r >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
    else
    if( in.g >= max )
        out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}


rgb hsv2rgb(hsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        if(isnan(in.h)) {   // in.h == NAN
            out.r = in.v;
            out.g = in.v;
            out.b = in.v;
            return out;
        }
        // error - should never happen
        out.r = 0.0;
        out.g = 0.0;
        out.b = 0.0;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;     
}


rgb rgb_interp(rgb a, rgb b, float k) {
	hsv ha = rgb2hsv(a);
	hsv hb = rgb2hsv(b);
	hsv hr = { lerp(ha.h, hb.h, (double)k), lerp(ha.s, hb.s, (double)k), lerp(ha.v, hb.v, (double)k) };
	return hsv2rgb(hr);
}

rgb rgb_lerp(rgb a, rgb b, float k) {
	rgb result(lerp(a.r, b.r, k), lerp(a.g, b.g, k), lerp(a.b, b.b, k));
	return result;
}
