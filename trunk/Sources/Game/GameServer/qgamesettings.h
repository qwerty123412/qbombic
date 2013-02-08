#ifndef QGAMESETTINGS_H
#define QGAMESETTINGS_H


// place constants & functions

static const unsigned SIZE_X = 26;
static const unsigned SIZE_Y = 18;

static inline unsigned at(unsigned x, unsigned y, unsigned line = SIZE_X)
{
    return y * line + x;
}

static inline void index(unsigned in, unsigned &x, unsigned &y, unsigned line = SIZE_X)
{
    x = in % line;
    y = in / line;
}

constexpr unsigned size(unsigned x = SIZE_X, unsigned y = SIZE_Y)
{
    return x * y;
}

// time constants & functions

static const unsigned FPS = 10;

constexpr unsigned tickDelay()
{
    return 1000 / FPS;
}

template<typename T>
constexpr unsigned time2frame()
{
    return T::TTL / tickDelay();
}

constexpr unsigned time2frame(unsigned ms)
{
    return ms / tickDelay();
}


#endif // QGAMESETTINGS_H
