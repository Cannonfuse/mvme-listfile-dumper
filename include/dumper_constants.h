#include <unistd.h>
#include "TROOT.h"


#ifndef DUMPERCONSTS
#define DUMPERCONSTS

static const size_t maxfilesize = (size_t)(0.5*(sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGE_SIZE)));
static const size_t maxread = (size_t)(0.05 * maxfilesize);
static const char* installpath = std::getenv("DUMPERSYS");

#endif