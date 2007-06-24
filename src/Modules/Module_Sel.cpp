// This file is part of RLVM, a RealLive virtual machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2007 Elliot Glaysher
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//  
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//  
// -----------------------------------------------------------------------

#include "Precompiled.hpp"

// -----------------------------------------------------------------------

#include "Modules/Module_Sel.hpp"
#include "Modules/cp932toUnicode.hpp"

#include "MachineBase/RLMachine.hpp"
#include "MachineBase/RLOperation.hpp"

#include <vector>
#include <iostream>

using std::cin;
using std::cerr;
using std::endl;
using std::vector;
using libReallive::SelectElement;
using libReallive::CommandElement;

// -----------------------------------------------------------------------

struct Sel_select : public RLOp_SpecialCase
{
  void operator()(RLMachine& machine, const CommandElement& ce)
  {
    cerr << "-----------SELECT!!!-----------" << endl;
    
    const SelectElement& element = dynamic_cast<const SelectElement&>(ce);
    cerr << "We have " << element.param_count() << " options!" << endl;

    const vector<SelectElement::Param>& params = element.getRawParams();
    for(int i = 0; i < params.size(); ++i)
    {
      std::string utf8str = cp932toUTF8(params[i].text, 
                                        machine.getTextEncoding());

      cerr << params[i].line << ": " << utf8str << endl;
    }

    int retVal;
    cerr << "> ";
    cin >> retVal;

    machine.setStoreRegister(retVal);
    machine.advanceInstructionPointer();
  }
};

// -----------------------------------------------------------------------

SelModule::SelModule()
  : RLModule("Sel", 0, 2)
{
  addOpcode(1, 0, new Sel_select);
}