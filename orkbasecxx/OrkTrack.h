/*
 * Oreka -- A media capture and retrieval platform
 *
 * Copyright (C) 2005, orecx LLC
 *
 * http://www.orecx.com
 *
 */
#ifndef __ORKTRACK_H__
#define __ORKTRACK_H__ 1

#include "shared_ptr.h"
#include "OrkClient.h"

struct OrkTrackHost {
	char m_serverHostname[256];
	int m_serverPort;
};
typedef oreka::shared_ptr<OrkTrackHost> OrkTrackHostRef;

class DLL_IMPORT_EXPORT_ORKBASE OrkTrack {
public:
	static void Initialize();
	static void Run(void *args);
};

#endif
