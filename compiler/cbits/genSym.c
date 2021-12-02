#include <Rts.h>
#include <assert.h>
#include "Unique.h"
#include "ghcversion.h"

// These global variables have been moved into the RTS.  It allows them to be
// shared with plugins even if two different instances of the GHC library are
// loaded at the same time (#19940)
#if !MIN_VERSION_GLASGOW_HASKELL(8,10,0,0)
static HsInt GenSymCounter = 0;
static HsInt GenSymInc = 1;
#endif

#define UNIQUE_BITS (sizeof (HsInt) * 8 - UNIQUE_TAG_BITS)
#define UNIQUE_MASK ((1ULL << UNIQUE_BITS) - 1)

STATIC_INLINE void checkUniqueRange(HsInt u STG_UNUSED) {
#if DEBUG
    // Uh oh! We will overflow next time a unique is requested.
    assert(u != UNIQUE_MASK);
#endif
}

HsInt genSym(void) {
#if defined(THREADED_RTS)
    if (n_capabilities == 1) {
        GenSymCounter = (GenSymCounter + GenSymInc) & UNIQUE_MASK;
        checkUniqueRange(GenSymCounter);
        return GenSymCounter;
    } else {
        HsInt n = atomic_inc((StgWord *)&GenSymCounter, GenSymInc)
          & UNIQUE_MASK;
        checkUniqueRange(n);
        return n;
    }
#else
    GenSymCounter = (GenSymCounter + GenSymInc) & UNIQUE_MASK;
    checkUniqueRange(GenSymCounter);
    return GenSymCounter;
#endif
}

void initGenSym(HsInt NewGenSymCounter, HsInt NewGenSymInc) {
  GenSymCounter = NewGenSymCounter;
  GenSymInc = NewGenSymInc;
}
