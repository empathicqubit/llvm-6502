//===-- Mos6502ELFObjectWriter.cpp - Mos6502 ELF Writer -----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/Mos6502FixupKinds.h"
#include "MCTargetDesc/Mos6502MCExpr.h"
#include "MCTargetDesc/Mos6502MCTargetDesc.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

namespace {
  class Mos6502ELFObjectWriter : public MCELFObjectTargetWriter {
  public:
    Mos6502ELFObjectWriter(bool Is64Bit, uint8_t OSABI)
      : MCELFObjectTargetWriter(Is64Bit, OSABI,
                                Is64Bit ?  ELF::EM_SPARCV9 : ELF::EM_SPARC,
                                /*HasRelocationAddend*/ true) {}

    ~Mos6502ELFObjectWriter() override {}

  protected:
    unsigned GetRelocType(const MCValue &Target, const MCFixup &Fixup,
                          bool IsPCRel) const override;

    bool needsRelocateWithSymbol(const MCSymbol &Sym,
                                 unsigned Type) const override;

  };
}

unsigned Mos6502ELFObjectWriter::GetRelocType(const MCValue &Target,
                                            const MCFixup &Fixup,
                                            bool IsPCRel) const {

  if (const Mos6502MCExpr *SExpr = dyn_cast<Mos6502MCExpr>(Fixup.getValue())) {
    if (SExpr->getKind() == Mos6502MCExpr::VK_Mos6502_R_DISP32)
      return ELF::R_SPARC_DISP32;
  }

  if (IsPCRel) {
    switch((unsigned)Fixup.getKind()) {
    default:
      llvm_unreachable("Unimplemented fixup -> relocation");
    case FK_Data_1:                  return ELF::R_SPARC_DISP8;
    case FK_Data_2:                  return ELF::R_SPARC_DISP16;
    case FK_Data_4:                  return ELF::R_SPARC_DISP32;
    case FK_Data_8:                  return ELF::R_SPARC_DISP64;
    case Mos6502::fixup_mos6502_call30:  return ELF::R_SPARC_WDISP30;
    case Mos6502::fixup_mos6502_br22:    return ELF::R_SPARC_WDISP22;
    case Mos6502::fixup_mos6502_br19:    return ELF::R_SPARC_WDISP19;
    case Mos6502::fixup_mos6502_pc22:    return ELF::R_SPARC_PC22;
    case Mos6502::fixup_mos6502_pc10:    return ELF::R_SPARC_PC10;
    case Mos6502::fixup_mos6502_wplt30:  return ELF::R_SPARC_WPLT30;
    }
  }

  switch((unsigned)Fixup.getKind()) {
  default:
    llvm_unreachable("Unimplemented fixup -> relocation");
  case FK_Data_1:                return ELF::R_SPARC_8;
  case FK_Data_2:                return ((Fixup.getOffset() % 2)
                                         ? ELF::R_SPARC_UA16
                                         : ELF::R_SPARC_16);
  case FK_Data_4:                return ((Fixup.getOffset() % 4)
                                         ? ELF::R_SPARC_UA32
                                         : ELF::R_SPARC_32);
  case FK_Data_8:                return ((Fixup.getOffset() % 8)
                                         ? ELF::R_SPARC_UA64
                                         : ELF::R_SPARC_64);
  case Mos6502::fixup_mos6502_hi22:  return ELF::R_SPARC_HI22;
  case Mos6502::fixup_mos6502_lo10:  return ELF::R_SPARC_LO10;
  case Mos6502::fixup_mos6502_h44:   return ELF::R_SPARC_H44;
  case Mos6502::fixup_mos6502_m44:   return ELF::R_SPARC_M44;
  case Mos6502::fixup_mos6502_l44:   return ELF::R_SPARC_L44;
  case Mos6502::fixup_mos6502_hh:    return ELF::R_SPARC_HH22;
  case Mos6502::fixup_mos6502_hm:    return ELF::R_SPARC_HM10;
  case Mos6502::fixup_mos6502_got22: return ELF::R_SPARC_GOT22;
  case Mos6502::fixup_mos6502_got10: return ELF::R_SPARC_GOT10;
  case Mos6502::fixup_mos6502_tls_gd_hi22:   return ELF::R_SPARC_TLS_GD_HI22;
  case Mos6502::fixup_mos6502_tls_gd_lo10:   return ELF::R_SPARC_TLS_GD_LO10;
  case Mos6502::fixup_mos6502_tls_gd_add:    return ELF::R_SPARC_TLS_GD_ADD;
  case Mos6502::fixup_mos6502_tls_gd_call:   return ELF::R_SPARC_TLS_GD_CALL;
  case Mos6502::fixup_mos6502_tls_ldm_hi22:  return ELF::R_SPARC_TLS_LDM_HI22;
  case Mos6502::fixup_mos6502_tls_ldm_lo10:  return ELF::R_SPARC_TLS_LDM_LO10;
  case Mos6502::fixup_mos6502_tls_ldm_add:   return ELF::R_SPARC_TLS_LDM_ADD;
  case Mos6502::fixup_mos6502_tls_ldm_call:  return ELF::R_SPARC_TLS_LDM_CALL;
  case Mos6502::fixup_mos6502_tls_ldo_hix22: return ELF::R_SPARC_TLS_LDO_HIX22;
  case Mos6502::fixup_mos6502_tls_ldo_lox10: return ELF::R_SPARC_TLS_LDO_LOX10;
  case Mos6502::fixup_mos6502_tls_ldo_add:   return ELF::R_SPARC_TLS_LDO_ADD;
  case Mos6502::fixup_mos6502_tls_ie_hi22:   return ELF::R_SPARC_TLS_IE_HI22;
  case Mos6502::fixup_mos6502_tls_ie_lo10:   return ELF::R_SPARC_TLS_IE_LO10;
  case Mos6502::fixup_mos6502_tls_ie_ld:     return ELF::R_SPARC_TLS_IE_LD;
  case Mos6502::fixup_mos6502_tls_ie_ldx:    return ELF::R_SPARC_TLS_IE_LDX;
  case Mos6502::fixup_mos6502_tls_ie_add:    return ELF::R_SPARC_TLS_IE_ADD;
  case Mos6502::fixup_mos6502_tls_le_hix22:  return ELF::R_SPARC_TLS_LE_HIX22;
  case Mos6502::fixup_mos6502_tls_le_lox10:  return ELF::R_SPARC_TLS_LE_LOX10;
  }

  return ELF::R_SPARC_NONE;
}

