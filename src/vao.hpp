#ifndef VAO_HPP
#define VAO_HPP
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>
struct Vbo {
	const GLuint id;
	unsigned int index;
	GLuint dim;
	bool instanced = false;
	Vbo(const GLuint id, unsigned int index, GLuint dim, bool instanced = false)
		: id(id), index(index), dim(dim), instanced(instanced) {}
};
class Vao {
	GLuint id;
	std::vector<Vbo> vbos;
	std::optional<GLuint> indicesId;
	std::optional<long> instanceCount;
	long itemsCount = 0;
	template <typename T>
	unsigned int gen_vbo(unsigned int index, unsigned int stride, T *data,
						 unsigned int len);
	template <typename T>
	unsigned int gen_instanced_vbo(unsigned int index, unsigned int stride,
								   T *data, unsigned int len,
								   unsigned int divisor);
	template <typename T>
	unsigned int add_vertex_buffer(unsigned int dim, const T *data,
								   unsigned int len);
	template <typename T>
	unsigned int add_instanced_vertex_buffer(unsigned int dim, const T *data,
											 unsigned int len, int div = 1);
	template <typename T>
	void update_vbo(int index, const T *data, size_t start, unsigned int len);

	template <typename T>
	void update_vbo(int index, const T *data, unsigned int len);

   public:
	Vao() { glGenVertexArrays(1, &id); };
	void add_index_buffer(const unsigned int *data, size_t count);
	/**
   * Adds a Index Buffer to the Vao.
   * The Vao will be rendered according to if it has an index buffer or not.
   * @param indices index data to store
   */
	inline void add_index_buffer(std::vector<unsigned int> indices) {
		add_index_buffer(indices.data(), indices.size());
	}
	/**
   * Adds a vertex buffer to the Vao.
   * Only int and float datatypes are supported.
   * The index of this vbo is the count of vertex buffers already present.
   * @param dim   Dimension or stride of the vbo (e.g. 3 for a vec3)
   * @param data  Pointer to the data array
   * @param len   numbers of entries in the data array
   * @return the index of this vbo
   */
	unsigned int add_vertex_buffer(unsigned int dim, const float *data,
								   unsigned int len);
	/**
   * Adds a vertex buffer to the Vao.
   * Only int and float datatypes are supported.
   * The index of this vbo is the count of vertex buffers already present.
   * @param dim   Dimension or stride of the vbo (e.g. 3 for a vec3)
   * @param data  Pointer to the data array
   * @param len   numbers of entries in the data array
   * @return the index of this vbo
   */
	unsigned int add_vertex_buffer(unsigned int dim, const int *data,
								   unsigned int len);
	/**
   * Adds a vertex buffer to the Vao.
   * Only int and float datatypes are supported.
   * The index of this vbo is the count of vertex buffers already present.
   * @param dim   Dimension or stride of the vbo (e.g. 3 for a vec3)
   * @param data  the data array
   * @return the index of this vbo
   */
	template <typename T>
	unsigned int add_vertex_buffer(unsigned int dim, std::vector<T> data) {
		return add_vertex_buffer(dim, data.data(), data.size());
	}

