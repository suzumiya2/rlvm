// -*- Mode: C++; tab-width:2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi:tw=80:et:ts=2:sts=2
//
// -----------------------------------------------------------------------
//
// This file is part of RLVM, a RealLive virtual machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2006 Elliot Glaysher
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
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

#include "libReallive/gameexe.h"

#include "gtest/gtest.h"

#include "testUtils.hpp"
#include <iostream>

using namespace std;

TEST(GameexeUnit, ReadAllKeys) {
  Gameexe ini(locateTestCase("Gameexe_data/Gameexe.ini"));
  EXPECT_EQ(26, ini.size()) << "Wrong number of keys";
}

// Make sure #CAPTION exists and that we read its value correctly.
TEST(GameexeUnit, ReadsCaption) {
  Gameexe ini(locateTestCase("Gameexe_data/Gameexe.ini"));
  EXPECT_TRUE(ini("CAPTION").exists()) << "#CAPTION exists";
  EXPECT_EQ(string("Canon: A firearm"), ini("CAPTION").to_string())
      << "Wrong value for CAPTION";
}

// Make sure #RANDOM_KEY doesn't exist.
TEST(GameexeUnit, RandomKeyDoesntExist) {
  Gameexe ini(locateTestCase("Gameexe_data/Gameexe.ini"));
  EXPECT_FALSE(ini("RANDOM_KEY").exists()) << "#RANDOM_KEY does not exist";
}

// Test to_intVector() parsing.
TEST(GameexeUnit, IntVectorParsing) {
  Gameexe ini(locateTestCase("Gameexe_data/Gameexe.ini"));
  EXPECT_TRUE(ini("WINDOW_ATTR").exists()) << "#WINDOW_ATTR exists!";

  vector<int> ints = ini("WINDOW_ATTR").to_intVector();
  for(int i = 0; i < 5; ++i) {
    EXPECT_EQ(i + 1, ints.at(i));
  }
}

// Make sure operator() works with multiple keys...
TEST(GameexeUnit, MultipleKeys) {
  Gameexe ini(locateTestCase("Gameexe_data/Gameexe.ini"));
  EXPECT_EQ(1, ini("IMAGINE", "ONE"));
  EXPECT_EQ(2, ini("IMAGINE", "TWO"));
  EXPECT_EQ(3, ini("IMAGINE", "THREE"));
}

// Make sure GameexeInterpretObject chaining works correctly.
TEST(GameexeUnit, ChainingWorks) {
  Gameexe ini(locateTestCase("Gameexe_data/Gameexe.ini"));
  GameexeInterpretObject imagine = ini("IMAGINE");
  EXPECT_EQ(1, imagine("ONE"));
  EXPECT_EQ(2, imagine("TWO"));
  EXPECT_EQ(3, imagine("THREE"));
}

TEST(GameexeUnit, FilteringIterators) {
  Gameexe ini(locateTestCase("Gameexe_data/Gameexe.ini"));
  GameexeFilteringIterator it = ini.filtering_begin("IMAGINE");
  GameexeFilteringIterator end = ini.filtering_end();
  for(; it != end; ++it) {
    if(it->key() != "IMAGINE.ONE" &&
       it->key() != "IMAGINE.TWO" &&
       it->key() != "IMAGINE.THREE") {
      FAIL() << "Failed to filter keys in GameexeFilteringIterator. Has key "
             << it->key();
    }
  }
}

TEST(GameexeUnit, KeyParts) {
  Gameexe ini(locateTestCase("Gameexe_data/Gameexe.ini"));
  GameexeInterpretObject gio = ini("WINDOW.000.ATTR_MOD");
  vector<string> pieces = gio.key_parts();
  EXPECT_EQ(3, pieces.size());
  EXPECT_EQ("WINDOW", pieces[0]);
  EXPECT_EQ("000", pieces[1]);
  EXPECT_EQ("ATTR_MOD", pieces[2]);
}

// A demo on the Princess Bride Box Set does something weird with its \#DSTRACK
// keys. While all other games space the entries like this:
//
//   #DSTRACK = 00000000 - 99999999 - 00269364 = "BGM01"  = "BGM01"
//
// this game does not:
//
//   #DSTRACK=00000000-10998934-00000000="dcbgm000"="dcbgm000"
//
// and this runs afoul of the gameexe tokenization
// code. (00000000-10998934-00000000 is treated as a single token.)
//
// This test ensures that the gameexe parser can handle both.
TEST(GameexeUnit, DstrackRegression) {
  Gameexe ini(locateTestCase("Gameexe_data/Gameexe_tokenization.ini"));

  GameexeInterpretObject clannad = ini("CLANNADDSTRACK");
  EXPECT_EQ(0, clannad.getIntAt(0));
  EXPECT_EQ(99999999, clannad.getIntAt(1));
  EXPECT_EQ(269364, clannad.getIntAt(2));
  EXPECT_EQ("BGM01", clannad.getStringAt(3));
  EXPECT_EQ("BGM01", clannad.getStringAt(4));

  GameexeInterpretObject dc = ini("DCDSTRACK");
  EXPECT_EQ(0, dc.getIntAt(0));
  EXPECT_EQ(10998934, dc.getIntAt(1));
  EXPECT_EQ(0, dc.getIntAt(2));
  EXPECT_EQ("dcbgm000", dc.getStringAt(3));
  EXPECT_EQ("dcbgm000", dc.getStringAt(4));
}
