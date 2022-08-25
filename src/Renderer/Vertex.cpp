#include <Renderer/Vertex.hpp>

namespace Nth {
	VertexInputDescription Vertex::getVertexDescription() {
		VertexInputDescription description;

		VkVertexInputBindingDescription mainBinding = {};
		mainBinding.binding = 0;
		mainBinding.stride = sizeof(Vertex);
		mainBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		description.bindings.push_back(mainBinding);

		VkVertexInputAttributeDescription positionAttribute = {};
		positionAttribute.binding = 0;
		positionAttribute.location = 0;
		positionAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		positionAttribute.offset = 0;
		
		VkVertexInputAttributeDescription textureAttribute = {};
		textureAttribute.binding = 0;
		textureAttribute.location = 1;
		textureAttribute.format = VK_FORMAT_R32G32_SFLOAT;
		textureAttribute.offset = offsetof(Vertex, texturePos);

		VkVertexInputAttributeDescription normalAttribute = {};
		normalAttribute.binding = 0;
		normalAttribute.location = 2;
		normalAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		normalAttribute.offset = offsetof(Vertex, normal);

		description.attributes.push_back(positionAttribute);
		description.attributes.push_back(textureAttribute);
		description.attributes.push_back(normalAttribute);

		return description;
	}
}