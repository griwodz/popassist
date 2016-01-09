#pragma once

#include <vector>
#include <cuda_runtime.h>

namespace popart {

class device_prop_t
{
    int _num_devices;
    std::vector<cudaDeviceProp*> _properties;
public:
    device_prop_t( bool output = false );
    ~device_prop_t( );

    void print( );
    void set( int n );
};

}; // namespace popart