	void set_instance_count(int count) { instanceCount = count; }
	/**
   * Adds an instanced vertex buffer to the Vao.
   * The index of this vbo is the count of vertex buffers already present.
   * Note: if you use instanced vbos and vaos, dont forget to set the count of
   * objects with 'setInstanceCount(int)', or your data will not be rendered
   * correctly. Only int and float datatypes are supported.
   * @param dim   Dimension or stride of the vbo (e.g. 3 for a ivec3)
   * @param data  Pointer to the data array
   * @param len  numbers of entries in the data array
   * @param div   Attribute Divisor Count, default is 1
   * @return the index of this vbo
   */
	unsigned int add_instanced_vertex_buffer(unsigned int dim,
											 const float *data,
											 unsigned int len, int div = 1);
	/**
   * Adds an instanced vertex buffer to the Vao.
   * The index of this vbo is the count of vertex buffers already present.
   * Note: if you use instanced vbos and vaos, dont forget to set the count of
   * objects with 'setInstanceCount(int)', or your data will not be rendered
   * correctly. Only int and float datatypes are supported.
   * @param dim   Dimension or stride of the vbo (e.g. 3 for a ivec3)
   * @param data  Pointer to the data array
   * @param len  numbers of entries in the data array
   * @param div   Attribute Divisor Count, default is 1
   * @return the index of this vbo
   */
	unsigned int add_instanced_vertex_buffer(unsigned int dim, const int *data,
											 unsigned int len, int div = 1);
	/**
   * Adds an instanced vertex buffer to the Vao.
   * The index of this vbo is the count of vertex buffers already present.
   * Note: if you use instanced vbos and vaos, dont forget to set the count of
   * objects with 'setInstanceCount(int)', or your data will not be rendered
   * correctly.
   * @param dim   Dimension or stride of the vbo (e.g. 3 for a ivec3)
   * @param data  the data array
   * @param div   Attribute Divisor Count, default is 1
   * @return the index of this vbo
   */
	template <class T>
	unsigned int add_instanced_vertex_buffer(unsigned int dim,
											 std::vector<T> data, int div = 1) {
		static_assert(std::is_same<T, float>() || std::is_same<T, int>(),
					  "Only float and int data is permitted in vbos!");
		return add_instanced_vertex_buffer(dim, data.data(), data.size(), div);
	}
	/**
   * Updates the data of an instanced vbo
   * @param index the index of this vbo in the vao
   * @param data  the data that should be stored in the vbo
   * @param start byte offset
   * @param len   the count of entries of the data array
   */
	void update_vbo(int index, const float *data, size_t start,
					unsigned int len);
	/**
   * Updates the data of an instanced vbo
   * @param index the index of this vbo in the vao
   * @param data  the data that should be stored in the vbo
   * @param start byte offset
   * @param len   the count of entries of the data array
   */
	void update_vbo(int index, const int *data, size_t start, unsigned int len);

	/**
   * Updates the data of an instanced vbo
   * @param index the index of this vbo i.e. the number at which this vbo has
   * been added to this vao
   * @param data  the data that should be stored in the vbo
   * @param len   the count of entries of the data array
   */
	void update_vbo(int index, const float *data, unsigned int len);

	/**
   * Updates the data of an instanced vbo
   * @param index the index of this vbo i.e. the number at which this vbo has
   * been added to this vao
   * @param data  the data that should be stored in the vbo
   * @param len   the count of entries of the data array
   */
	void update_vbo(int index, const int *data, unsigned int len);
	/**
   * Updates the data of an instanced vbo
   * @param index the index of this vbo i.e. the number at which this vbo has
   * been added to this vao
   * @param data  the data that should be stored in the vbo
   */
	template <typename T>
	void update_vbo(int index, std::vector<T> data) {
		static_assert(std::is_same<T, float>() || std::is_same<T, int>(),
					  "Only float and int data is permitted in vbos!");
		update_vbo(index, data.data(), data.size());
	}
	/**
   * Updates the data of an instanced vbo
   * @param index the index of this vbo in the vao
   * @param data  the data that should be stored in the vbo
   * @param start byte offset
   * @param len   the count of entries of the data array
   */
	template <typename T>
	void update_vbo(int index, std::vector<T> data, size_t start) {
		update_vbo(index, data.data(), start, data.size());
	}
	/**
   * Replaces Destructor, so you can safely copy a vao.
   * Cleans up all OpenGL related data.
   */
	void clean_up();
	/**
   * Not actually necessary
   */
	void unbind();
	/**
   * Draws the vao data corresponding to the present vbos
   */
	void draw(GLenum mode = GL_TRIANGLES);
	/**
   * Activates and binds the vao to the current context
   */
	void bind();
};
#endif
