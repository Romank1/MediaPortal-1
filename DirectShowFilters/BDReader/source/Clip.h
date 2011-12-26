/*
 *  Copyright (C) 2005-2011 Team MediaPortal
 *  http://www.team-mediaportal.com
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#pragma once

#include "StdAfx.h"

#include <streams.h>
#include <map>
#include <vector>
#include <dshow.h>
#include "Packet.h"

using namespace std;

// TODO - enum
#define SUPERCEEDED_AUDIO_RETURN    1
#define SUPERCEEDED_VIDEO_RETURN    2
#define SUPERCEEDED_SUBTITLE_RETURN 4
#define SUPERCEEDED_AUDIO_FILL      8
#define SUPERCEEDED_VIDEO_FILL     16
#define SUPERCEEDED_SUBTITLE_FILL  32

#define FAKE_AUDIO_DURATION 320000LL
#define AC3_FRAME_LENGTH 1792

class CClip
{
public:
  CClip(int clipNumber, int playlistNumber, REFERENCE_TIME playlistFirstPacketTime, REFERENCE_TIME clipOffset, REFERENCE_TIME totalStreamOffset, bool audioPresent, REFERENCE_TIME duration);
  ~CClip(void);
  Packet* ReturnNextAudioPacket(REFERENCE_TIME playlistOffset);
  Packet* ReturnNextVideoPacket(REFERENCE_TIME playlistOffset);
  bool AcceptAudioPacket(Packet* packet);
  bool AcceptVideoPacket(Packet* packet);
  void FlushAudio(Packet* pPacketToKeep = NULL);
  void FlushVideo(Packet* pPacketToKeep = NULL);
  int  nClip;
  int  nPlaylist;
  bool noAudio;
  bool clipReset;
  void Superceed(int superceedType);
  bool IsSuperceeded(int superceedType);
  REFERENCE_TIME playlistFirstPacketTime;
  REFERENCE_TIME clipPlaylistOffset;
  void Reset(REFERENCE_TIME totalStreamOffset);
  bool FakeAudioAvailable();
  bool HasAudio();
  bool HasVideo();
  REFERENCE_TIME Incomplete();
  REFERENCE_TIME PlayedDuration();
  void SetVideoPMT(AM_MEDIA_TYPE *pmt);

  // starttime of the last audio packet buffered in the clip
  REFERENCE_TIME lastAudioPosition;
  // starttime of the last video packet buffered in the clip
  REFERENCE_TIME lastVideoPosition;

  // starttime of the last audio packet returned from the clip to the pin
  REFERENCE_TIME audioPlaybackPosition;
  // starttime of the last video packet returned from the clip to the pin
  REFERENCE_TIME videoPlaybackPosition;

  REFERENCE_TIME earliestPacketAccepted;

  // Clip duration as provided by libbluray 
  REFERENCE_TIME clipDuration;

  // offset of this clip from the beginning of the playlist
  REFERENCE_TIME m_playlistOffset;

  // Accurate clip starting time (when known).
  // Not set when selecting chapter from the menu
  REFERENCE_TIME m_rtClipStartingOffset;

protected:
  typedef vector<Packet*>::iterator ivecVideoBuffers;
  typedef vector<Packet*>::iterator ivecAudioBuffers;
  vector<Packet*> m_vecClipAudioPackets;
  vector<Packet*> m_vecClipVideoPackets;
  AM_MEDIA_TYPE *m_videoPmt;
  int superceeded;

  CCritSec m_sectionRead;

  // true would indicate that this is the first audio packet
  bool firstAudio;
  // true would indicate that this is the first video packet
  bool firstVideo;

  // indicates if this is the first packet to be buffered in clip
  bool firstPacketAccepted;
  // indicates if this is the first packet to be returned from the clip
  bool firstPacketReturned;

  Packet* GenerateFakeAudio(REFERENCE_TIME rtStart);
};

// Silent AC3 frame
static unsigned char ac3_sample[AC3_FRAME_LENGTH] = {
0x0B, 0x77, 0x9E, 0xCC, 0x1E, 0x40, 0xE1, 0xDE, 0xAC, 0xC0, 0x3F, 0xC4, 0xFF, 0x59, 0x70, 0x83, 
0xC2, 0x10, 0x1B, 0xC2, 0x02, 0x08, 0x40, 0x6F, 0x08, 0x0C, 0x15, 0x57, 0x8F, 0x9F, 0x3E, 0x7C, 
0xF9, 0xF3, 0xE7, 0xCF, 0x9F, 0x3E, 0x7C, 0xF9, 0xF3, 0xE7, 0xCF, 0x9F, 0x3E, 0x7D, 0x7D, 0xF1, 
0xF3, 0xD0, 0xF9, 0xF3, 0xE7, 0xCF, 0x38, 0x3E, 0x7C, 0xF9, 0xF3, 0xE6, 0xCA, 0x9F, 0x3E, 0x6B, 
0x5A, 0xBB, 0xE7, 0xCF, 0x9F, 0x3E, 0x7C, 0xF9, 0xF3, 0xE7, 0xCF, 0x95, 0x2A, 0x7C, 0xF9, 0x13, 
0x67, 0x9C, 0x1F, 0x3E, 0x7C, 0xF3, 0x81, 0x62, 0x4F, 0x9F, 0x36, 0x75, 0xD7, 0x7C, 0x7E, 0x67, 
0xBC, 0xE0, 0xF9, 0xD1, 0x67, 0x4F, 0x36, 0x3E, 0x6C, 0xF9, 0x51, 0x4D, 0x0F, 0x38, 0x2A, 0x7C, 
0xA6, 0x9B, 0xE2, 0xC5, 0x9F, 0x2A, 0x79, 0x71, 0xF3, 0xCE, 0x0E, 0x91, 0x16, 0x79, 0xB0, 0xB0, 
0xCB, 0x6E, 0x73, 0x40, 0x74, 0x53, 0x83, 0xA4, 0x4A, 0x9E, 0x7D, 0xDF, 0x1B, 0xD9, 0xE5, 0x89, 
0x3E, 0x74, 0x59, 0xF2, 0x8D, 0x8A, 0x9F, 0x16, 0x6C, 0x53, 0x83, 0xE7, 0xC5, 0x9F, 0x3E, 0x6C, 
0xF9, 0xF3, 0xCE, 0x0F, 0x9F, 0x16, 0x7C, 0xF2, 0x43, 0xE7, 0xC4, 0x61, 0x92, 0x29, 0xBB, 0x82, 
0x27, 0x9B, 0xE7, 0x92, 0x6A, 0xFD, 0xF3, 0xE7, 0x3E, 0xF7, 0xC6, 0xB7, 0x43, 0xE7, 0x4F, 0x38, 
0x3D, 0xAE, 0xA9, 0xF3, 0xDA, 0xEA, 0x9F, 0x16, 0x2C, 0xF9, 0xB2, 0x8E, 0x0F, 0x9F, 0x2A, 0x2C, 
0xF9, 0xF1, 0x67, 0x4F, 0x8B, 0x3E, 0x2C, 0xF9, 0xF3, 0xE7, 0xB0, 0xDF, 0x3D, 0x9A, 0xFD, 0x6F, 
0x07, 0xCD, 0x38, 0x3E, 0x7C, 0xF9, 0xB7, 0xBD, 0xF1, 0xAD, 0xE0, 0xF8, 0x71, 0x67, 0xCA, 0x69, 
0xBE, 0x7C, 0x59, 0xF3, 0xE6, 0xCA, 0x9F, 0x3E, 0x6C, 0xF8, 0xB3, 0xA6, 0xCF, 0x6B, 0xBA, 0x7C, 
0x59, 0xD3, 0xCE, 0x0D, 0x9F, 0x3E, 0x7C, 0x59, 0x12, 0xA7, 0xB5, 0x77, 0x6F, 0xAB, 0xBB, 0x83, 
0x64, 0xCE, 0x9F, 0x3E, 0x7D, 0xDF, 0x7C, 0x79, 0x5F, 0x80, 0x20, 0x40, 0x81, 0x02, 0x04, 0x09, 
0x00, 0x67, 0x73, 0x22, 0x34, 0xA1, 0x32, 0x00, 0x20, 0x1B, 0x5D, 0xDF, 0x3E, 0x7C, 0xDB, 0x6C, 
0x0D, 0xB6, 0xDB, 0xC7, 0x8F, 0x1E, 0x3C, 0x78, 0xC0, 0xDD, 0xB6, 0xDB, 0x6D, 0xA1, 0xE3, 0xE6, 
0xF9, 0xAD, 0x6B, 0x5A, 0xF9, 0xB6, 0xD6, 0xE0, 0x56, 0x1F, 0x39, 0x00, 0xA4, 0x8E, 0x04, 0x08, 
0xED, 0xAE, 0xFA, 0xDF, 0x3E, 0x6C, 0x0D, 0xB7, 0x6D, 0x81, 0xE0, 0x1E, 0x3C, 0x01, 0xE3, 0xC7, 
0x6C, 0x0C, 0x0C, 0x0D, 0xB0, 0x36, 0xDB, 0xE7, 0xCD, 0x6B, 0x6B, 0xE4, 0xB1, 0xF3, 0x6D, 0xAD, 
0xD6, 0xDB, 0x6C, 0xAD, 0xEA, 0xBE, 0x7C, 0x19, 0x47, 0x8D, 0x0D, 0x77, 0x7C, 0xED, 0xF3, 0x60, 
0x6D, 0xB6, 0xEC, 0x0F, 0x00, 0x07, 0x8F, 0x1E, 0x00, 0xF1, 0xA1, 0xA1, 0x81, 0xB6, 0xDB, 0x6D, 
0xBB, 0xE7, 0xCD, 0x6B, 0x5A, 0xFD, 0xAC, 0xAE, 0xDB, 0xAD, 0xAD, 0xB1, 0x70, 0xFB, 0x14, 0xED, 
0x70, 0x0D, 0xDA, 0xEE, 0xF9, 0xBE, 0x03, 0x6D, 0xB7, 0x6D, 0xBB, 0x6F, 0x1E, 0x3C, 0x03, 0xC8, 
0x3C, 0x6D, 0xA1, 0xB0, 0x36, 0xDB, 0x60, 0x61, 0xDF, 0x35, 0xAD, 0xAD, 0x7C, 0xC6, 0xB6, 0xDB, 
0x75, 0xB8, 0x50, 0x84, 0x83, 0x74, 0xB0, 0x06, 0xEE, 0xD7, 0xCE, 0xE8, 0x7C, 0xDB, 0x78, 0xDD, 
0xBB, 0x6D, 0xBC, 0x01, 0xE3, 0xC7, 0x8F, 0x18, 0x1B, 0xB6, 0xDB, 0x6D, 0xB0, 0x37, 0xCE, 0xF9, 
0xAF, 0x1A, 0xD6, 0x3E, 0x6B, 0x5B, 0x6D, 0xB6, 0x4F, 0xC3, 0xB3, 0x4F, 0x3B, 0x99, 0x4F, 0x80, 
0x07, 0xCF, 0xE1, 0xE1, 0x7C, 0x01, 0x21, 0x01, 0x07, 0xF0, 0xF0, 0xFE, 0x1E, 0x00, 0x00, 0x03, 
0x26, 0x99, 0x41, 0x9A, 0x09, 0x10, 0x01, 0x00, 0xDA, 0xEE, 0xF9, 0xF3, 0xE6, 0xDB, 0x60, 0x6D, 
0xB6, 0xDE, 0x3C, 0x78, 0xF1, 0xE3, 0xC6, 0x06, 0x0D, 0xB6, 0xDB, 0x6C, 0x0F, 0x1F, 0x37, 0xED, 
0x6B, 0x5A, 0xD7, 0xCD, 0xD6, 0xB6, 0xD5, 0x31, 0x59, 0xBC, 0x85, 0x04, 0x40, 0x20, 0x07, 0x6D, 
0xB7, 0xCE, 0xF9, 0xF3, 0x60, 0x6D, 0xBB, 0x6F, 0x0F, 0x00, 0xF1, 0xE0, 0x0F, 0x1E, 0x3B, 0x60, 
0x60, 0x68, 0x6D, 0x81, 0xB6, 0xDF, 0x3C, 0x6B, 0x5B, 0x6B, 0xB5, 0xB0, 0xEB, 0x69, 0xAD, 0xB7, 
0x5B, 0x6B, 0x6E, 0xF6, 0x03, 0xD0, 0xCA, 0x3C, 0x6B, 0x6D, 0x9B, 0xE7, 0x6F, 0x9B, 0x03, 0x6D, 
0xB7, 0x60, 0x78, 0x00, 0x3C, 0x78, 0xF0, 0x07, 0x8C, 0x0C, 0x0D, 0x0D, 0xB6, 0xDB, 0x6D, 0xDF, 
0x3E, 0x6B, 0x5B, 0x07, 0x2D, 0x53, 0xE7, 0x5B, 0x64, 0x6C, 0x8E, 0x87, 0xA8, 0xD7, 0x6B, 0x9C, 
0x6E, 0xD7, 0x77, 0xCD, 0xF2, 0x1B, 0x6D, 0x83, 0x6D, 0xDB, 0x78, 0xF1, 0xE0, 0x1E, 0x01, 0xE3, 
0x6C, 0x0D, 0x81, 0xB6, 0xDB, 0x03, 0x0E, 0xF9, 0xAD, 0x6B, 0x6B, 0xE9, 0x91, 0xB6, 0xDD, 0x76, 
0xC4, 0x24, 0x04, 0x33, 0xB5, 0x80, 0x33, 0xB6, 0xBE, 0x77, 0xC3, 0xE6, 0xDB, 0xC6, 0x0D, 0xDB, 
0x6D, 0xE0, 0x0F, 0x1E, 0x3C, 0x78, 0xC0, 0xDD, 0xB6, 0xDB, 0x6D, 0xA1, 0xBE, 0x77, 0xCD, 0xBA, 
0xD6, 0xB5, 0xF3, 0x5A, 0xDB, 0x6D, 0xB3, 0x70, 0x11, 0x99, 0x55, 0xB4, 0xC2, 0x70, 0x00, 0x3F, 
0xC5, 0x42, 0x03, 0x78, 0x40, 0x6F, 0x08, 0x08, 0x21, 0x01, 0x04, 0x20, 0x37, 0x00, 0x00, 0x18, 
0x90, 0xCB, 0x6C, 0x78, 0x48, 0x7C, 0x08, 0x00, 0xD7, 0x77, 0xCF, 0x9F, 0x36, 0xDB, 0x43, 0x6D, 
0xB6, 0xF1, 0xE3, 0xC7, 0x8F, 0x1E, 0x34, 0x37, 0x6D, 0xB6, 0xDB, 0x60, 0x0A, 0xF9, 0xBE, 0x6B, 
0x5A, 0xD2, 0x5C, 0xB5, 0xA9, 0xB5, 0x49, 0x8D, 0xAD, 0x8C, 0x28, 0x22, 0x01, 0x80, 0x18, 0x6B, 
0xBE, 0x77, 0xCF, 0x9B, 0x43, 0x6D, 0xDB, 0x60, 0x78, 0x07, 0x8F, 0x00, 0x78, 0xF1, 0xDB, 0x03, 
0x03, 0x03, 0x6C, 0x0D, 0xB0, 0xF8, 0x63, 0x5A, 0xDA, 0xF9, 0xAA, 0xAE, 0xDB, 0x23, 0xAD, 0xB7, 
0x6B, 0x5B, 0x74, 0xB0, 0x5D, 0x86, 0x41, 0xE3, 0x5B, 0x5D, 0xDF, 0x3B, 0x7C, 0xD8, 0x1B, 0x6D, 
0xBB, 0x43, 0xC8, 0x41, 0xE3, 0xC7, 0x88, 0x3C, 0x60, 0x60, 0x60, 0x6D, 0xB6, 0xDB, 0x76, 0xF9, 
0xF3, 0x5A, 0xD8, 0x57, 0x69, 0x1F, 0x57, 0x5D, 0x6B, 0x1C, 0x8A, 0x3B, 0xC7, 0xBB, 0x5C, 0xE3, 
0x76, 0xBB, 0xBE, 0x0F, 0x80, 0xDB, 0x6D, 0xDB, 0x6E, 0xDB, 0xC7, 0x8F, 0x00, 0xF0, 0x0F, 0x1B, 
0x60, 0x6D, 0x0D, 0xB6, 0xD8, 0x1B, 0x77, 0xCD, 0x6B, 0x5B, 0x5F, 0x35, 0xB5, 0xB7, 0x5B, 0x6E, 
0x2D, 0x1F, 0x62, 0x9D, 0xAE, 0x71, 0xBB, 0xB5, 0xF3, 0xB8, 0x1F, 0x36, 0xC2, 0xB7, 0x6E, 0xDB, 
0x6F, 0x10, 0x78, 0xF1, 0xE3, 0xC6, 0x06, 0xED, 0xB6, 0xDB, 0x6C, 0x0D, 0xF3, 0xBE, 0x6B, 0xC6, 
0xB5, 0xAF, 0x9A, 0x46, 0xDD, 0x6D, 0x9E, 0xA1, 0x0B, 0x49, 0xAF, 0xA5, 0xD0, 0x00, 0x01, 0xF2, 
0xF8, 0x01, 0x5F, 0x00, 0xA9, 0xE0, 0x41, 0x7C, 0x02, 0xA1, 0x07, 0x80, 0x00, 0x02, 0x1C, 0x00, 
0x19, 0xF1, 0x9E, 0x4D, 0xCE, 0xAE, 0xB0, 0xD0, 0x18, 0x83, 0x2F, 0x24, 0xE8, 0x21, 0x8A, 0x78, 
0x2C, 0x38, 0x33, 0xFE, 0xD6, 0x78, 0x99, 0xE5, 0xD4, 0x92, 0x2C, 0x5F, 0xD3, 0x32, 0xB0, 0xC1, 
0x21, 0xE0, 0x20, 0x1B, 0x5D, 0xDF, 0x3E, 0x7C, 0xDB, 0x6C, 0x0D, 0xB6, 0xDB, 0xC7, 0x8F, 0x1E, 
0x3C, 0x78, 0xC0, 0xDD, 0xB6, 0xDB, 0x6D, 0x81, 0xBB, 0xE0, 0xF9, 0xAD, 0x6B, 0x52, 0x95, 0xD6, 
0x4A, 0xCF, 0xC6, 0x42, 0xB4, 0xD0, 0x98, 0x82, 0x04, 0x00, 0x61, 0xAE, 0xF9, 0xDF, 0x3E, 0x6C, 
0x0D, 0xB7, 0x6D, 0x81, 0xE4, 0x1E, 0x3C, 0x41, 0xE3, 0xC7, 0x6C, 0x0C, 0x0C, 0x0D, 0xB4, 0x36, 
0xDB, 0xE7, 0xCD, 0x6D, 0x6B, 0xF6, 0xB5, 0xF3, 0x75, 0xAD, 0xBA, 0xDB, 0x64, 0x6D, 0xC6, 0xC3, 
0x74, 0x19, 0x07, 0x8D, 0x6D, 0x77, 0x7C, 0xED, 0xF3, 0x68, 0x6D, 0xB6, 0xEC, 0x0F, 0x00, 0x07, 
0x8F, 0x1E, 0x00, 0xF1, 0x81, 0x81, 0x81, 0xB6, 0xDB, 0x6D, 0xBB, 0xE7, 0xCD, 0x6B, 0x5A, 0xF9, 
0xA4, 0x7C, 0xDB, 0x75, 0xAD, 0xB2, 0x84, 0xE7, 0x24, 0xED, 0x73, 0x8E, 0xDB, 0x6E, 0xF9, 0xBE, 
0x03, 0x6D, 0xB7, 0x6D, 0x83, 0x6F, 0x1E, 0x3C, 0x03, 0xC0, 0x3C, 0x6D, 0x81, 0xB0, 0x36, 0xDB, 
0x68, 0x6E, 0xDF, 0x35, 0xAD, 0x6D, 0xAE, 0xD5, 0x3A, 0x4B, 0x6D, 0xB8, 0xE4, 0x7A, 0x8C, 0x76, 
0xB9, 0xC6, 0xEE, 0xD7, 0xCE, 0xE0, 0x7C, 0xDB, 0x78, 0xDD, 0xBB, 0x6D, 0xBC, 0x01, 0xE3, 0xC7, 
0x8F, 0x18, 0x1B, 0xB6, 0xDB, 0x6D, 0xB4, 0x37, 0xCE, 0xD5, 0xAF, 0x1A, 0x46, 0xBE, 0xB2, 0x9D, 
0x6D, 0xB6, 0x75, 0x03, 0x25, 0x4A, 0xBC, 0x58, 0x8F, 0x18, 0x07, 0xF8, 0xC8, 0x41, 0xE1, 0x08, 
0x08, 0x21, 0x01, 0x84, 0x20, 0xF0, 0x84, 0x06, 0xE0, 0x00, 0x09, 0x28, 0x01, 0x74, 0x60, 0xD1, 
0x72, 0x56, 0x40, 0x3E, 0x66, 0xFC, 0xB7, 0x4D, 0x4C, 0x2B, 0x80, 0xC3, 0xEA, 0x96, 0x5F, 0x5F, 
0xFB, 0x09, 0x11, 0xDB, 0x3E, 0x43, 0x56, 0xFB, 0x74, 0xF8, 0xF1, 0x7E, 0x63, 0xF9, 0x9C, 0x25, 
0xC3, 0xFD, 0x21, 0xB3, 0x55, 0xC3, 0x34, 0xE4, 0xDE, 0x0F, 0x9C, 0x78, 0x19, 0x9E, 0x1F, 0x32, 
0xC5, 0x2E, 0x6F, 0xCE, 0xA9, 0x84, 0x55, 0x0D, 0x83, 0x63, 0x8D, 0xB5, 0x0F, 0x83, 0xDC, 0x7F, 
0x0E, 0xA0, 0xFC, 0x29, 0x41, 0x89, 0x75, 0x8C, 0xDE, 0xBE, 0x04, 0x47, 0x40, 0x80, 0x6D, 0x7B, 
0x7C, 0xF9, 0xF3, 0x6D, 0xB0, 0x36, 0xDB, 0x6F, 0x1E, 0x3C, 0x78, 0xF1, 0xE3, 0xC3, 0x76, 0xDB, 
0x6D, 0xB6, 0x07, 0x8F, 0x9B, 0xE6, 0xB5, 0xAD, 0x6B, 0xE6, 0xDB, 0x5B, 0x2A, 0x19, 0x36, 0xCE, 
0x02, 0x51, 0xF8, 0x10, 0x03, 0xB6, 0xDB, 0xE7, 0x7C, 0xF9, 0xB0, 0x36, 0xC1, 0xB6, 0x87, 0x80, 
0x78, 0xF0, 0x07, 0x8F, 0x1D, 0xB0, 0x34, 0x30, 0x36, 0xD0, 0xDB, 0x6F, 0x99, 0x35, 0xA5, 0xB0, 
0xDB, 0x67, 0xCD, 0xB6, 0xB6, 0xDD, 0x6D, 0xA9, 0xB6, 0xF3, 0x15, 0xC0, 0x45, 0x1B, 0xB5, 0xB5, 
0xDD, 0xF5, 0xB7, 0xCD, 0x81, 0xB6, 0xD8, 0x30, 0x3C, 0x00, 0x1E, 0x3C, 0x78, 0x03, 0xC6, 0x86, 
0x06, 0x06, 0xDB, 0x6D, 0xB6, 0xEF, 0xA2, 0x35, 0xAD, 0x74, 0x30, 0xB5, 0xF3, 0x6D, 0xB0, 0x35, 
0x4B, 0x63, 0x8C, 0xAB, 0xB6, 0x00, 0x33, 0x69, 0xBB, 0xE6, 0xF8, 0x0D, 0xB6, 0xDD, 0xB6, 0xED, 
0xBC, 0x78, 0xF0, 0x0F, 0x00, 0xF1, 0xB6, 0x06, 0xC0, 0xDB, 0x6D, 0x81, 0xBB, 0x7C, 0xD6, 0xB5, 
0xB1, 0xF3, 0x5A, 0xDD, 0xAD, 0xB6, 0xE4, 0x41, 0xDE, 0x3D, 0xDA, 0xE0, 0x1B, 0xBB, 0x5F, 0x3D, 
0x81, 0xF3, 0x6C, 0x2B, 0x76, 0x0D, 0xB6, 0xF0, 0x07, 0x8F, 0x1E, 0x3C, 0x60, 0x6E, 0xDB, 0x6D, 
0xB6, 0xC0, 0xDF, 0x3B, 0xE6, 0xBC, 0x6B, 0x5B, 0x75, 0xAC, 0xAE, 0xB6, 0xAA, 0x2E, 0x0A, 0x5C, 
0xE2, 0xE4, 0x63, 0x38, 0x00, 0x1F, 0x3F, 0x87, 0x85, 0xF0, 0x09, 0xBE, 0x00, 0x90, 0x80, 0xE6, 
0x78, 0x10, 0x00, 0x00, 0x24, 0x90, 0x0A, 0xCD, 0xE5, 0xA3, 0xD2, 0xF4, 0xB5, 0x16, 0x8B, 0x60, 
0x2A, 0x51, 0x53, 0x3B, 0x1C, 0x67, 0x39, 0x40, 0x92, 0x4B, 0x1C, 0xBB, 0xA2, 0xF0, 0x1C, 0x67, 
0x45, 0x79, 0x27, 0x7E, 0x70, 0xC9, 0x3C, 0xCC, 0x4B, 0xE3, 0x6B, 0x6B, 0x9D, 0xDA, 0xA2, 0x67, 
0x7D, 0x81, 0xE0, 0xAA, 0xE2, 0x1C, 0x45, 0x40, 0xE2, 0x7C, 0xBF, 0x24, 0x85, 0xA6, 0x3D, 0x60, 
0xA2, 0xAB, 0xBE, 0xE0, 0x50, 0x70, 0x52, 0xBE, 0x96, 0x17, 0x07, 0xE6, 0x6A, 0x82, 0xB5, 0xB0, 
0x33, 0xCA, 0xE4, 0x11, 0x1D, 0x02, 0x01, 0xB6, 0xDD, 0xF3, 0xE7, 0xCD, 0xB6, 0xC0, 0xDB, 0x6D, 
0xBC, 0x78, 0xF1, 0xE3, 0xC7, 0x8C, 0x0D, 0xDB, 0x6D, 0xB6, 0xD8, 0x1E, 0x3E, 0x6F, 0x9A, 0xD6, 
0xB5, 0xAF, 0x9B, 0x6D, 0x6C, 0x56, 0x65, 0x8B, 0x22, 0x09, 0x07, 0xC0, 0x40, 0x0E, 0xDA, 0xEF, 
0x9D, 0xF3, 0xE6, 0xC0, 0xDB, 0x76, 0xD8, 0x1E, 0x01, 0xE3, 0xC0, 0x1E, 0x3C, 0x06, 0xD0, 0xC0, 
0xC0, 0xDB, 0x43, 0x6D, 0xBE, 0x78, 0xD6, 0xB4, 0xBE, 0xB3, 0x5F, 0x3A, 0xDA, 0xDB, 0x75, 0xB6, 
0xD7, 0x5B, 0x0C, 0x66, 0xE1, 0x90, 0x78, 0xD6, 0xD7, 0xB7, 0xCE, 0x1F, 0x36, 0x06, 0xDB, 0x6E, 
0xC0, 0xF0, 0x00, 0x78, 0xF1, 0xE0, 0x0F, 0xD8, 0x18, 0x18, 0x1B, 0x6D, 0xB6, 0xDB, 0xBE, 0x7C, 
0xD6, 0xB5, 0xAF, 0x98, 0xD7, 0xCD, 0xB6, 0xDB, 0x5B, 0x33, 0x0D, 0xD3, 0x0E, 0xD8, 0x38, 0xDD, 
0xB7, 0x6F, 0x9B, 0xE0, 0x36, 0xDB, 0x76, 0xDB, 0xB6, 0xF1, 0xE3, 0xC8, 0x3C, 0x83, 0xC6, 0xD8, 
0x1B, 0x03, 0x6D, 0xB6, 0x06, 0xDD, 0xF3, 0x5A, 0x46, 0xD8, 0x6D, 0x6B, 0x66, 0xB6, 0xDB, 0x93, 
0xC7, 0x49, 0x27, 0x6B, 0x80, 0x76, 0xED, 0x7D, 0x6E, 0x07, 0xCD, 0xB7, 0x8D, 0xDB, 0xB6, 0xDB, 
0xC0, 0x1E, 0x3C, 0x78, 0xF1, 0xA1, 0x83, 0x6D, 0xB6, 0xDB, 0x03, 0x7D, 0x6F, 0x5A, 0xF1, 0xAD, 
0x53, 0xE4, 0xB6, 0xBA, 0xE9, 0x29, 0x38, 0x39, 0x33, 0xFB, 0xE9, 0x88, 0xF0, 0x00, 0xF5, 0x65};