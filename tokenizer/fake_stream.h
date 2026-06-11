// ========================================================================= //
// Andy Kwak 2026

// This was for the fake stream implementation where I used an array and 
// simply moved indices.

// ========================================================================= //


// ========================================================================= //
#ifndef FAKE_STREAM_H
#define FAKE_STREAM_H

#include "token.h"


typedef struct {
        Token** T;
        int index;
} Stream;

// ========================================================================= //

#endif
