// ======================================================================== //
// Copyright 2009-2017 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

#include "rtcore_device.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* Specifies the type of buffers */
enum RTCBufferType
{
  RTC_BUFFER_TYPE_INDEX = 0,
  RTC_BUFFER_TYPE_VERTEX = 1,
  RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE = 2,

  RTC_BUFFER_TYPE_FACE = 3,
  RTC_BUFFER_TYPE_LEVEL = 4,

  RTC_BUFFER_TYPE_EDGE_CREASE_INDEX = 5,
  RTC_BUFFER_TYPE_EDGE_CREASE_WEIGHT = 6,

  RTC_BUFFER_TYPE_VERTEX_CREASE_INDEX = 7,
  RTC_BUFFER_TYPE_VERTEX_CREASE_WEIGHT = 8,

  RTC_BUFFER_TYPE_HOLE = 9,

  RTC_BUFFER_TYPE_FLAGS = 10
};

/* Defines an opaque buffer type */
typedef struct __RTCBuffer* RTCBuffer;

/* Creates a new buffer. */
RTCORE_API RTCBuffer rtcNewBuffer(RTCDevice device, size_t byteSize);

/* Creates a new shared buffer. */
RTCORE_API RTCBuffer rtcNewSharedBuffer(RTCDevice device, void* ptr, size_t byteSize);

/* Returns a pointer to the buffer data. */
RTCORE_API void* rtcGetBufferData(RTCBuffer buffer);

/* Retains the buffer (increments reference count). */
RTCORE_API void rtcRetainBuffer(RTCBuffer buffer);

/* Releases the buffer (decrements reference count). */
RTCORE_API void rtcReleaseBuffer(RTCBuffer buffer);

#if defined(__cplusplus)
}
#endif
