// -*- Mode: C++; tab-width:2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi:tw=80:et:ts=2:sts=2
//
// -----------------------------------------------------------------------
//
// This file is part of RLVM, a RealLive virtual machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2008 Elliot Glaysher
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
// -----------------------------------------------------------------------

#include "Platforms/gcn/GCNScrollArea.hpp"

#include "Platforms/gcn/gcnUtils.hpp"

ImageRect GCNScrollArea::s_background;
ImageRect GCNScrollArea::s_vMarker;
boost::scoped_ptr<gcn::Image> GCNScrollArea::s_buttonImages[4][2];

// BUGS:
//
// 1) Up/down scroll buttons need to be hold-downable

// -----------------------------------------------------------------------
// GCNScrollArea
// -----------------------------------------------------------------------

GCNScrollArea::GCNScrollArea(gcn::Widget *widget)
  : gcn::ScrollArea(widget) {
  init();
}

// -----------------------------------------------------------------------

GCNScrollArea::~GCNScrollArea() {
  // Correct?
  delete getContent();
}

// -----------------------------------------------------------------------

void GCNScrollArea::logic() {
  gcn::ScrollArea::logic();
  gcn::Widget *content = getContent();

  // When no scrollbar in a certain direction, adapt content size to match
  // the content dimension exactly.
  if (content != NULL) {
    if (getHorizontalScrollPolicy() == gcn::ScrollArea::SHOW_NEVER) {
      content->setWidth(getChildrenArea().width -
                        2 * content->getFrameSize());
    }
    if (getVerticalScrollPolicy() == gcn::ScrollArea::SHOW_NEVER) {
      content->setHeight(getChildrenArea().height -
                         2 * content->getFrameSize());
    }
  }
}

// -----------------------------------------------------------------------

void GCNScrollArea::draw(gcn::Graphics *graphics) {
  if (mVBarVisible) {
    drawUpButton(graphics);
    drawDownButton(graphics);
    drawVBar(graphics);
    drawVMarker(graphics);
  }

  if (mHBarVisible) {
    drawLeftButton(graphics);
    drawRightButton(graphics);
    drawHBar(graphics);
    drawHMarker(graphics);
  }

  if (mHBarVisible && mVBarVisible) {
    graphics->setColor(getBaseColor());
    graphics->fillRectangle(gcn::Rectangle(getWidth() - mScrollbarWidth,
                                           getHeight() - mScrollbarWidth,
                                           mScrollbarWidth,
                                           mScrollbarWidth));
  }

  drawChildren(graphics);
}

// -----------------------------------------------------------------------

void GCNScrollArea::drawFrame(gcn::Graphics *graphics) {
  int bs = getFrameSize();
  int w = getWidth() + bs * 2;
  int h = getHeight() + bs * 2;

  static_cast<GCNGraphics*>(graphics)->
    drawImageRect(0, 0, w, h, s_background);
}

// -----------------------------------------------------------------------

void GCNScrollArea::init() {
  if (s_background.image == NULL) {
    s_background.image.reset(getThemeImage(IMG_DEEPBOX));
    // TODO: +1 these values??
    int bggridx[] = {0, 3, 28, 31};
    int bggridy[] = {0, 3, 28, 31};
    s_background.setCoordinates(bggridx, bggridy);

    s_vMarker.image.reset(getThemeImage(IMG_VSCROLL_GREY));
    // TODO: +1 these values? Different +1 status then other set.
    int vsgridx[] = {0, 4, 7, 11};
    int vsgridy[] = {0, 4, 15, 19};
    s_vMarker.setCoordinates(vsgridx, vsgridy);

    s_buttonImages[UP][0].reset(getThemeImage(IMG_VSCROLL_UP_DEFAULT));
    s_buttonImages[DOWN][0].reset(getThemeImage(IMG_VSCROLL_DOWN_DEFAULT));
    s_buttonImages[LEFT][0].reset(getThemeImage(IMG_HSCROLL_LEFT_DEFAULT));
    s_buttonImages[RIGHT][0].reset(getThemeImage(IMG_HSCROLL_RIGHT_DEFAULT));
    s_buttonImages[UP][1].reset(getThemeImage(IMG_VSCROLL_UP_PRESSED));
    s_buttonImages[DOWN][1].reset(getThemeImage(IMG_VSCROLL_DOWN_PRESSED));
    s_buttonImages[LEFT][1].reset(getThemeImage(IMG_HSCROLL_LEFT_PRESSED));
    s_buttonImages[RIGHT][1].reset(getThemeImage(IMG_HSCROLL_RIGHT_PRESSED));
  }
}

// -----------------------------------------------------------------------

void GCNScrollArea::drawButton(gcn::Graphics *graphics, BUTTON_DIR dir) {
  int state = 0;
  gcn::Rectangle dim;

  switch (dir) {
    case UP:
      state = mUpButtonPressed ? 1 : 0;
      dim = getUpButtonDimension();
      break;
    case DOWN:
      state = mDownButtonPressed ? 1 : 0;
      dim = getDownButtonDimension();
      break;
    case LEFT:
      state = mLeftButtonPressed ? 1 : 0;
      dim = getLeftButtonDimension();
      break;
    case RIGHT:
      state = mRightButtonPressed ? 1 : 0;
      dim = getRightButtonDimension();
      break;
  }

  graphics->drawImage(s_buttonImages[dir][state].get(), dim.x, dim.y);
}

// -----------------------------------------------------------------------

void GCNScrollArea::drawUpButton(gcn::Graphics *graphics) {
  drawButton(graphics, UP);
}

// -----------------------------------------------------------------------

void GCNScrollArea::drawDownButton(gcn::Graphics *graphics) {
  drawButton(graphics, DOWN);
}

// -----------------------------------------------------------------------

void GCNScrollArea::drawLeftButton(gcn::Graphics *graphics) {
  drawButton(graphics, LEFT);
}

// -----------------------------------------------------------------------

void GCNScrollArea::drawRightButton(gcn::Graphics *graphics) {
  drawButton(graphics, RIGHT);
}

// -----------------------------------------------------------------------

void GCNScrollArea::drawVBar(gcn::Graphics *graphics) {
  gcn::Rectangle dim = getVerticalBarDimension();
  graphics->setColor(gcn::Color(0, 0, 0, 32));
  graphics->fillRectangle(dim);
  graphics->setColor(gcn::Color(255, 255, 255));
}

// -----------------------------------------------------------------------

void GCNScrollArea::drawHBar(gcn::Graphics *graphics) {
  gcn::Rectangle dim = getHorizontalBarDimension();
  graphics->setColor(gcn::Color(0, 0, 0, 32));
  graphics->fillRectangle(dim);
  graphics->setColor(gcn::Color(255, 255, 255));
}

// -----------------------------------------------------------------------

void GCNScrollArea::drawVMarker(gcn::Graphics *graphics) {
  gcn::Rectangle dim = getVerticalMarkerDimension();

  static_cast<GCNGraphics*>(graphics)->
    drawImageRect(dim.x, dim.y, dim.width, dim.height, s_vMarker);
}

// -----------------------------------------------------------------------

void GCNScrollArea::drawHMarker(gcn::Graphics *graphics) {
  gcn::Rectangle dim = getHorizontalMarkerDimension();

  static_cast<GCNGraphics*>(graphics)->
    drawImageRect(dim.x, dim.y, dim.width, dim.height, s_vMarker);
}
