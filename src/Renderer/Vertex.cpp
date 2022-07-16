#include "Renderer/Vertex.hpp"

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
		positionAttribute.format = VK_FORMAT_R32G32B32A32_SFLOAT;
		positionAttribute.offset = 0;

		VkVertexInputAttributeDescription colorAttribute = {};
		colorAttribute.binding = 0;
		colorAttribute.location = 1;
		colorAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
		colorAttribute.offset = sizeof(float) * 4;
		
		VkVertexInputAttributeDescription textureAttribute = {};
		textureAttribute.binding = 0;
		textureAttribute.location = 2;
		textureAttribute.format = VK_FORMAT_R32G32_SFLOAT;
		textureAttribute.offset = sizeof(float) * 7;

		description.attributes.push_back(positionAttribute);
		description.attributes.push_back(colorAttribute);
		description.attributes.push_back(textureAttribute);

		return description;
	}
}