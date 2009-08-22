// -*- Mode: C++; tab-width:2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi:tw=80:et:ts=2:sts=2
//
// -----------------------------------------------------------------------
//
// This file is part of RLVM, a RealLive virtual machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2006, 2007 Elliot Glaysher
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


#ifndef SRC_SYSTEMS_BASE_EVENTSYSTEM_HPP_
#define SRC_SYSTEMS_BASE_EVENTSYSTEM_HPP_

#include "Systems/Base/RLTimer.hpp"
#include "Systems/Base/Rect.hpp"

#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/utility.hpp>
#include <queue>
#include <set>

class RLMachine;

class Gameexe;
class FrameCounter;
class EventListener;

// -----------------------------------------------------------------------

struct EventSystemGlobals {
  EventSystemGlobals();
  EventSystemGlobals(Gameexe& gexe);

  /// The two generic values that the reallive game has control over
  /// with the Generic1 and Generic2 functions.
  int generic1, generic2;

  /// boost::serialization support
  template<class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    ar & generic1 & generic2;
  }
};

// -----------------------------------------------------------------------

/**
 * Generalization of an event system. Reallive's event model is a bit
 * weird; interpreted code will check the state of certain keyboard
 * modifiers, with functions such as CtrlPressed() or ShiftPressed().
 *
 * So what's the solution? Have two different event systems side by
 * side. One is exposed to Reallive and mimics what RealLive bytecode
 * expects. The other is based on event handlers and is sane.
 */
class EventSystem : public boost::noncopyable {
public:
  EventSystem(Gameexe& gexe);
  virtual ~EventSystem();

  /// Run once per cycle through the game loop to process events.
  virtual void executeEventSystem(RLMachine& machine);

  /**
   * Returns the number of milliseconds since the program
   * started. Used for timing things.
   */
  virtual unsigned int getTicks() const = 0;

  RLTimer& getTimer(int layer, int counter) { return timers_[layer][counter]; }

  /**
   * Idles the program for a certain amount of time in milliseconds.
   *
   * @param milliseconds Time
   */
  virtual void wait(unsigned int milliseconds) const = 0;

  // -----------------------------------------------------------------------

  /**
   * @name Frame Counters
   *
   * "Frame counters are designed to make it simple to ensure events happen at a
   * constant speed regardless of the host system's specifications. Once a frame
   * counter has been initialised, it will count from one arbitrary number to
   * another, over a given length of time. The counter can be queried at any
   * point to get its current value."
   *
   * Valid values for layer are 0 and 1. Valid values for frame_counter are 0
   * through 255.
   *
   * @{
   */
  void setFrameCounter(int layer, int frame_counter, FrameCounter* counter);
  FrameCounter& getFrameCounter(int layer, int frame_counter);
  bool frameCounterExists(int layer, int frame_counter);
  /// @}

  // -----------------------------------------------------------------------

  /**
   * @name Keyboard and Mouse Input (Event Listener style)
   *
   * rlvm event handling works by registering objects that received input
   * notifications from the EventSystem. These objects are EventListeners,
   * which passively listen for input and have a first chance grab at any click
   * or keypress.
   *
   * If no EventListener claims the event, then we try to reinterpret the top
   * of the RLMachine callstack as an EventListener. Otherwise, the events
   * are handled RealLive style (see below).
   *
   * @{
   */
  void addMouseListener(EventListener* listener);
  void removeMouseListener(EventListener* listener);
  /// @}

  /**
   * @name Keyboard and Mouse Input (Reallive style)
   *
   * RealLive applications poll for input, with all the problems that sort of
   * event handeling has. We therefore provide an interface for polling.
   *
   * Don't use it. This interface is provided for RealLive
   * bytecode. EventListeners should be used within rlvm code, instead.
   *
   * @{
   */
  /**
   * Returns whether shift is currently pressed.
   */
  virtual bool shiftPressed() const = 0;

  /**
   * Returns whether ctrl has been presed since the last invocation of
   * ctrlPresesd().
   *
   * @todo Do I have to keep track of if the key was pressed between
   * two ctrlPressed() invocations?
   */
  virtual bool ctrlPressed() const = 0;

  /**
   * Returns the current cursor hotspot.
   */
  virtual Point getCursorPos() { return Point(); }

  /**
   * Gets the location of the mouse cursor and the button states.
   *
   * The following values are used to indicate a button's status:
   * - 0 if unpressed
   * - 1 if being pressed
   * - 2 if pressed and released.
   */
  virtual void getCursorPos(Point& position, int& button1, int& button2) {}

  /**
   * Resets the state of the mouse buttons.
   */
  virtual void flushMouseClicks() {}

  /// @}

  // -----------------------------------------------------------------------

  /**
   * @name Generic values
   *
   * These values should have, from the beginning, been placed somewhere
   * else. They will remain here till the end of time for save game file
   * compatibility, though.
   *
   * "RealLive provides two generic settings to permit games using the standard
   * system command menu to include custom options in it. The meaning of each
   * generic flag is left up to the programmer. Valid values are 0 to 4."
   *
   * @{
   */
  void setGeneric1(const int in) { globals_.generic1 = in; }
  int generic1() const { return globals_.generic1; }

  void setGeneric2(const int in) { globals_.generic2 = in; }
  int generic2() const { return globals_.generic2; }
  /// @}

  // -----------------------------------------------------------------------

  /**
   * @name Testing
   *
   * Allows test systems like luaRlvm to inject mouse movement and clicks.
   *
   * @{
   */
  virtual void injectMouseMovement(RLMachine& machine, const Point& loc) = 0;
  virtual void injectMouseDown(RLMachine& machine) = 0;
  virtual void injectMouseUp(RLMachine& machine) = 0;
  /// @}

  // -----------------------------------------------------------------------

  EventSystemGlobals& globals() { return globals_; }

protected:
  typedef std::set<EventListener*> EventListeners;

  EventListeners::iterator listeners_begin() { return event_listeners_.begin(); }
  EventListeners::iterator listeners_end() { return event_listeners_.end(); }

  /**
   * Calls a EventListener member function on all event listeners, and then
   * event handlers, stopping when an object says they handled it.
   */
  void dispatchEvent(RLMachine& machine,
                     const boost::function<bool(EventListener&)>& event);
  void broadcastEvent(RLMachine& machine,
                      const boost::function<void(EventListener&)>& event);

private:
  boost::scoped_ptr<FrameCounter> frame_counters_[255][2];
  RLTimer timers_[255][2];

  /// Helper function that verifies input
  void checkLayerAndCounter(int layer, int counter);

  EventListeners event_listeners_;

  EventSystemGlobals globals_;
};

#endif  // SRC_SYSTEMS_BASE_EVENTSYSTEM_HPP_
