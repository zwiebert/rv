/**
 * \file   rv/rv_zone.hh
 * \brief  Access some zone related settings
 */

#pragma once

#include <utils_misc/cstring_utils.hh>
#include <rv/rv.hh>
#include <stddef.h>
#include <stdlib.h>


/// \brief  Class representing a single irrigation zone
class RvZone {
  int mLph = 0;
#ifdef USE_ZONE_NAME
  csu mName;
  //char *mName = NULL;
#endif
public:

  /// \brief  Get the configured liter per hour for this zone
  int getLph() const {
    return mLph ? mLph : ZONE_LPH_DEFAULT;
  }

  /// \brief Set liter per hour for this zone
  bool setLph(int lph) {
    mLph = lph;
    return true;
  }

#ifdef USE_ZONE_NAME
  /// \brief Get name of this zone.
  const char* getName() const {
    return mName;
  }

  /// \brief Set name of this zone.
  void setName(const char *name) {
    mName = name;
  }

#endif
};
