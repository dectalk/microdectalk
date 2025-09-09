//#include "dectalkf_hlsyn.h"
#include "dectalkf_klsyn.h"

/* Horrible Windows fix */
#ifdef _WIN32
#ifndef __unix__
#define __unix__
#endif
#endif

// On UNIX/Linux based machines, this is the location where DECtalk will assume it was built.
#ifndef DECTALK_INSTALL_PREFIX
#define DECTALK_INSTALL_PREFIX "/opt/dectalk"
#endif
