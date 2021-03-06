#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include <cuda_runtime.h>

namespace popart {

/*************************************************************
 * Global switch to use exclusively synchronous CUDA calls
 * for everything using the debug APIs.
 * Terribly slow but helps debugging.
 *************************************************************/

void pop_cuda_only_sync_calls( bool on );

/*************************************************************
 * Group: warning and error messages
 *************************************************************/
#define POP_INFO(s)

#define POP_INFO2(silent,s) \
    if (not silent) { \
        std::cerr << __FILE__ << ":" << __LINE__ << std::endl << "    " << s << std::endl; \
    }

#define POP_FATAL(s) { \
        std::cerr << __FILE__ << ":" << __LINE__ << std::endl << "    " << s << std::endl; \
        exit( -__LINE__ ); \
    }

#define POP_FATAL_FL(s,file,line) { \
        std::cerr << file << ":" << line << std::endl << "    " << s << std::endl; \
        exit( -__LINE__ ); \
    }

#define POP_CHECK_NON_NULL(ptr,s) if( ptr == 0 ) { POP_FATAL(s); }

#define POP_CHECK_NON_NULL_FL(ptr,s,file,line) if( ptr == 0 ) { POP_FATAL_FL(s,file,line); }

#define POP_CUDA_FATAL_FL(err,s,file,line) { \
        std::cerr << file << ":" << line << std::endl; \
        std::cerr << "    " << s << cudaGetErrorString(err) << std::endl; \
        exit( -__LINE__ ); \
    }

#define POP_CUDA_FATAL(err,s) POP_CUDA_FATAL_FL(err,s,__FILE__,__LINE__)

#define POP_CUDA_FATAL_TEST_FL(err,s,file,line) if( err != cudaSuccess ) { POP_CUDA_FATAL_FL(err,s,file,line); }

#define POP_CUDA_FATAL_TEST(err,s) if( err != cudaSuccess ) { POP_CUDA_FATAL(err,s); }

#define POP_CHK_CALL_IFSYNC pop_cuda_checkerror_ifsync( __FILE__, __LINE__ )

void pop_cuda_checkerror_ifsync( const char* file, size_t line );

void pop_check_last_error( const char* file,
                           size_t      line );
#define POP_CHK pop_check_last_error( __FILE__, __LINE__ )

void pop_sync_and_check_last_error( const char* file,
                                    size_t      line );
#define POP_SYNC_CHK pop_sync_and_check_last_error( __FILE__, __LINE__ )

void pop_info_gridsize( bool               silent,
                        dim3&              grid,
                        dim3&              block,
                        const std::string& kernel,
                        const char*        file,
                        size_t             line );
#define POP_INFO_GRIDSIZE(silent,grid,block,kernel) \
    pop_info_gridsize(silent,grid,block,kernel,__FILE__,__LINE__)

/*************************************************************
 * Group: memcpy
 *************************************************************/

namespace cuda {
namespace toHost {
void memcpy( void*          dst, const void* src,
             size_t         sz,
             const char*    file, size_t     line );
void memcpy( void*          dst, const void* src,
             size_t         sz,
             cudaStream_t   stream,
             const char*    file, size_t     line );
void memcpy2D( void*          dst,   size_t dpitch,
               const void*    src,   size_t spitch,
               size_t         width, size_t height,
               const char*    file,  size_t line );
void memcpy2D( void*          dst,   size_t dpitch,
               const void*    src,   size_t spitch,
               size_t         width, size_t height,
               cudaStream_t   stream,
               const char*    file,  size_t line );
} // namespace toHost

namespace toDev {
void memcpy( void*          dst, const void* src,
             size_t         sz,
             const char*    file,
             size_t         line );
void memcpy( void*          dst, const void* src,
             size_t         sz,
             cudaStream_t   stream,
             const char*    file,
             size_t         line );
void memcpy2D( void*          dst,   size_t dpitch,
               const void*    src,   size_t spitch,
               size_t         width, size_t height,
               const char*    file,  size_t line );
void memcpy2D( void*          dst,   size_t dpitch,
               const void*    src,   size_t spitch,
               size_t         width, size_t height,
               cudaStream_t   stream,
               const char*    file,  size_t line );
} // namespace toDev
} // namespace cuda

void pop_cuda_memcpy_to_symbol_async( const void*    symbol,
                                      const void*    src,
                                      size_t         sz,
                                      size_t         offset,
                                      cudaMemcpyKind type,
                                      cudaStream_t   stream,
                                      const char*    file,
                                      size_t         line );
