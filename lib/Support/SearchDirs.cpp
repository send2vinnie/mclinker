/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/Support/SearchDirs.h>
#include <mcld/Support/FileSystem.h>

using namespace mcld;

//==========================
// SearchDirs
sys::fs::Path& SearchDirs::iterator::operator*()
{
}

const sys::fs::Path& SearchDirs::const_iterator::operator*()
{
}

SearchDirs::iterator SearchDirs::begin()
{
}

SearchDirs::iterator SearchDirs::end()
{
}

SearchDirs::const_iterator SearchDirs::begin() const
{
}

SearchDirs::const_iterator SearchDirs::end() const
{
}

SearchDirs::iterator SearchDirs::find(const std::string& pNamespec)
{
}

SearchDirs::const_iterator SearchDirs::find(const std::string& pNamespec) const
{
}

void SearchDirs::add(const MCLDDirectory& pDirectory)
{
}

