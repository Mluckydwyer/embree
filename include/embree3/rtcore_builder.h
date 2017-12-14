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

#include "rtcore_scene.h"

#if defined(__cplusplus)
extern "C" {
#endif
  
/* Defines an opaque BVH type */
typedef struct __RTCBVH* RTCBVH;

/* Input primitives for the builder. Stores primitive bounds and ID. */
struct RTCORE_ALIGN(32) RTCBuildPrimitive
{
  float lower_x, lower_y, lower_z;  // lower bounds in x/y/z
  int geomID;                       // first ID
  float upper_x, upper_y, upper_z;  // upper bounds in x/y/z
  int primID;                       // second ID
};

/* Defines an opaque thread local allocator type */
typedef struct __RTCThreadLocalAllocator* RTCThreadLocalAllocator;

/* Callback to create a node. */
typedef void* (*RTCCreateNodeFunction) (RTCThreadLocalAllocator allocator, unsigned int numChildren, void* userPtr);

/* Callback to set the pointer to all children. */
typedef void  (*RTCSetNodeChildrenFunction) (void* nodePtr, void** children, unsigned int numChildren, void* userPtr);

/* Callback to set the bounds of all children. */
typedef void  (*RTCSetNodeBoundsFunction) (void* nodePtr, const struct RTCBounds** bounds, unsigned int numChildren, void* userPtr);

/* Callback to create a leaf node. */
typedef void* (*RTCCreateLeafFunction) (RTCThreadLocalAllocator allocator, const struct RTCBuildPrimitive* prims, size_t numPrims, void* userPtr);

/* Callback to split a build primitive. */
typedef void  (*RTCSplitPrimitiveFunction) (const struct RTCBuildPrimitive* prim, unsigned int dim, float pos, struct RTCBounds* lbounds, struct RTCBounds* rbounds, void* userPtr);

/* Settings for builders */
struct RTCBuildSettings
{
  unsigned int size;             // Size of this structure in bytes. Makes future extension easier.
  enum RTCBuildQuality quality;   // quality of BVH build
  unsigned int maxBranchingFactor; // miximal branching factor of BVH to build
  unsigned int maxDepth;         // maximal depth of BVH to build
  unsigned int sahBlockSize;     // block size for SAH heuristic
  unsigned int minLeafSize;      // minimal size of a leaf
  unsigned int maxLeafSize;      // maximal size of a leaf
  float travCost;            // estimated cost of one traversal step
  float intCost;             // estimated cost of one primitive intersection
  unsigned int extraSpace;       // for spatial splitting we need extra space at end of primitive array

  RTCBVH bvh;                                     // BVH to build
  struct RTCBuildPrimitive* primitives;                  // list of input primitives
  size_t numPrimitives;                           // number of input primitives
  RTCCreateNodeFunction createNode;                   // creates a node
  RTCSetNodeChildrenFunction setNodeChildren;         // sets pointer to all children
  RTCSetNodeBoundsFunction setNodeBounds;             // sets bounds of all children
  RTCCreateLeafFunction createLeaf;                   // creates a leaf
  RTCSplitPrimitiveFunction splitPrimitive;           // splits a primitive
  RTCProgressMonitorFunction buildProgress;           // used to report build progress
  void* userPtr;                                   // user pointer passed to callback functions
};

/* Creates default build settings.  */
RTCORE_FORCEINLINE struct RTCBuildSettings rtcDefaultBuildSettings()
{
  struct RTCBuildSettings settings;
  settings.size = sizeof(settings);
  settings.quality = RTC_BUILD_QUALITY_MEDIUM;
  settings.maxBranchingFactor = 2;
  settings.maxDepth = 32;
  settings.sahBlockSize = 1;
  settings.minLeafSize = 1;
  settings.maxLeafSize = 32;
  settings.travCost = 1.0f;
  settings.intCost = 1.0f;
  settings.extraSpace = 0;
  settings.bvh = NULL;
  settings.primitives = NULL;
  settings.numPrimitives = 0;
  settings.createNode = NULL;
  settings.setNodeChildren = NULL;
  settings.setNodeBounds = NULL;
  settings.createLeaf = NULL;
  settings.splitPrimitive = NULL;
  settings.buildProgress = NULL;
  settings.userPtr = NULL;
  return settings;
}

/* Creates a new BVH. */
RTCORE_API RTCBVH rtcNewBVH(RTCDevice device);

/* builds the BVH */
RTCORE_API void* rtcBuildBVH(const struct RTCBuildSettings* settings);

/* Allocates memory using the thread local allocator. Use this function to allocate nodes in the callback functions. */
RTCORE_API void* rtcThreadLocalAlloc(RTCThreadLocalAllocator allocator, size_t bytes, size_t align);

/* Makes the BVH static. No further rtcBVHBuild can be called anymore on the BVH. */
RTCORE_API void rtcMakeStaticBVH(RTCBVH bvh);

/* Retains the BVH (increments reference count). */
RTCORE_API void rtcRetainBVH(RTCBVH bvh);

/* Releases the BVH (decrements reference count). */
RTCORE_API void rtcReleaseBVH(RTCBVH bvh);

#if defined(__cplusplus)
}
#endif
