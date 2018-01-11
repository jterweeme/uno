#ifndef _STREAM_H_
#define _STREAM_H_
#include "misc.h"

class ostream
{
public:
    virtual void write(char c) { }
    inline void write(const char *s) { while (*s) write(*s++); }
    virtual void writeString(const char *s) { while (*s) write(*s++); }
    virtual void flush() { }
};

#endif