bool Mos6502ELFObjectWriter::needsRelocateWithSymbol(const MCSymbol &Sym,
                                                 unsigned Type) const {
  switch (Type) {
    default:
      return false;

    // All relocations that use a GOT need a symbol, not an offset, as
    // the offset of the symbol within the section is irrelevant to
    // where the GOT entry is. Don't need to list all the TLS entries,
    // as they're all marked as requiring a symbol anyways.
    case ELF::R_SPARC_GOT10:
    case ELF::R_SPARC_GOT13:
    case ELF::R_SPARC_GOT22:
    case ELF::R_SPARC_GOTDATA_HIX22:
    case ELF::R_SPARC_GOTDATA_LOX10:
    case ELF::R_SPARC_GOTDATA_OP_HIX22:
    case ELF::R_SPARC_GOTDATA_OP_LOX10:
      return true;
  }
}

MCObjectWriter *llvm::createMos6502ELFObjectWriter(raw_pwrite_stream &OS,
                                                 bool Is64Bit,
                                                 bool IsLittleEndian,
                                                 uint8_t OSABI) {
  MCELFObjectTargetWriter *MOTW = new Mos6502ELFObjectWriter(Is64Bit, OSABI);
  return createELFObjectWriter(MOTW, OS, IsLittleEndian);
}
