#include "Bsp_dma.hpp"
#include <stdlib.h>


void *Dma::ram_alloc(size_t _size)
{
    constexpr size_t alignment = 4;
    _size = (_size + alignment - 1) & ~(alignment - 1); // 统一处理对齐

#ifdef SOC_DMA_RAM
    void *ptr = aligned_alloc(4, _size); // 分配对齐内存
    return ptr;
#endif
#ifdef SOC_DMA_SRAM
    uint8_t *_ramDmaStart = (uint8_t *)SOC_DMA_SRAM;
    uint8_t *_ramDmaEnd = (uint8_t *)SOC_DMA_SRAM_END;
    static uint8_t *dmaHeapPtr = (uint8_t *)_ramDmaStart;
    uint8_t *ptr = NULL;
    if ((dmaHeapPtr + _size) <= (uint8_t *)_ramDmaEnd) {
        ptr = dmaHeapPtr;
        dmaHeapPtr += _size;
    }
    return ptr;
#endif
    return nullptr;
}

void Dma::ram_free(void *_ptr) { free(_ptr); }
