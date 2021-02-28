//
// Created by Novak on 06/06/2019.
//

#include "eagle/Log.h"
#include "VulkanVertexBuffer.h"

namespace eagle {

VulkanVertexBuffer::VulkanVertexBuffer(VkDevice device, VulkanVertexBufferCreateInfo &createInfo,
                                       UpdateType usageFlags) :
        m_device(device),
        m_physicalDevice(createInfo.physicalDevice),
        m_layout(createInfo.vertexLayout),
        m_size(createInfo.size),
        m_buffers(createInfo.bufferCount),
        m_usage(usageFlags){


    VkDeviceSize bufferSize = m_size;

    if (m_size != 0){
        m_data = new char[bufferSize];
        memcpy(m_data, createInfo.data, bufferSize);
    }

    switch(usageFlags){
        case UpdateType::HOST:{

            if (m_size == 0) {
                return;
            }

            VulkanBufferCreateInfo createBufferInfo = {};
            createBufferInfo.memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
            createBufferInfo.usageFlags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            for (auto& buffer : m_buffers){
                VulkanBuffer::create_buffer(createInfo.physicalDevice, m_device, buffer, createBufferInfo, bufferSize);
                buffer->map();
                buffer->copy_to(m_data, bufferSize);
                buffer->flush();
            }

        }break;
        case UpdateType::CONSTANT:{
            VulkanBufferCreateInfo createBufferInfo = {};
            createBufferInfo.memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            createBufferInfo.usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

            std::shared_ptr<VulkanBuffer> stagingBuffer;
            VK_CALL
            VulkanBuffer::create_buffer(createInfo.physicalDevice, m_device, stagingBuffer, createBufferInfo,
                                        bufferSize, m_data);


            createBufferInfo.memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            createBufferInfo.usageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

            VK_CALL
            VulkanBuffer::create_buffer(createInfo.physicalDevice, m_device, m_buffers[0], createBufferInfo, bufferSize);

            VK_CALL
            VulkanBuffer::copy_buffer(
                    device,
                    createInfo.commandPool,
                    createInfo.graphicsQueue,
                    stagingBuffer->native_buffer(),
                    m_buffers[0]->native_buffer(),
                    bufferSize,
                    0);

            stagingBuffer->destroy();
        }break;
    }
    m_initialized = true;
}

VulkanVertexBuffer::~VulkanVertexBuffer() {
    for (auto& buffer : m_buffers){
        buffer->unmap();
        buffer->destroy();
    }
}

void
VulkanVertexBuffer::flush(uint32_t bufferIndex) {

    if (!is_dirty()){
        return;
    }

    VkDeviceSize bufferSize = m_size;

    if (m_resizing || !m_buffers[bufferIndex]){
        if (m_buffers[bufferIndex]){
            m_buffers[bufferIndex]->unmap();
            m_buffers[bufferIndex]->destroy();
        }


        VulkanBufferCreateInfo createBufferInfo = {};
        createBufferInfo.memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        createBufferInfo.usageFlags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        VulkanBuffer::create_buffer(m_physicalDevice, m_device, m_buffers[bufferIndex], createBufferInfo, bufferSize, m_data);
        m_buffers[bufferIndex]->map();
    }
    m_buffers[bufferIndex]->copy_to(m_data, bufferSize);
    m_buffers[bufferIndex]->flush();
    m_dirtyBuffers.erase(bufferIndex);
}

void VulkanVertexBuffer::upload(void *data, uint32_t size) {

    if (m_usage == UpdateType::CONSTANT) {
        EG_CORE_ERROR("Trying to flush data to a vertex buffer created with CONSTANT flag! To be able to flush data, be sure to create the buffer with DYNAMIC flag!");
        return;
    }

    VkDeviceSize bufferSize = size;

    if (m_data != nullptr){
        delete[](m_data);
    }

    m_data = new char[bufferSize];
    memcpy(m_data, data, bufferSize);

    if (!m_initialized){

        VulkanBufferCreateInfo createBufferInfo = {};
        createBufferInfo.memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        createBufferInfo.usageFlags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        for (auto& buffer : m_buffers){
            VulkanBuffer::create_buffer(m_physicalDevice, m_device, buffer, createBufferInfo, bufferSize);
            buffer->map();
            buffer->copy_to(m_data, bufferSize);
            buffer->flush();
        }
        m_initialized = true;
    }
    else{
        if (m_size != size){
            m_resizing = true;
        }

        m_dirtyBuffers.clear();
        for (int i = 0; i < m_buffers.size(); i++){
            m_dirtyBuffers.insert(i);
        }
        VulkanCleaner::push(this);
    }
    m_size = size;

}

uint32_t
VulkanVertexBuffer::get_vertices_count() {
    return m_size;
}

uint32_t
VulkanVertexBuffer::get_stride() {
    return m_layout.get_stride();
}

bool VulkanVertexBuffer::is_dirty() const {
    return !m_dirtyBuffers.empty();
}

}


