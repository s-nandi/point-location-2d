#ifndef HELPER_H_DEFINED
#define HELPER_H_DEFINED

// Returns next index after i, rolling back to 0 if next index is out of bounds
int nextIndex(int i, int sz)
{
    return (i + 1 < sz) ? i + 1 : 0;
}

#endif
