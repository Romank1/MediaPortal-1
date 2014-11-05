/*
    Copyright (C) 2007-2010 Team MediaPortal
    http://www.team-mediaportal.com

    This file is part of MediaPortal 2

    MediaPortal 2 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MediaPortal 2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MediaPortal 2.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "StdAfx.h"

#include "IntraFrameStreamVariantTag.h"

CIntraFrameStreamVariantTag::CIntraFrameStreamVariantTag(HRESULT *result)
  : CTag(result)
{
}

CIntraFrameStreamVariantTag::~CIntraFrameStreamVariantTag(void)
{
}

/* get methods */

/* set methods */

/* other methods */

bool CIntraFrameStreamVariantTag::IsMediaPlaylistItem(unsigned int version)
{
  return false;
}

bool CIntraFrameStreamVariantTag::IsMasterPlaylistItem(unsigned int version)
{
  return (version == PLAYLIST_VERSION_04);
}

bool CIntraFrameStreamVariantTag::IsPlaylistItemTag(void)
{
  return false;
}

bool CIntraFrameStreamVariantTag::ApplyTagToPlaylistItems(unsigned int version, CItemCollection *notProcessedItems, CPlaylistItemCollection *processedPlaylistItems)
{
  return false;
}

bool CIntraFrameStreamVariantTag::ParseTag(unsigned int version)
{
  bool result = __super::ParseTag(version);
  result &= (version == PLAYLIST_VERSION_04);

  if (result)
  {
    // successful parsing of tag
    // compare it to our tag
    result &= (wcscmp(this->tag, TAG_INTRA_FRAME_STREAM_VARIANT) == 0);

    if (result)
    {
      result &= this->ParseAttributes(version);
    }
  }

  return result;
}


/* protected methods */

CItem *CIntraFrameStreamVariantTag::CreateItem(void)
{
  HRESULT result = S_OK;
  CIntraFrameStreamVariantTag *item = new CIntraFrameStreamVariantTag(&result);
  CHECK_POINTER_HRESULT(result, item, result, E_OUTOFMEMORY);

  CHECK_CONDITION_EXECUTE(FAILED(result), FREE_MEM_CLASS(item));
  return item;
}

bool CIntraFrameStreamVariantTag::CloneInternal(CItem *item)
{
  bool result = __super::CloneInternal(item);
  CIntraFrameStreamVariantTag *tag = dynamic_cast<CIntraFrameStreamVariantTag *>(item);
  result &= (tag != NULL);

  return result;
}