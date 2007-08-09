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

#ifndef __Argc_T_hpp__
#define __Argc_T_hpp__

/** 
 * @ingroup RLOperationGroup
 *
 * Type struct that implements the argc concept. 
 *
 * This type struct can only be used as the last element in a type
 * definition. (This is not checked for at runtime; I'm not even sure
 * how I'd check this concept.) This type struct takes a type struct
 * as its parameter type, and then will accept a variable number of
 * items of that type.
 */
template<typename CON>
struct Argc_T {
  /// The output type of this type struct
  typedef std::vector<typename CON::type> type;

  /** Convert the incoming parameter objects into the resulting type.
   * Passes each parameter down to 
   */
  static type getData(RLMachine& machine, 
                      const boost::ptr_vector<libReallive::ExpressionPiece>& p,
                      unsigned int position);

  /// Parse the raw parameter string and put the results in ExpressionPiece
  static void parseParameters(unsigned int position,
                              const std::vector<std::string>& input,
                              boost::ptr_vector<libReallive::ExpressionPiece>& output);

  enum {
    isRealTypestruct = true,
    isComplex = false
  };
};

// -----------------------------------------------------------------------

template<typename CON>
struct Argc_T<CON>::type Argc_T<CON>::getData(RLMachine& machine, 
                     const boost::ptr_vector<libReallive::ExpressionPiece>& p,
                     unsigned int position) {
  type returnVector;
  for(unsigned int i = position; i < p.size(); ++i)
    returnVector.push_back(CON::getData(machine, p, i));

  return returnVector;
}

// -----------------------------------------------------------------------

template<typename CON>
void Argc_T<CON>::
parseParameters(unsigned int position,
                const std::vector<std::string>& input,
                boost::ptr_vector<libReallive::ExpressionPiece>& output)
{
  for(unsigned int i = position; i < input.size(); ++i) {
    CON::parseParameters(i, input, output);
  }
}

#endif
