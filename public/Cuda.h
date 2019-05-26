#ifndef CUDA_H
#define CUDA_H

#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <curand.h>
#include <curand_kernel.h>

#include "../public/EEXIST.h"

//__global__
void UpdateSystems(struct System* SystemPtr, struct tube*** BufferPtr);
void StartSysUpdate(struct System* SystemPtr, struct tube*** BufferPtr);

__global__
void AddSystems(struct System* SystemPtr, struct tube*** BufferPtr);
void StartSysAdd(struct System* SystemPtr, struct tube*** BufferPtr);

#endif