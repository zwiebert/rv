/*
 * status_output.h
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */

#ifndef USER_MAIN_STATUS_OUTPUT_H_
#define USER_MAIN_STATUS_OUTPUT_H_

#include <stdint.h>
#include <stdbool.h>
#include "uout/uo_types.h"
#include "so_msg.h"
#include "so_config.h"
#include "so_types.h"

#ifdef __cplusplus


typedef void (*voidFuncP)(const struct TargetDesc &td);

template<class T>
class so_object {
private:
  const struct TargetDesc *my_td;
  voidFuncP mEnd;
public:
  so_object(void (*begin)(const struct TargetDesc &td, T), T args, voidFuncP end, const struct TargetDesc &td) {
    my_td = &td;
    mEnd = end;
    begin(td, args);
  }
  ~so_object() {
    mEnd(*my_td);
  }
};

template<>
class so_object<void> {
private:
  const struct TargetDesc *my_td;
  voidFuncP mEnd;
public:
  so_object(voidFuncP begin, voidFuncP end, const struct TargetDesc &td)  {
    my_td = &td;
    mEnd = end;
    begin(td);
  }

  ~so_object() {
    mEnd(*my_td);
  }
};


#endif

#endif /* USER_MAIN_STATUS_OUTPUT_H_ */
