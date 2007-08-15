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
#pragma warning( disable: 4786 )

#define _WINSOCKAPI_		// prevents the inclusion of winsock.h

#include <vector>
#include <bitset>

#include "ConfigManager.h"
#include "BatchProcessing.h"
#include "ace/OS_NS_unistd.h"
#include "audiofile/LibSndFileFile.h"
#include "Daemon.h"
#include "Filter.h"
#include "Reporting.h"

#ifndef WIN32
#include <pwd.h>
#include <grp.h>
#endif

TapeProcessorRef BatchProcessing::m_singleton;

void BatchProcessing::Initialize()
{
	if(m_singleton.get() == NULL)
	{
		m_singleton.reset(new BatchProcessing());
		TapeProcessorRegistry::instance()->RegisterTapeProcessor(m_singleton);
	}
}


BatchProcessing::BatchProcessing()
{
	m_threadCount = 0;

	struct tm date = {0};
	time_t now = time(NULL);
	ACE_OS::localtime_r(&now, &date);
	m_currentDay = date.tm_mday;
}

CStdString __CDECL__ BatchProcessing::GetName()
{
	return "BatchProcessing";
}

TapeProcessorRef  BatchProcessing::Instanciate()
{
	return m_singleton;
}

void BatchProcessing::AddAudioTape(AudioTapeRef& audioTapeRef)
{
	if (!m_audioTapeQueue.push(audioTapeRef))
	{
		// Log error
		LOG4CXX_ERROR(LOG.batchProcessingLog, CStdString("queue full"));
	}
}

void BatchProcessing::SetQueueSize(int size)
{
	m_audioTapeQueue.setSize(size);
}

