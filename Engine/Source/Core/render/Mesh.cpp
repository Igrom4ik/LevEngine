#include "Core/render/Mesh.hpp"
#include "Core/graphics/GraphicsAPI.hpp"
#include "Core/Engine.hpp"
#include <GL/glew.h>
#include <stdexcept>


namespace LEN {
	Mesh::Mesh(const VertexLayout &layout, const std::vector<float> &vertices, const std::vector<uint32_t> &indices) {
		m_vertexLayout = layout;

		auto &graphicsAPI = Engine::GetInstance().GetGraphicsAPI(); // Get GraphicsAPI instance

		// Create and upload vertex/index buffers using GraphicsAPI
		m_VBO = graphicsAPI.CreateVertexBuffer(vertices);
		m_EBO = graphicsAPI.CreateIndexBuffer(indices);

		glGenVertexArrays(1, &m_VAO); // Generate VAO
		glBindVertexArray(m_VAO); // Bind VAO

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO); // Bind VBO

		for (auto &element: m_vertexLayout.elements) {
			glVertexAttribPointer(
				element.index,
				static_cast<GLint>(element.size),
				element.type,
				GL_FALSE,
				static_cast<GLsizei>(m_vertexLayout.stride),
				reinterpret_cast<void *>(static_cast<uintptr_t>(element.offset))
			);
			glEnableVertexAttribArray(element.index);
		}

