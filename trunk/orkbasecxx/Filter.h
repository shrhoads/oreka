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
#ifndef __FILTER_H__
#define __FILTER_H__

#include <list>
#include "ace/Singleton.h"
#include "AudioCapture.h"

#ifdef WIN32
#define __CDECL__ __cdecl
#else
#define __CDECL__
#endif

class Filter;

typedef boost::shared_ptr<Filter> FilterRef;

/** Filter Interface
 *  a filter is a black box that takes media chunks as an input and produces media chunks as an output
 *  it can be translating between two encodings (codec) or just processing the signal
 */
class DLL_IMPORT_EXPORT Filter
{
public:
	virtual FilterRef __CDECL__ Instanciate() = 0;
	virtual void __CDECL__ AudioChunkIn(AudioChunkRef& chunk) = 0;
	virtual void __CDECL__ AudioChunkOut(AudioChunkRef& chunk) = 0;
	virtual AudioEncodingEnum __CDECL__ GetInputAudioEncoding() = 0;
	virtual AudioEncodingEnum __CDECL__ GetOutputAudioEncoding() = 0;
	virtual CStdString __CDECL__ GetName() = 0;
	/** Input RTP payload time - this is overridden if the filter is a codec that accepts a certain
	    RTP payload type such as GSM. if not, returns -1 by default */
	virtual int __CDECL__ GetInputRtpPayloadType();
};
//===================================================================

class DLL_IMPORT_EXPORT AlawToPcmFilter : public Filter
{
public:
	FilterRef __CDECL__ Instanciate();
	void __CDECL__ AudioChunkIn(AudioChunkRef& chunk);
	void __CDECL__ AudioChunkOut(AudioChunkRef& chunk);
	AudioEncodingEnum __CDECL__ GetInputAudioEncoding();
	AudioEncodingEnum __CDECL__ GetOutputAudioEncoding();
	CStdString __CDECL__ GetName();
	int __CDECL__ GetInputRtpPayloadType();

private:
	AudioChunkRef m_outputAudioChunk;
};

//===================================================================

/** Filter Registry
*/
class DLL_IMPORT_EXPORT FilterRegistry
{
public:
	static FilterRegistry* instance();
	void RegisterFilter(FilterRef& Filter);
	FilterRef GetNewFilter(AudioEncodingEnum inputEncoding, AudioEncodingEnum outputEncoding);
	FilterRef GetNewFilter(int rtpPayloadType);
	FilterRef GetNewFilter(CStdString& filterName);

private:
	std::list<FilterRef> m_Filters;

	static FilterRegistry* m_singleton;
};

//typedef ACE_Singleton<FilterRegistry, ACE_Thread_Mutex> FilterRegistrySingleton;

#endif