void BatchProcessing::ThreadHandler(void *args)
{
	CStdString debug;
	CStdString logMsg;

	CStdString processorName("BatchProcessing");
	TapeProcessorRef batchProcessing = TapeProcessorRegistry::instance()->GetNewTapeProcessor(processorName);
	if(batchProcessing.get() == NULL)
	{
		LOG4CXX_ERROR(LOG.batchProcessingLog, "Could not instanciate BatchProcessing");
		return;
	}
	BatchProcessing* pBatchProcessing = (BatchProcessing*)(batchProcessing->Instanciate().get());

	pBatchProcessing->SetQueueSize(CONFIG.m_batchProcessingQueueSize);

	int threadId = 0;
	{
		MutexSentinel sentinel(pBatchProcessing->m_mutex);
		threadId = pBatchProcessing->m_threadCount++;
	}
	CStdString threadIdString = IntToString(threadId);
	debug.Format("thread Th%s starting - queue size:%d", threadIdString, CONFIG.m_batchProcessingQueueSize);
	LOG4CXX_INFO(LOG.batchProcessingLog, debug);

	bool stop = false;

	for(;stop == false;)
	{
		AudioFileRef fileRef;
		AudioFileRef outFileRef;
		AudioTapeRef audioTapeRef;
		CStdString trackingId = "[no-trk]";

		try
		{
			audioTapeRef = pBatchProcessing->m_audioTapeQueue.pop();
			if(audioTapeRef.get() == NULL)
			{
				if(Daemon::Singleton()->IsStopping())
				{
					stop = true;
				}
				if(Daemon::Singleton()->GetShortLived())
				{
					Daemon::Singleton()->Stop();
				}
			}
			else
			{
				fileRef = audioTapeRef->GetAudioFileRef();
				trackingId = audioTapeRef->m_trackingId;

				// Let's work on the tape we have pulled
				//CStdString threadIdString = IntToString(threadId);
				LOG4CXX_INFO(LOG.batchProcessingLog, "[" + trackingId + "] Th" + threadIdString + " processing " + audioTapeRef->GetIdentifier());

				//fileRef->MoveOrig();	// #### could do this only when original and output file have the same extension. Irrelevant for now as everything is captured as mcf file
				fileRef->Open(AudioFile::READ);

				AudioChunkRef chunkRef;
				AudioChunkRef tmpChunkRef;

				switch(CONFIG.m_storageAudioFormat)
				{
				case FfUlaw:
					outFileRef.reset(new LibSndFileFile(SF_FORMAT_ULAW | SF_FORMAT_WAV));
					break;
				case FfAlaw:
					outFileRef.reset(new LibSndFileFile(SF_FORMAT_ALAW | SF_FORMAT_WAV));
					break;
				case FfGsm:
					outFileRef.reset(new LibSndFileFile(SF_FORMAT_GSM610 | SF_FORMAT_WAV));
					break;
				case FfPcmWav:
				default:
					outFileRef.reset(new LibSndFileFile(SF_FORMAT_PCM_16 | SF_FORMAT_WAV));
				}

				FilterRef filter;
				FilterRef decoder1;
				FilterRef decoder2;
				FilterRef decoder;

				std::bitset<RTP_PAYLOAD_TYPE_MAX> seenRtpPayloadTypes;
				std::vector<FilterRef> decoders1;
				std::vector<FilterRef> decoders2;
				for(int pt=0; pt<RTP_PAYLOAD_TYPE_MAX; pt++)
				{
					decoder1 = FilterRegistry::instance()->GetNewFilter(pt);
					decoders1.push_back(decoder1);
					decoder2 = FilterRegistry::instance()->GetNewFilter(pt);
					decoders2.push_back(decoder2);
				}

				bool firstChunk = true;
				bool voIpSession = false;

				size_t numSamplesS1 = 0;
				size_t numSamplesS2 = 0;
				size_t numSamplesOut = 0;

				while(fileRef->ReadChunkMono(chunkRef))
				{
					// ############ HACK
					//ACE_Time_Value yield;
					//yield.set(0,1);
					//ACE_OS::sleep(yield);
					// ############ HACK

					AudioChunkDetails details = *chunkRef->GetDetails();
					decoder.reset();

					if(details.m_rtpPayloadType < -1 || details.m_rtpPayloadType > RTP_PAYLOAD_TYPE_MAX)
					{
						logMsg.Format("RTP payload type out of bound:%d", details.m_rtpPayloadType);
						throw(logMsg);
					}

					// Instanciate any decoder we might need during a VoIP session
					if(details.m_rtpPayloadType != -1)
					{
						voIpSession = true;

						if(details.m_channel == 2)
						{
							decoder2 = decoders2.at(details.m_rtpPayloadType);
							decoder = decoder2;
						}
						else
						{
							decoder1 = decoders1.at(details.m_rtpPayloadType);
							decoder = decoder1;
						}

						bool ptAlreadySeen = seenRtpPayloadTypes.test(details.m_rtpPayloadType);
						seenRtpPayloadTypes.set(details.m_rtpPayloadType);

						if(decoder.get() == NULL)
						{
							if(ptAlreadySeen == false)
							{
								// First time we see a particular unsupported payload type in this session, log it
								CStdString rtpPayloadType = IntToString(details.m_rtpPayloadType);
								LOG4CXX_ERROR(LOG.batchProcessingLog, "[" + trackingId + "] Th" + threadIdString + " unsupported RTP payload type:" + rtpPayloadType);
							}
							// We cannot decode this chunk due to unknown codec, go to next chunk
							continue;
						}
						else if(ptAlreadySeen == false)
						{
							// First time we see a particular supported payload type in this session, log it
							CStdString rtpPayloadType = IntToString(details.m_rtpPayloadType);
							LOG4CXX_INFO(LOG.batchProcessingLog, "[" + trackingId + "] Th" + threadIdString + " RTP payload type:" + rtpPayloadType);
						}
					}
					if(!voIpSession || (firstChunk && decoder.get()))
					{
						firstChunk = false;

						// At this point, we know we have a working codec, create an RTP mixer and open the output file
						if(voIpSession)
						{
							CStdString filterName("RtpMixer");
							filter = FilterRegistry::instance()->GetNewFilter(filterName);
							if(filter.get() == NULL)
							{
								debug = "Could not instanciate RTP mixer";
								throw(debug);
							}
						}

						CStdString path = CONFIG.m_audioOutputPath + "/" + audioTapeRef->GetPath();
						FileRecursiveMkdir(path);

						CStdString file = path + "/" + audioTapeRef->GetIdentifier();
						outFileRef->Open(file, AudioFile::WRITE, false, fileRef->GetSampleRate());
					}
					if(voIpSession)
					{	
						if(details.m_channel == 2)
						{
							decoder2->AudioChunkIn(chunkRef);
							decoder2->AudioChunkOut(tmpChunkRef);
							if(tmpChunkRef.get())
							{
								numSamplesS2 += tmpChunkRef->GetNumSamples();
							}
						}
						else
						{
							decoder1->AudioChunkIn(chunkRef);
							decoder1->AudioChunkOut(tmpChunkRef);
							if(tmpChunkRef.get())
							{
								numSamplesS1 += tmpChunkRef->GetNumSamples();
							}
						}
						filter->AudioChunkIn(tmpChunkRef);
						filter->AudioChunkOut(tmpChunkRef);
					}
					outFileRef->WriteChunk(tmpChunkRef);
					if(tmpChunkRef.get())
					{
						numSamplesOut += tmpChunkRef->GetNumSamples();
					}

					if(CONFIG.m_batchProcessingEnhancePriority == false)
					{
						// Give up CPU between every audio buffer to make sure the actual recording always has priority
						//ACE_Time_Value yield;
						//yield.set(0,1);	// 1 us
						//ACE_OS::sleep(yield);

						// Use this instead, even if it still seems this holds the whole process under Linux instead of this thread only.
						struct timespec ts;
						ts.tv_sec = 0;
						ts.tv_nsec = 1;
						ACE_OS::nanosleep (&ts, NULL);
					}
				}

				if(voIpSession && !firstChunk)
				{
					// Flush the RTP mixer
					AudioChunkRef stopChunk(new AudioChunk());
					stopChunk->GetDetails()->m_marker = MEDIA_CHUNK_EOS_MARKER;
					filter->AudioChunkIn(stopChunk);
					filter->AudioChunkOut(tmpChunkRef);
					outFileRef->WriteChunk(tmpChunkRef);
					if(tmpChunkRef.get())
					{
						numSamplesOut += tmpChunkRef->GetNumSamples();
					}
				}

				fileRef->Close();
				outFileRef->Close();
				logMsg.Format("[%s] Th%s stop: num samples: s1:%u s2:%u out:%u", trackingId, threadIdString, numSamplesS1, numSamplesS2, numSamplesOut);
				LOG4CXX_INFO(LOG.batchProcessingLog, logMsg);

				CStdString audioFilePath = CONFIG.m_audioOutputPath + "/" + audioTapeRef->GetPath();
				CStdString audioFileName, mcfFileName;

				audioFileName = audioFilePath + "/" + audioTapeRef->GetIdentifier() + outFileRef->GetExtension();
				mcfFileName = audioFilePath + "/" + audioTapeRef->GetIdentifier() + fileRef->GetExtension();

				//LOG4CXX_INFO(LOG.batchProcessingLog, "[" + trackingId + "] Th" + threadIdString + " audioFileName: " + audioFileName + " mcfFileName: " + mcfFileName);

#ifndef WIN32
				if(chmod(audioFileName.c_str(), CONFIG.m_audioFilePermissions))
				{
					CStdString logMsg;

					logMsg.Format("Error setting permissions of %s to %o: %s", audioFileName.c_str(), CONFIG.m_audioFilePermissions, strerror(errno));
					LOG4CXX_INFO(LOG.batchProcessingLog, "[" + trackingId + "] Th" + threadIdString + " " + logMsg);
				}

				if(chmod(mcfFileName.c_str(), CONFIG.m_audioFilePermissions))
				{
					CStdString logMsg;

					logMsg.Format("Error setting permissions of %s to %o: %s", mcfFileName.c_str(), CONFIG.m_audioFilePermissions, strerror(errno));
					LOG4CXX_INFO(LOG.batchProcessingLog, "[" + trackingId + "] Th" + threadIdString + " " + logMsg);
				}

				struct group fileGroup, *fgP = NULL;
				struct passwd fileUser, *fuP = NULL;
				char infoGroupBuf[4096], infoUserBuf[4096];

				memset(infoGroupBuf, 0, sizeof(infoGroupBuf));
				memset(infoUserBuf, 0, sizeof(infoUserBuf));
				memset(&fileGroup, 0, sizeof(fileGroup));
				memset(&fileUser, 0, sizeof(fileUser));

				if(!getgrnam_r(CONFIG.m_audioFileGroup.c_str(), &fileGroup, infoGroupBuf, sizeof(infoGroupBuf), &fgP))
				{
					if(!getpwnam_r(CONFIG.m_audioFileOwner.c_str(), &fileUser, infoUserBuf, sizeof(infoUserBuf), &fuP))
					{

						if(chown(audioFileName.c_str(), fileUser.pw_uid, fileGroup.gr_gid))
						{
							CStdString logMsg;

							logMsg.Format("Error setting ownership and group of %s to %s:%s: %s", audioFileName.c_str(), CONFIG.m_audioFileOwner, CONFIG.m_audioFileGroup, strerror(errno));
							LOG4CXX_INFO(LOG.batchProcessingLog, "[" + trackingId + "] Th" + threadIdString + " " + logMsg);
						}

						if(chown(mcfFileName.c_str(), fileUser.pw_uid, fileGroup.gr_gid))
						{
							CStdString logMsg;

							logMsg.Format("Error setting ownership and group of %s to %s:%s: %s", mcfFileName.c_str(), CONFIG.m_audioFileOwner, CONFIG.m_audioFileGroup, strerror(errno));
							LOG4CXX_INFO(LOG.batchProcessingLog, "[" + trackingId + "] Th" + threadIdString + " " + logMsg);
						}
					}
					else
					{
						CStdString logMsg;

						logMsg.Format("Failed to get user information for user %s, please check the AudioFileOwner parameter in config.xml", CONFIG.m_audioFileOwner);
						LOG4CXX_INFO(LOG.batchProcessingLog, "[" + trackingId + "] Th" + threadIdString + " " + logMsg);
					}
				}
				else
				{
					CStdString logMsg;

					logMsg.Format("Failed to get group information for group %s, please check the AudioFileGroup parameter in config.xml", CONFIG.m_audioFileGroup);
					LOG4CXX_INFO(LOG.batchProcessingLog, "[" + trackingId + "] Th" + threadIdString + " " + logMsg);
				}
				
#endif
				if(CONFIG.m_deleteNativeFile)
				{
					fileRef->Delete();
					LOG4CXX_INFO(LOG.batchProcessingLog, "[" + trackingId + "] Th" + threadIdString + " deleting native: " + audioTapeRef->GetIdentifier());
				}

				// Report tape ready message to orktrack
				CaptureEventRef readyEvent(new CaptureEvent);
				readyEvent->m_type = CaptureEvent::EtReady;
				readyEvent->m_timestamp = time(NULL);
				audioTapeRef->AddCaptureEvent(readyEvent, true);
				Reporting::Instance()->AddAudioTape(audioTapeRef);

				// Finished processing the tape, pass on to next processor
				pBatchProcessing->RunNextProcessor(audioTapeRef);
			}
		}
		catch (CStdString& e)
		{
			LOG4CXX_ERROR(LOG.batchProcessingLog, "[" + trackingId + "] Th" + threadIdString + " " + e);
			if(fileRef.get()) {fileRef->Close();}
			if(outFileRef.get()) {outFileRef->Close();}
			if(CONFIG.m_deleteFailedCaptureFile && fileRef.get() != NULL)
			{
				LOG4CXX_INFO(LOG.batchProcessingLog, "[" + trackingId + "] Th" + threadIdString + " deleting native and transcoded");
				if(fileRef.get()) {fileRef->Delete();}
				if(outFileRef.get()) {outFileRef->Delete();}
			}
		}
		//catch(...)
		//{
		//	LOG4CXX_ERROR(LOG.batchProcessingLog, CStdString("unknown exception"));
		//}
	}
	LOG4CXX_INFO(LOG.batchProcessingLog, CStdString("Exiting thread Th" + threadIdString));
}


