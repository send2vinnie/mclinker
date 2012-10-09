//===- ELFObjectWriter.cpp ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/ELFObjectWriter.h>

#include <mcld/Module.h>
#include <mcld/Target/GNULDBackend.h>
#include <mcld/Fragment/FragmentLinker.h>
#include <mcld/Support/MemoryArea.h>

#include <llvm/Support/system_error.h>
using namespace llvm;
using namespace mcld;

//===----------------------------------------------------------------------===//
// ELFObjectWriter
//===----------------------------------------------------------------------===//
ELFObjectWriter::ELFObjectWriter(GNULDBackend& pBackend,
                                 FragmentLinker& pLinker)
  : ObjectWriter(pBackend), ELFWriter(pBackend), m_Linker(pLinker) {
}

ELFObjectWriter::~ELFObjectWriter()
{
}

llvm::error_code ELFObjectWriter::writeObject(Module& pModule,
                                              MemoryArea& pOutput)
{
  // Write out name pool sections: .symtab, .strtab
  target().emitRegNamePools(pModule,
                            m_Linker.getLayout(),
                            pOutput);

  // Write out regular ELF sections
  Module::iterator sect, sectEnd = pModule.end();
  for (sect = pModule.begin(); sect != sectEnd; ++sect) {
    MemoryRegion* region = NULL;
    // request output region
    switch((*sect)->kind()) {
      case LDFileFormat::Regular:
      case LDFileFormat::Relocation:
      case LDFileFormat::Target:
      case LDFileFormat::Debug:
      case LDFileFormat::GCCExceptTable:
      case LDFileFormat::EhFrame: {
        region = pOutput.request((*sect)->offset(), (*sect)->size());
        if (NULL == region) {
          llvm::report_fatal_error(llvm::Twine("cannot get enough memory region for output section `") +
                                   llvm::Twine((*sect)->name()) +
                                   llvm::Twine("'.\n"));
        }
        break;
      }
      case LDFileFormat::Null:
      case LDFileFormat::NamePool:
      case LDFileFormat::BSS:
      case LDFileFormat::Note:
      case LDFileFormat::MetaData:
      case LDFileFormat::Version:
      case LDFileFormat::EhFrameHdr:
        // ignore these sections
        continue;
      default: {
        llvm::errs() << "WARNING: unsupported section kind: "
                     << (*sect)->kind()
                     << " of section "
                     << (*sect)->name()
                     << ".\n";
        continue;
      }
    }

    // write out sections with data
    switch((*sect)->kind()) {
      case LDFileFormat::Regular:
      case LDFileFormat::Debug:
      case LDFileFormat::GCCExceptTable:
      case LDFileFormat::EhFrame: {
        // FIXME: if optimization of exception handling sections is enabled,
        // then we should emit these sections by the other way.
        emitSectionData(m_Linker.getLayout(), **sect, *region);
        break;
      }
      case LDFileFormat::Relocation:
        emitRelocation(m_Linker.getLayout(), m_Linker.getLDInfo(), **sect, *region);
        break;
      case LDFileFormat::Target:
        target().emitSectionData(**sect,
                                 m_Linker.getLayout(),
                                 *region);
        break;
      default:
        continue;
    }
  } // end of for loop

  if (32 == target().bitclass()) {
    // Write out ELF header
    // Write out section header table
    emitELF32ShStrTab(pModule, m_Linker, pOutput);

    writeELF32Header(m_Linker.getLDInfo(),
                     pModule,
                     m_Linker.getLayout(),
                     pOutput);

    emitELF32ProgramHeader(pOutput);

    emitELF32SectionHeader(pModule, m_Linker.getLDInfo(), m_Linker, pOutput);
  }
  else if (64 == target().bitclass()) {
    // Write out ELF header
    // Write out section header table
    emitELF64ShStrTab(pModule, m_Linker, pOutput);

    writeELF64Header(m_Linker.getLDInfo(),
                     pModule,
                     m_Linker.getLayout(),
                     pOutput);

    emitELF64ProgramHeader(pOutput);

    emitELF64SectionHeader(pModule, m_Linker.getLDInfo(), m_Linker, pOutput);
  }
  else
    return make_error_code(errc::not_supported);

  pOutput.clear();
  return llvm::make_error_code(llvm::errc::success);
}

