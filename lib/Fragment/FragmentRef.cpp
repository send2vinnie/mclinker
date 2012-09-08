//===- FragmentRef.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Fragment/FragmentRef.h>

#include <cstring>
#include <cassert>

#include <llvm/Support/Casting.h>
#include <llvm/Support/ManagedStatic.h>

#include <mcld/Fragment/Fragment.h>
#include <mcld/LD/Layout.h>
#include <mcld/LD/LDSection.h>
#include <mcld/LD/SectionData.h>
#include <mcld/Support/GCFactory.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/LD/RegionFragment.h>

using namespace mcld;

typedef GCFactory<FragmentRef, MCLD_SECTIONS_PER_INPUT> FragRefFactory;

static llvm::ManagedStatic<FragRefFactory> g_FragRefFactory;

//===----------------------------------------------------------------------===//
// FragmentRef
//===----------------------------------------------------------------------===//
FragmentRef::FragmentRef()
  : m_pFragment(NULL), m_Offset(0) {
}

FragmentRef::FragmentRef(Fragment& pFrag,
                         FragmentRef::Offset pOffset)
  : m_pFragment(&pFrag), m_Offset(pOffset) {
}

/// create - create a fragment reference for a given fragment.
///
/// @param pFrag - the given fragment
/// @param pOffset - the offset, can be larger than the fragment, but can not
///                  be larger than the section size.
/// @return if the offset is legal, return the fragment reference. Otherwise,
/// return NULL.
FragmentRef* FragmentRef::create(Fragment& pFrag, uint64_t pOffset)
{
  int64_t offset = pOffset;
  Fragment* frag = &pFrag;

  while (NULL != frag) {
    offset -= frag->size();
    if (offset <= 0)
      break;
    frag = frag->getNextNode();
  }

  FragmentRef* result = g_FragRefFactory->allocate();

  if (NULL == frag)
    new (result) FragmentRef();
  else
    new (result) FragmentRef(*frag, offset + frag->size());

  return result;
}

FragmentRef* FragmentRef::create(LDSection& pSection, uint64_t pOffset)
{
  return create(pSection.getSectionData()->front(), pOffset);
}

FragmentRef& FragmentRef::assign(const FragmentRef& pCopy)
{
  m_pFragment = const_cast<Fragment*>(pCopy.m_pFragment);
  m_Offset = pCopy.m_Offset;
  return *this;
}

FragmentRef& FragmentRef::assign(Fragment& pFrag, FragmentRef::Offset pOffset)
{
  m_pFragment = &pFrag;
  m_Offset = pOffset;
  return *this;
}

void FragmentRef::memcpy(void* pDest, size_t pNBytes, Offset pOffset) const
{
  // check if the offset is still in a legal range.
  if (NULL == m_pFragment)
    return;
  unsigned int total_offset = m_Offset + pOffset;
  switch(m_pFragment->getKind()) {
    case Fragment::Region: {
      RegionFragment* region_frag = static_cast<RegionFragment*>(m_pFragment);
      unsigned int total_length = region_frag->getRegion().size();
      if (total_length < (total_offset+pNBytes))
        pNBytes = total_length - total_offset;

      std::memcpy(pDest, region_frag->getRegion().getBuffer(total_offset), pNBytes);
      return;
    }
    case Fragment::Alignment:
    case Fragment::Fillment:
    default:
      return;
  }
}

FragmentRef::Address FragmentRef::deref()
{
  if (NULL == m_pFragment)
    return NULL;
  Address base = NULL;
  switch(m_pFragment->getKind()) {
    case Fragment::Region:
      base = static_cast<RegionFragment*>(m_pFragment)->getRegion().getBuffer();
      break;
    case Fragment::Alignment:
    case Fragment::Fillment:
    default:
      return NULL;
  }
  return base + m_Offset;
}

FragmentRef::ConstAddress FragmentRef::deref() const
{
  if (NULL == m_pFragment)
    return NULL;
  ConstAddress base = NULL;
  switch(m_pFragment->getKind()) {
    case Fragment::Region:
      base = static_cast<const RegionFragment*>(m_pFragment)->getRegion().getBuffer();
      break;
    case Fragment::Alignment:
    case Fragment::Fillment:
    default:
      return NULL;
  }
  return base + m_Offset;
}

FragmentRef::Offset FragmentRef::getOutputOffset() const
{
  Offset result = 0;
  if (NULL != m_pFragment)
    result = m_pFragment->getOffset();
  return (result + m_Offset);
}
