/*
 * Oreka -- A media capture and retrieval platform
 * 
 * Copyright (C) 2005, orecx LLC
 *
 * http://www.orecx.com
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License.
 * Please refer to http://www.gnu.org/copyleft/gpl.html
 *
 */

#ifndef __MEDIACHUNKFILE_H__
#define __MEDIACHUNKFILE_H__

#include "audiofile/AudioFile.h"


/** File class for saving audio chunks as-is */
class MediaChunkFile : public AudioFile
{
public:
	MediaChunkFile();
	~MediaChunkFile();

	void Open(CStdString& filename, fileOpenModeEnum mode, bool stereo = false, int sampleRate = 8000);
	void Close();

	void WriteChunk(AudioChunkRef chunkRef);
	int ReadChunkMono(AudioChunkRef& chunkRef);

	CStdString GetExtension();
protected:

	FILE* m_stream;
};

#endif