		// Bind element array buffer while VAO is bound so EBO becomes part of VAO state
		if (m_EBO != 0) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		}

		if (m_vertexLayout.stride == 0) {
			std::cerr << "Mesh::Mesh(): vertex layout stride is 0 — invalid mesh layout" << std::endl;
			throw std::runtime_error("Invalid vertex layout stride");
		}

		// Set 0 for Buffer's
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		// Do not unbind GL_ELEMENT_ARRAY_BUFFER here — element buffer binding is part of VAO state

		// vertices.size() returns number of floats; m_vertexLayout.stride is in bytes.
		// Convert float count to bytes before dividing by stride to get vertex count.
		m_vertexCount = (vertices.size() * sizeof(float)) / static_cast<size_t>(m_vertexLayout.stride);
		m_indexCount = indices.size();
	}

	Mesh::Mesh(const VertexLayout &layout, const std::vector<float> &vertices) {
		m_vertexLayout = layout;

		auto &graphicsAPI = Engine::GetInstance().GetGraphicsAPI(); // Get GraphicsAPI instance

		// Create and upload vertex/index buffers using GraphicsAPI
		m_VBO = graphicsAPI.CreateVertexBuffer(vertices);

		glGenVertexArrays(1, &m_VAO); // Generate VAO
		glBindVertexArray(m_VAO); // Bind VAO

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO); // Bind VBO

		for (auto &element: m_vertexLayout.elements) {
			glVertexAttribPointer(
				element.index,
				static_cast<GLint>(element.size),
				element.type,
				GL_FALSE,
				static_cast<GLsizei>(m_vertexLayout.stride),
				reinterpret_cast<void *>(static_cast<uintptr_t>(element.offset))
			);
			glEnableVertexAttribArray(element.index);
		}
		// Set 0 for Buffer's
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// vertices.size() returns number of floats; m_vertexLayout.stride is in bytes.
		// Convert float count to bytes before dividing by stride to get vertex count.
		m_vertexCount = (vertices.size() * sizeof(float)) / static_cast<size_t>(m_vertexLayout.stride);
	}

	bool Mesh::IsGPUReady() const {
		// GPU ready if VAO and VBO exist (non-zero). EBO may be zero for non-indexed meshes.
		return m_VAO != 0 && m_VBO != 0;
	}

	void Mesh::Bind() {
		if (!IsGPUReady()) {
			std::cerr << "Mesh::Bind(): Attempt to bind mesh with uninitialized GPU resources - VAO=" << m_VAO <<
					" VBO=" << m_VBO << " EBO=" << m_EBO << std::endl;
			return;
		}
		glBindVertexArray(m_VAO);
		std::cerr << "Mesh::Bind() VAO=" << m_VAO << " VBO=" << m_VBO << " EBO=" << m_EBO
				<< " verts=" << m_vertexCount << " idx=" << m_indexCount << std::endl;
	}

	void Mesh::Draw() {
		if (!IsGPUReady()) {
			std::cerr << "Mesh::Draw(): Attempt to draw mesh with uninitialized GPU resources - VAO=" << m_VAO <<
					" VBO=" << m_VBO << " EBO=" << m_EBO << std::endl;
			return;
		}

		// Clear previous GL errors
		while (glGetError() != GL_NO_ERROR) {
		}

		// Validate OpenGL objects at runtime (driver may have invalidated them)
		GLboolean vaoOk = glIsVertexArray(m_VAO);
		GLboolean vboOk = glIsBuffer(m_VBO);
		GLboolean eboOk = m_EBO == 0 ? GL_TRUE : glIsBuffer(m_EBO);
		std::cerr << "Mesh::Draw() validation: vaoOk=" << vaoOk << " vboOk=" << vboOk << " eboOk=" << eboOk << " (VAO="
				<< m_VAO << ", VBO=" << m_VBO << ", EBO=" << m_EBO << ")" << std::endl;
		if (!vaoOk || !vboOk || !eboOk) {
			std::cerr << "Mesh::Draw(): OpenGL objects invalid - skipping draw" << std::endl;
			return;
		}

		std::cerr << "Mesh::Draw() called. VAO=" << m_VAO << " idxCount=" << m_indexCount << " vertCount=" <<
				m_vertexCount << std::endl;
		if (m_indexCount > 0) {
			glBindVertexArray(m_VAO);
			if (m_EBO != 0) {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
			}
			// final safety: ensure no GL error before draw
			GLenum pre = glGetError();
			if (pre != GL_NO_ERROR) {
				std::cerr << "Mesh::Draw(): pre-draw GL error: " << pre << std::endl;
				return;
			}
			glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_INT, 0);
		} else {
			glBindVertexArray(m_VAO);
			GLenum pre = glGetError();
			if (pre != GL_NO_ERROR) {
				std::cerr << "Mesh::Draw(): pre-draw GL error: " << pre << std::endl;
				return;
			}
			glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_vertexCount));
		}
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			std::cerr << "OpenGL error after draw: " << err << std::endl;
		}
	}

	std::shared_ptr<Mesh> Mesh::CreateQube() {
		std::vector<float> vertices = {
			// Front face (z = 0.5)
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,

			// Top face (y = 0.5)
			0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

			// Right face (x = 0.5)
			0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

			// Left face (x = -0.5)
			-0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,

			// Bottom face (y = -0.5)
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f,
			-0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f,
			0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f,

			// Back face (z = -0.5)
			0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
			0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f
		};

		std::vector<uint32_t> indices = {
			// Front face
			0, 1, 2,
			0, 2, 3,
			// Top face
			4, 5, 6,
			4, 6, 7,
			// Right face
			8, 9, 10,
			8, 10, 11,
			// Left face
			12, 13, 14,
			12, 14, 15,
			// Back face
			16, 17, 18,
			16, 18, 19,
			// Bottom face
			20, 21, 22,
			20, 22, 23
		};

		LEN::VertexLayout vertexLayout;
		// Build vertex layout programmatically to ensure offsets and stride are consistent
		{
			VertexElement el;
			el.type = GL_FLOAT;

			el.index = VertexElement::PositionIndex;
			el.size = 3; // x,y,z
			el.offset = vertexLayout.stride;
			vertexLayout.stride += el.size * sizeof(float);
			vertexLayout.elements.push_back(el);

			el.index = VertexElement::ColorIndex;
			el.size = 3; // r,g,b
			el.offset = vertexLayout.stride;
			vertexLayout.stride += el.size * sizeof(float);
			vertexLayout.elements.push_back(el);

			el.index = VertexElement::UVIndex;
			el.size = 2; // u,v
			el.offset = vertexLayout.stride;
			vertexLayout.stride += el.size * sizeof(float);
			vertexLayout.elements.push_back(el);

			el.index = VertexElement::NormalIndex;
			el.size = 3; // normal x,y,z (was incorrectly 2)
			el.offset = vertexLayout.stride;
			vertexLayout.stride += el.size * sizeof(float);
			vertexLayout.elements.push_back(el);
		}

		auto result = std::make_shared<LEN::Mesh>(vertexLayout, vertices, indices);

		return result;
	}

