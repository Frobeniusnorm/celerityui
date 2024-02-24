#include "vao.hpp"
void Vao::add_index_buffer(const unsigned int *data, size_t count) {
  itemsCount = count;
  glBindVertexArray(id);
  GLuint indid;
  glGenBuffers(1, &indid);
  indicesId = std::optional<GLuint>(indid);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesId.value());
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void Vao::clean_up() {
  for (Vbo v : vbos)
    glDeleteBuffers(1, &v.id);
  if (indicesId.has_value())
    glDeleteBuffers(1, &indicesId.value());
  glDeleteVertexArrays(1, &id);
}
template <typename T>
unsigned int Vao::gen_vbo(unsigned int index, unsigned int stride, T *data,
                          unsigned int len) {
  static_assert(std::is_same<T, float>() || std::is_same<T, int>() ||
                    std::is_same<T, const float>() ||
                    std::is_same<T, const int>(),
                "Only float and int data is permitted in vbos!");
  unsigned int id;
  glGenBuffers(1, &id);
  glBindBuffer(GL_ARRAY_BUFFER, id);
  glBufferData(GL_ARRAY_BUFFER, len * sizeof(float), data, GL_STATIC_DRAW);
  glEnableVertexAttribArray(index);
  if (std::is_same<T, float>() || std::is_same<T, const float>())
    glVertexAttribPointer(index, stride, GL_FLOAT, GL_FALSE, 0, nullptr);
  else
    glVertexAttribIPointer(index, stride, GL_INT, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  return id;
}
template <typename T>
unsigned int Vao::gen_instanced_vbo(unsigned int index, unsigned int stride,
                                    T *data, unsigned int len,
                                    unsigned int divisor) {
  static_assert(std::is_same<T, float>() || std::is_same<T, int>() ||
                    std::is_same<T, const float>() ||
                    std::is_same<T, const int>(),
                "Only float and int data is permitted in vbos!");
  unsigned int id;
  glGenBuffers(1, &id);
  glBindBuffer(GL_ARRAY_BUFFER, id);
  glBufferData(GL_ARRAY_BUFFER, len * sizeof(T), data, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(index);
  if (std::is_same<T, float>() || std::is_same<T, const float>())
    glVertexAttribPointer(index, stride, GL_FLOAT, GL_FALSE, 0, nullptr);
  else
    glVertexAttribIPointer(index, stride, GL_INT, 0, nullptr);
  glVertexAttribDivisor(index, divisor);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  return id;
}
template <typename T>
unsigned int Vao::add_vertex_buffer(unsigned int dim, const T *data,
                                    unsigned int len) {
  glBindVertexArray(id);
  vbos.emplace_back(gen_vbo(vbos.size(), dim, data, len), vbos.size(), dim);
  if (!indicesId.has_value())
    itemsCount = len / dim;
  return vbos.size() - 1;
}
unsigned int Vao::add_vertex_buffer(unsigned int dim, const float *data,
                                    unsigned int len) {
  return add_vertex_buffer<float>(dim, data, len);
}
unsigned int Vao::add_vertex_buffer(unsigned int dim, const int *data,
                                    unsigned int len) {
  return add_vertex_buffer<int>(dim, data, len);
}
template <typename T>
unsigned int Vao::add_instanced_vertex_buffer(unsigned int dim, const T *data,
                                              unsigned int len, int div) {
  glBindVertexArray(id);
  vbos.emplace_back(gen_instanced_vbo(vbos.size(), dim, data, len, div),
                    vbos.size(), dim, true);
  if (!instanceCount.has_value())
    instanceCount = 1;
  if (!indicesId.has_value())
    itemsCount = len / dim;
  return vbos.size() - 1;
}
unsigned int Vao::add_instanced_vertex_buffer(unsigned int dim,
                                              const float *data,
                                              unsigned int len, int div) {
  return add_instanced_vertex_buffer<float>(dim, data, len, div);
}
unsigned int Vao::add_instanced_vertex_buffer(unsigned int dim, const int *data,
                                              unsigned int len, int div) {
  return add_instanced_vertex_buffer<int>(dim, data, len, div);
}
template <typename T>
void Vao::update_vbo(int index, const T *data, size_t start, unsigned int len) {
  static_assert(std::is_same<T, float>() || std::is_same<T, int>(),
                "Only float and int data is permitted in vbos!");
  glBindVertexArray(this->id);
  glBindBuffer(GL_ARRAY_BUFFER, vbos[index].id);
  glBufferSubData(GL_ARRAY_BUFFER, start, sizeof(T) * len, &data[0]);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
template <typename T>
void Vao::update_vbo(int index, const T *data, unsigned int len) {
  glBindVertexArray(this->id);
  glBindBuffer(GL_ARRAY_BUFFER, vbos[index].id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(T) * len, &(data[0]), GL_DYNAMIC_DRAW);
  if (!indicesId.has_value())
    itemsCount = len / vbos[index].dim;
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void Vao::unbind() { glBindVertexArray(0); }
void Vao::draw(GLenum mode) {
  if (instanceCount.has_value()) {
    if (indicesId.has_value()) {
      glDrawElementsInstanced(mode, itemsCount, GL_UNSIGNED_INT, nullptr,
                              instanceCount.value());
    } else
      glDrawArraysInstanced(mode, 0, itemsCount, instanceCount.value());
  } else {
    if (indicesId.has_value()) {
      glDrawElements(mode, itemsCount, GL_UNSIGNED_INT, nullptr);
    } else
      glDrawArrays(mode, 0, itemsCount);
  }
}
void Vao::bind() { glBindVertexArray(id); }
void Vao::update_vbo(int index, const float *data, size_t start,
                     unsigned int len) {

  return update_vbo<float>(index, data, start, len);
}
void Vao::update_vbo(int index, const int *data, size_t start,
                     unsigned int len) {
  return update_vbo<int>(index, data, start, len);
}
void Vao::update_vbo(int index, const float *data, unsigned int len) {
  return update_vbo<float>(index, data, len);
}
void Vao::update_vbo(int index, const int *data, unsigned int len) {
  return update_vbo<int>(index, data, len);
}