void pop_cuda_memcpy_to_symbol_sync( const void*    symbol,
                                     const void*    src,
                                     size_t         sz,
                                     size_t         offset,
                                     cudaMemcpyKind type,
                                     const char*    file,
                                     size_t         line );

/*************************************************************
 * Group: memset
 *************************************************************/

void pop_cuda_memset_async( void*        ptr,
                            int          value,
                            size_t       bytes,
                            cudaStream_t stream,
                            const char*  file,
                            size_t       line );
template <typename T>
inline void pop_cuda_set0_async( T*           ptr,
                                 cudaStream_t stream,
                                 const char*  file,
                                 size_t       line )
{
    pop_cuda_memset_async( (void*)ptr, 0, sizeof(T), stream, file, line );
}
template <typename T>
inline void pop_cuda_setx_async( T*           ptr,
                                 T            val,
                                 cudaStream_t stream,
                                 const char*  file,
                                 size_t       line )
{
    pop_cuda_memcpy_async( (void*)ptr, &val, sizeof(T),
                           cudaMemcpyHostToDevice, stream, file, line );
}

void pop_cuda_memset_sync( void*        ptr,
                           int          value,
                           size_t       bytes,
                           const char*  file,
                           size_t       line );

/* async */
#define POP_CUDA_MEMSET_ASYNC( ptr, val, sz, stream ) \
    pop_cuda_memset_async( ptr, val, sz, stream, __FILE__, __LINE__ )

#define POP_CUDA_SET0_ASYNC( ptr, stream ) \
    pop_cuda_set0_async( ptr, stream, __FILE__, __LINE__ )

#define POP_CUDA_SETX_ASYNC( ptr, x, stream ) \
    pop_cuda_setx_async( ptr, x, stream, __FILE__, __LINE__ )

/* sync */
#define POP_CUDA_MEMSET_SYNC( ptr, val, sz ) \
    pop_cuda_memset_sync( ptr, val, sz, __FILE__, __LINE__ )

/*************************************************************
 * Group: memory allocation and release
 *************************************************************/

void pop_cuda_malloc( void**      ptr,
                      uint32_t    byte_size,
                      const char* file, uint32_t line );
void pop_cuda_malloc_pitch( void**      ptr,
                            size_t*     byte_pitch,
                            uint32_t    byte_width, uint32_t byte_height,
                            const char* file, uint32_t line );
void pop_cuda_free( void* ptr, const char* file, uint32_t line );
void pop_cuda_free_host( void* ptr, const char* file, uint32_t line );

#define POP_CUDA_MALLOC( ptr, byte_size ) \
    pop_cuda_malloc( ptr, byte_size, __FILE__, __LINE__ )

#define POP_CUDA_MALLOC_INIT0( ptr, sz ) { \
        POP_CUDA_MALLOC( ptr, sz ); \
        POP_CUDA_MEMSET_SYNC( ptr, 0, sz ); \
    }

#define POP_CUDA_MALLOC_NOINIT( ptr, sz ) \
    POP_CUDA_MALLOC( ptr, sz )

#define POP_CUDA_MALLOC_PITCH( ptr, byte_pitch, byte_width, byte_height ) \
    pop_cuda_malloc_pitch( ptr, byte_pitch, byte_width, byte_height, __FILE__, __LINE__ )

#define POP_CUDA_MALLOC_HOST( ptr, sz ) { \
        cudaError_t err; \
        err = cudaMallocHost( ptr, sz ); \
        POP_CUDA_FATAL_TEST( err, "cudaMallocHost failed: " ); \
    }

#define POP_CUDA_FREE( ptr ) \
    pop_cuda_free( ptr, __FILE__, __LINE__ )

#define POP_CUDA_FREE_HOST( ptr ) \
    pop_cuda_free_host( ptr, __FILE__, __LINE__ )

/*************************************************************
 * Group: CUDA stream handling
 *************************************************************/

void pop_cuda_stream_create( cudaStream_t* stream, const char* file, uint32_t line );
void pop_cuda_stream_destroy( cudaStream_t stream, const char* file, uint32_t line );
void pop_stream_synchronize( cudaStream_t stream,
                             const char*  file,
                             size_t       line );

#define POP_CUDA_STREAM_CREATE( stream ) \
    pop_cuda_stream_create( stream, __FILE__, __LINE__ )

#define POP_CUDA_STREAM_DESTROY( stream ) \
    pop_cuda_stream_destroy( stream, __FILE__, __LINE__ )

#define POP_CUDA_SYNC( stream ) \
    pop_stream_synchronize( stream, __FILE__, __LINE__ )

}; // namespace popart

