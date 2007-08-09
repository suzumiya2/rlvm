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

#ifndef __Special_T_hpp__
#define __Special_T_hpp__

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/tuple/tuple.hpp>

#include "libReallive/bytecode_fwd.h"
#include "libReallive/expression.h"

/** 
 * Type definition that implements the special parameter concept; the
 * way to expect multiple different types in a parameter slot. 
 */
template<typename A, typename B = Empty_T, typename C = Empty_T, 
         typename D = Empty_T, typename E = Empty_T>
struct Special_T {
  /// Internal unionish structure which we pass in to the 
  struct Parameter {
    // 0 = A, 1 = B
    int type;

    typename A::type first;
    typename B::type second;
    typename C::type third;
    typename D::type fourth;
    typename E::type fifth;
  };

  /// Export our internal struct as our external type
  typedef Parameter type;

  /// Convert the incoming parameter objects into the resulting type.
  static type getData(RLMachine& machine, 
                      const boost::ptr_vector<libReallive::ExpressionPiece>& p,
                      unsigned int position)
  {
    const libReallive::SpecialExpressionPiece& sp = 
      static_cast<const libReallive::SpecialExpressionPiece&>(p[position]);
    Parameter par;
    par.type = sp.getOverloadTag();
    switch(par.type) {
    case 0:
      par.first = A::getData(machine, sp.getContainedPieces(), 0);
      break;
    case 1:
      par.second = B::getData(machine, sp.getContainedPieces(), 0);
      break;
    case 2:
      par.third = C::getData(machine, sp.getContainedPieces(), 0);
      break;
    case 3:
      par.fourth = D::getData(machine, sp.getContainedPieces(), 0);
      break;
    case 4:
      par.fifth = E::getData(machine, sp.getContainedPieces(), 0);
      break;
    default:
      throw rlvm::Exception("Illegal overload in Special2_T::getData()");
    };

    return par;
  }

  static void parseParameters(unsigned int position,
                              const std::vector<std::string>& input,
                              boost::ptr_vector<libReallive::ExpressionPiece>& output)
  {
	const char* data = input.at(position).c_str();
	std::auto_ptr<libReallive::ExpressionPiece> ep(libReallive::get_data(data));

	output.push_back(ep.release());
  }

  enum {
    isRealTypestruct = true,
    isComplex = false
  };
};

#endif
