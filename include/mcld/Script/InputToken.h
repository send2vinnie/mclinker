//===- InputToken.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SCRIPT_INPUT_TOKEN_INTERFACE_H
#define MCLD_SCRIPT_INPUT_TOKEN_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Script/StrToken.h>

namespace mcld
{

/** \class InputToken
 *  \brief This class defines the interfaces to a file/namespec token.
 */

class InputToken : public StrToken
{
public:
  enum Type {
    Unknown,
    File,
    NameSpec,
  };

protected:
  InputToken();
  InputToken(Type pType, const std::string& pName, bool pAsNeeded);

public:
  virtual ~InputToken();

  Type type() const { return m_Type; }

  bool asNeeded() const { return m_bAsNeeded; }

  static bool classof(const StrToken* pToken)
  {
    return pToken->kind() == StrToken::Input;
  }

private:
  Type m_Type;
  bool m_bAsNeeded;
};

} // namepsace of mcld

#endif