#if 0
	std::shared_ptr<Mesh> Mesh::Load(const std::string &path) {
		auto contents = Engine::GetInstance().GetFileSystem().LoadAssetFile(path);
		if (contents.empty()) {
			return nullptr;
		}

		auto readFloats = [](const cgltf_accessor *acc, cgltf_size i, float *out, int n)
				// read n floats from accessor at index i
		{
			std::fill(out, out + n, 0.0f); // default to 0
			return cgltf_accessor_read_float(acc, i, out, n) == 1;
		};

		auto readIndex = [](const cgltf_accessor *acc, cgltf_size i) {
			cgltf_uint out = 0;
			cgltf_bool ok = cgltf_accessor_read_uint(acc, i, &out, 1);
			return ok ? static_cast<uint32_t>(out) : 0;
		};

		cgltf_options options = {};
		cgltf_data *data = nullptr;
		cgltf_result res = cgltf_parse(&options, contents.data(), contents.size(), &data);
		if (res != cgltf_result_success) {
			return nullptr;
		}
		auto fullPath = Engine::GetInstance().GetFileSystem().GetAssetsFolder() / path;

		res = cgltf_load_buffers(&options, data, fullPath.remove_filename().string().c_str());
		if (res != cgltf_result_success) {
			cgltf_free(data);
			return nullptr;
		}

		std::shared_ptr<Mesh> result = nullptr;
		for (cgltf_size mi = 0; mi < data->meshes_count; ++mi) {
			auto mesh = data->meshes[mi];
			for (cgltf_size pi = 0; pi < mesh.primitives_count; ++pi) {
				auto &primitive = mesh.primitives[pi];
				if (primitive.type != cgltf_primitive_type_triangles) {
					continue; // only triangles supported
				}

				VertexLayout vertexLayout;
				cgltf_accessor *accessors[4] = {nullptr, nullptr, nullptr}; // position, normal, uv

				for (cgltf_size ai = 0; ai < primitive.attributes_count; ++ai) {
					auto &attr = primitive.attributes[ai];
					auto acc = attr.data;
					if (!acc) {
						continue;
					}
					VertexElement element;
					element.type = GL_FLOAT;

					switch (attr.type) {
						case cgltf_attribute_type_position: {
							accessors[VertexElement::PositionIndex] = acc;
							element.index = VertexElement::PositionIndex;
							element.size = 3; // x,y,z
						}
						break;
						case cgltf_attribute_type_color: {
							if (attr.index != 0) {
								continue; // only support first color set
							}
							accessors[VertexElement::ColorIndex] = acc;
							element.index = VertexElement::ColorIndex;
							element.size = 3;
						}
						break;
						case cgltf_attribute_type_texcoord: {
							if (attr.index != 0) {
								continue; // only support first color set
							}
							accessors[VertexElement::UVIndex] = acc;
							element.index = VertexElement::UVIndex;
							element.size = 2;
						}
						break;
						case cgltf_attribute_type_normal: {
							accessors[VertexElement::NormalIndex] = acc;
							element.index = VertexElement::NormalIndex;
							element.size = 3;
						}
						break;
						default:
							continue;
					}
					if (element.size > 0) {
						element.offset = vertexLayout.stride;
						vertexLayout.stride += element.size * sizeof(float);
						vertexLayout.elements.push_back(element);
					}
				}

				if (!accessors[VertexElement::PositionIndex]) {
					continue; // position is required
				}
				auto vertexCount = accessors[VertexElement::PositionIndex]->count;

				std::vector<float> vertices;
				vertices.resize((vertexLayout.stride / sizeof(float)) * vertexCount);

				for (cgltf_size vi = 0; vi < vertexCount; ++vi) {
					for (auto &el: vertexLayout.elements) {
						if (!accessors[el.index]) {
							continue;
						}
						auto index = (vi * vertexLayout.stride + el.offset) / sizeof(float);
						float *outFata = &vertices[index];
						readFloats(accessors[el.index], vi, outFata, el.size);
					}
				}

				if (primitive.indices) {
					auto indexCount = primitive.indices->count;
					std::vector<uint32_t> indices(indexCount);
					for (cgltf_size i = 0; i < indexCount; ++i) {
						indices[i] = readIndex(primitive.indices, i);
					}
					result = std::make_shared<Mesh>(vertexLayout, vertices, indices);
				} else {
					result = std::make_shared<Mesh>(vertexLayout, vertices);
				}

				if (result) {
					break;
				}
			}
			if (result) {
				break;
			}
		}
		cgltf_free(data);

		return result;
	}
#endif // 0
} // namespace LEN
