//
// Created by Novak on 19/06/2019.
//

#include <eagle/renderer/vulkan/VulkanConverter.h>
#include "VulkanTexture.h"
#include "VulkanHelper.h"

namespace eagle {

VulkanTexture::VulkanTexture(const TextureCreateInfo &textureCreateInfo,
                                 const VulkanTextureCreateInfo &nativeCreateInfo) :
    Texture(textureCreateInfo),
    m_nativeCreateInfo(nativeCreateInfo) {
    EG_CORE_TRACE("Constructing a vulkan texture!");

    VulkanImageCreateInfo nativeImageCreateInfo = {};
    nativeImageCreateInfo.device = nativeCreateInfo.device;
    nativeImageCreateInfo.physicalDevice = nativeCreateInfo.physicalDevice;
    nativeImageCreateInfo.commandPool = nativeCreateInfo.commandPool;
    nativeImageCreateInfo.graphicsQueue = nativeCreateInfo.graphicsQueue;
    nativeImageCreateInfo.imageCount = nativeCreateInfo.imageCount;

    m_image = std::make_shared<VulkanImage>(textureCreateInfo.imageCreateInfo, nativeImageCreateInfo);
    create();
    EG_CORE_TRACE("Vulkan texture constructed!");
}

VulkanTexture::~VulkanTexture() {
    EG_CORE_TRACE("Destroying a vulkan texture!");
    clear();
    EG_CORE_TRACE("Vulkan texture destroyed!");
}

void VulkanTexture::create() {
    EG_CORE_TRACE("Creating a vulkan texture!");
    VK_CALL VulkanHelper::create_image_sampler(
            m_nativeCreateInfo.device,
            m_sampler,
            VK_SAMPLER_ADDRESS_MODE_REPEAT,
            VulkanConverter::to_vk(m_createInfo.filter)
    );
    EG_CORE_TRACE("Vulkan texture created!");
}

void VulkanTexture::clear() {
    EG_CORE_TRACE("Clearing a vulkan texture!");
    if (m_sampler){
        VK_CALL vkDestroySampler(m_nativeCreateInfo.device, m_sampler, nullptr);
    }
    m_image.reset();
    EG_CORE_TRACE("Vulkan texture cleared!");
}

void VulkanTexture::resize(uint32_t width, uint32_t height) {
    EG_CORE_TRACE("Resizing a vulkan texture!");
    clear();
    m_image->resize(width, height);
    create();
    EG_CORE_TRACE("Vulkan texture resized!");
}

DescriptorType VulkanTexture::type() const {
    return DescriptorType::COMBINED_IMAGE_SAMPLER;
}

}
