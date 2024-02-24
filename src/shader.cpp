#include "shader.hpp"
#include "logger.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <filesystem>
#include <fstream>
#include <glm/glm.hpp>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unordered_map>
#include <vector>
static GLuint createShader(std::string srcs, GLuint type) {
  GLuint id = glCreateShader(type);

  if (srcs.length() == 0) {
    log(ERROR, "Could not compile Shader, because of empty source!");
    return 0;
  }
  const char *src = srcs.c_str();
  glShaderSource(id, 1, &src, nullptr);
  glCompileShader(id);
  int success;
  char infoLog[512];
  glGetShaderiv(id, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(id, 512, nullptr, infoLog);
    log(ERROR, "SHADER COMPILATION FAILED: \n" + std::string(infoLog) +
                   "\nFor source:\n" + srcs);
  }
  return id;
}

void ShaderProgram::_construct(
    std::vector<std::pair<std::string, GLuint>> shader,
    std::vector<std::string> predefattribs) {
  id = glCreateProgram();
  std::vector<GLuint> todel(shader.size());
  int i = 0;
  for (auto &shd : shader) {
    GLuint sid = createShader(shd.first, shd.second);
    glAttachShader(id, sid);
    todel[i++] = sid;
  }
  glLinkProgram(id);
  for (GLuint del : todel)
    glDeleteShader(del);
  int success;
  char infoLog[512];
  glGetProgramiv(id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(id, 512, nullptr, infoLog);
    log(ERROR, "SHADERPROGRAM LINKING FAILED\n" + std::string(infoLog));
  } else
    log(VERBOSE, "Successfully compiled shaderprogram");
  if (!predefattribs.empty())
    add_attributes(predefattribs);
}

#if DEBUG_BCE
void ShaderProgram::recompile() {
  GLuint id = glCreateProgram();
  std::vector<GLuint> todel(shader.size());
  int i = 0;
  for (auto &shd : shader) {
    GLuint sid = createShader(shd.first, shd.second);
    glAttachShader(id, sid);
    todel[i++] = sid;
  }
  glLinkProgram(id);
  for (GLuint del : todel)
    glDeleteShader(del);
  int success;
  char infoLog[512];
  glGetProgramiv(id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(id, 512, nullptr, infoLog);
    log(ERROR, "SHADERPROGRAM LINKING FAILED\n" + std::string(infoLog));
  } else {
    for (unsigned int i = 0; i < attribs.size(); i++)
      glBindAttribLocation(id, i, attribs[i].c_str());
    glDeleteProgram(this->id);
    this->id = id;
    // load previously loaded uniforms
    for (auto &[name, cont] : loaded_uniforms) {
      switch (cont.index()) {
      case 0:
        load(name, std::get<float>(cont));
        break;
      case 1:
        load(name, std::get<int>(cont));
        break;
      case 2:
        load(name, std::get<glm::vec2>(cont));
        break;
      case 3:
        load(name, std::get<glm::vec3>(cont));
        break;
      case 4:
        load(name, std::get<glm::vec4>(cont));
        break;
      case 5:
        load(name, std::get<glm::ivec2>(cont));
        break;
      case 6:
        load(name, std::get<glm::ivec3>(cont));
        break;
      case 7:
        load(name, std::get<glm::ivec4>(cont));
        break;
      case 8:
        load(name, std::get<glm::mat2>(cont));
        break;
      case 9:
        load(name, std::get<glm::mat3>(cont));
        break;
      case 10:
        load(name, std::get<glm::mat4>(cont));
        break;
      case 11:
        load(name, std::get<glm::mat2x3>(cont));
        break;
      case 12:
        load(name, std::get<glm::mat2x4>(cont));
        break;
      case 13:
        load(name, std::get<glm::mat3x2>(cont));
        break;
      case 14:
        load(name, std::get<glm::mat3x4>(cont));
        break;
      case 15:
        load(name, std::get<glm::mat4x2>(cont));
        break;
      case 16:
        load(name, std::get<glm::mat4x3>(cont));
        break;
      case 17: {
        auto &[texture, unit] = std::get<std::pair<int, int>>(cont);
        load_texture(name, texture, unit);
      } break;
      }
    }
    info("Successfully recompiled shaderprogram");
  }
}
#endif

ShaderProgram::ShaderProgram() {}
/**
 *  Associates the attribute name with a index which equals
 *  the number of attributes already present in this program
 */
void ShaderProgram::add_attribute(std::string attr) {
  int i = attribs.size();
  attribs.push_back(attr);
  glBindAttribLocation(id, i, attribs[i].c_str());
}
/**
 *  Associates the attribute names with the index which equals
 *  the number of attributes already present in this program
 */
void ShaderProgram::add_attributes(std::vector<std::string> attr) {
  attribs.reserve(attribs.size() + attr.size());
  for (auto &foo : attr)
    add_attribute(foo);
}
/**
 *  Does NOT happen in the destructor,
 *  so you can copy and move programs
 */
void ShaderProgram::clean_up() { glDeleteProgram(id); }
/**
 *  Tells OpenGL to draw the following draw calls
 *  with this program
 */
void ShaderProgram::start() {
  glUseProgram(id);
  activeTextures = 0;
}
/**
 *  Not necessary
 */
void ShaderProgram::stop() const { glUseProgram(0); }

/**
 *  Loads a value to a uniform variable
 * @param id the identifier of that uniform variable
 */
void ShaderProgram::load(std::string id, float value) {
  GLint i;
  if (uniformCache.find(id) == uniformCache.end()) {
    uniformCache.insert({id, glGetUniformLocation(this->id, id.c_str())});
    i = uniformCache[id];
    if (i == -1) {
      log(WARNING, "Uniform \"" + id + "\" does not exist!");
      return;
    }
  } else
    i = uniformCache[id];
#ifdef DEBUG_BCE
  loaded_uniforms.insert({id, {value}});
#endif
  glUniform1f(i, value);
}
/**
 *  Loads a value to a uniform variable
 * @param id the identifier of that uniform variable
 */
void ShaderProgram::load(std::string id, glm::vec2 value) {
  GLint i;
  if (uniformCache.find(id) == uniformCache.end()) {
    uniformCache.insert({id, glGetUniformLocation(this->id, id.c_str())});
    i = uniformCache[id];
    if (i == -1) {
      log(WARNING, "Uniform \"" + id + "\" does not exist!");
      return;
    }
  } else
    i = uniformCache[id];
#ifdef DEBUG_BCE
  loaded_uniforms.insert({id, {value}});
#endif
  glUniform2f(i, value.x, value.y);
}
/**
 *  Loads a value to a uniform variable
 * @param id the identifier of that uniform variable
 */
void ShaderProgram::load(std::string id, glm::vec3 value) {
  GLint i;
  if (uniformCache.find(id) == uniformCache.end()) {
    uniformCache.insert({id, glGetUniformLocation(this->id, id.c_str())});
    i = uniformCache[id];
    if (i == -1) {
      log(WARNING, "Uniform \"" + id + "\" does not exist!");
      return;
    }
  } else
    i = uniformCache[id];
#ifdef DEBUG_BCE
  loaded_uniforms.insert({id, {value}});
#endif
  glUniform3f(i, value.x, value.y, value.z);
}
/**
 *  Loads a value to a uniform variable
 * @param id the identifier of that uniform variable
 */
void ShaderProgram::load(std::string id, glm::vec4 value) {
  GLint i;
  if (uniformCache.find(id) == uniformCache.end()) {
    uniformCache.insert({id, glGetUniformLocation(this->id, id.c_str())});
    i = uniformCache[id];
    if (i == -1) {
      log(WARNING, "Uniform \"" + id + "\" does not exist!");
      return;
    }
  } else
    i = uniformCache[id];
#ifdef DEBUG_BCE
  loaded_uniforms.insert({id, {value}});
#endif
  glUniform4f(i, value.x, value.y, value.z, value.w);
}
/**
 *  Loads a value to a uniform variable
 * @param id the identifier of that uniform variable
 */
void ShaderProgram::load(std::string id, int value) {
  GLint i;
  if (uniformCache.find(id) == uniformCache.end()) {
    uniformCache.insert({id, glGetUniformLocation(this->id, id.c_str())});
    i = uniformCache[id];
    if (i == -1) {
      log(WARNING, "Uniform \"" + id + "\" does not exist!");
      return;
    }
  } else
    i = uniformCache[id];
#ifdef DEBUG_BCE
  loaded_uniforms.insert({id, {value}});
#endif
  glUniform1i(i, value);
}
/**
 *  Loads a value to a uniform variable
 * @param id the identifier of that uniform variable
 */
void ShaderProgram::load(std::string id, glm::ivec2 value) {
  GLint i;
  if (uniformCache.find(id) == uniformCache.end()) {
    uniformCache.insert({id, glGetUniformLocation(this->id, id.c_str())});
    i = uniformCache[id];
    if (i == -1) {
      log(WARNING, "Uniform \"" + id + "\" does not exist!");
      return;
    }
  } else
    i = uniformCache[id];
#ifdef DEBUG_BCE
  loaded_uniforms.insert({id, {value}});
#endif
  glUniform2i(i, value.x, value.y);
}
/**
 *  Loads a value to a uniform variable
 * @param id the identifier of that uniform variable
 */
void ShaderProgram::load(std::string id, glm::ivec3 value) {
  GLint i;
  if (uniformCache.find(id) == uniformCache.end()) {
    uniformCache.insert({id, glGetUniformLocation(this->id, id.c_str())});
    i = uniformCache[id];
    if (i == -1) {
      log(WARNING, "Uniform \"" + id + "\" does not exist!");
      return;
    }
  } else
    i = uniformCache[id];
#ifdef DEBUG_BCE
  loaded_uniforms.insert({id, {value}});
#endif
  glUniform3i(i, value.x, value.y, value.z);
}
/**
 *  Loads a value to a uniform variable
 * @param id the identifier of that uniform variable
 */
void ShaderProgram::load(std::string id, glm::ivec4 value) {
  GLint i;
  if (uniformCache.find(id) == uniformCache.end()) {
    uniformCache.insert({id, glGetUniformLocation(this->id, id.c_str())});
    i = uniformCache[id];
    if (i == -1) {
      log(WARNING, "Uniform \"" + id + "\" does not exist!");
      return;
    }
  } else
    i = uniformCache[id];
#ifdef DEBUG_BCE
  loaded_uniforms.insert({id, {value}});
#endif
  glUniform4i(i, value.x, value.y, value.z, value.w);
}

/**
 *  Loads a value to a uniform variable
 * @param id the identifier of that uniform variable
 */
void ShaderProgram::load(std::string id, glm::mat2 value) {
  GLint i;
  if (uniformCache.find(id) == uniformCache.end()) {
    uniformCache.insert({id, glGetUniformLocation(this->id, id.c_str())});
    i = uniformCache[id];
    if (i == -1) {
      log(WARNING, "Uniform \"" + id + "\" does not exist!");
      return;
    }
  } else
    i = uniformCache[id];
#ifdef DEBUG_BCE
  loaded_uniforms.insert({id, {value}});
#endif
  glUniformMatrix2fv(i, 1, false, &(value[0][0]));
}
/**
 *  Loads a value to a uniform variable
 * @param id the identifier of that uniform variable
 */
void ShaderProgram::load(std::string id, glm::mat3 value) {
  GLint i;
  if (uniformCache.find(id) == uniformCache.end()) {
    uniformCache.insert({id, glGetUniformLocation(this->id, id.c_str())});
    i = uniformCache[id];
    if (i == -1) {
      log(WARNING, "Uniform \"" + id + "\" does not exist!");
      return;
    }
  } else
    i = uniformCache[id];
#ifdef DEBUG_BCE
  loaded_uniforms.insert({id, {value}});
#endif
  glUniformMatrix3fv(i, 1, false, &(value[0][0]));
}
/**
 *  Loads a value to a uniform variable
 * @param id the identifier of that uniform variable
 */
void ShaderProgram::load(std::string id, glm::mat4 value) {
  GLint i;
  if (uniformCache.find(id) == uniformCache.end()) {
    uniformCache.insert({id, glGetUniformLocation(this->id, id.c_str())});
    i = uniformCache[id];
    if (i == -1) {
      log(WARNING, "Uniform \"" + id + "\" does not exist!");
      return;
    }
  } else
    i = uniformCache[id];
#ifdef DEBUG_BCE
  loaded_uniforms.insert({id, {value}});
#endif
  glUniformMatrix4fv(i, 1, false, &(value[0][0]));
}
/**
 *  Loads a value to a uniform variable
 * @param id the identifier of that uniform variable
 */
void ShaderProgram::load(std::string id, glm::mat2x3 value) {
  GLint i;
  if (uniformCache.find(id) == uniformCache.end()) {
    uniformCache.insert({id, glGetUniformLocation(this->id, id.c_str())});
    i = uniformCache[id];
    if (i == -1) {
      log(WARNING, "Uniform \"" + id + "\" does not exist!");
      return;
    }
  } else
    i = uniformCache[id];
#ifdef DEBUG_BCE
  loaded_uniforms.insert({id, {value}});
#endif
  glUniformMatrix2x3fv(i, 1, false, &(value[0][0]));
}
/**
 *  Loads a value to a uniform variable
 * @param id the identifier of that uniform variable
 */
void ShaderProgram::load(std::string id, glm::mat2x4 value) {
  GLint i;
  if (uniformCache.find(id) == uniformCache.end()) {
    uniformCache.insert({id, glGetUniformLocation(this->id, id.c_str())});
    i = uniformCache[id];
    if (i == -1) {
      log(WARNING, "Uniform \"" + id + "\" does not exist!");
      return;
    }
  } else
    i = uniformCache[id];
#ifdef DEBUG_BCE
  loaded_uniforms.insert({id, {value}});
#endif
  glUniformMatrix2x4fv(i, 1, false, &(value[0][0]));
}
/**
 *  Loads a value to a uniform variable
 * @param id the identifier of that uniform variable
 */
void ShaderProgram::load(std::string id, glm::mat3x2 value) {
  GLint i;
  if (uniformCache.find(id) == uniformCache.end()) {
    uniformCache.insert({id, glGetUniformLocation(this->id, id.c_str())});
    i = uniformCache[id];
    if (i == -1) {
      log(WARNING, "Uniform \"" + id + "\" does not exist!");
      return;
    }
  } else
    i = uniformCache[id];
#ifdef DEBUG_BCE
  loaded_uniforms.insert({id, {value}});
#endif
  glUniformMatrix3x2fv(i, 1, false, &(value[0][0]));
}
/**
 *  Loads a value to a uniform variable
 * @param id the identifier of that uniform variable
 */
void ShaderProgram::load(std::string id, glm::mat3x4 value) {
  GLint i;
  if (uniformCache.find(id) == uniformCache.end()) {
    uniformCache.insert({id, glGetUniformLocation(this->id, id.c_str())});
    i = uniformCache[id];
    if (i == -1) {
      log(WARNING, "Uniform \"" + id + "\" does not exist!");
      return;
    }
  } else
    i = uniformCache[id];
#ifdef DEBUG_BCE
  loaded_uniforms.insert({id, {value}});
#endif
  glUniformMatrix3x4fv(i, 1, false, &(value[0][0]));
}
/**
 *  Loads a value to a uniform variable
 * @param id the identifier of that uniform variable
 */
void ShaderProgram::load(std::string id, glm::mat4x2 value) {
  GLint i;
  if (uniformCache.find(id) == uniformCache.end()) {
    uniformCache.insert({id, glGetUniformLocation(this->id, id.c_str())});
    i = uniformCache[id];
    if (i == -1) {
      log(WARNING, "Uniform \"" + id + "\" does not exist!");
      return;
    }
  } else
    i = uniformCache[id];
#ifdef DEBUG_BCE
  loaded_uniforms.insert({id, {value}});
#endif
  glUniformMatrix4x2fv(i, 1, false, &(value[0][0]));
}
/**
 *  Loads a value to a uniform variable
 * @param id the identifier of that uniform variable
 */
void ShaderProgram::load(std::string id, glm::mat4x3 value) {
  GLint i;
  if (uniformCache.find(id) == uniformCache.end()) {
    uniformCache.insert({id, glGetUniformLocation(this->id, id.c_str())});
    i = uniformCache[id];
    if (i == -1) {
      log(WARNING, "Uniform \"" + id + "\" does not exist!");
      return;
    }
  } else
    i = uniformCache[id];
#ifdef DEBUG_BCE
  loaded_uniforms.insert({id, {value}});
#endif
  glUniformMatrix4x3fv(i, 1, false, &(value[0][0]));
}
/**
 * Loads an image to a uniform variable. The image is automatically bound.
 * @param id the identifier of that uniform variable
 * @param tex the opengl id for the texture
 * @param unit the texture socket this texture should be loaded to. Will be
 * automatically assigned if -1.
 */
void ShaderProgram::load_texture(std::string id, GLuint tex, int unit) {
  GLint i;
  if (uniformCache.find(id) == uniformCache.end()) {
    uniformCache.insert({id, glGetUniformLocation(this->id, id.c_str())});
    i = uniformCache[id];
    if (i == -1) {
      log(WARNING, "Uniform \"" + id + "\" does not exist!");
      return;
    }
  } else
    i = uniformCache[id];
  if (unit < 0)
    unit = activeTextures;
  activeTextures++;
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, tex);
  glUniform1i(i, unit);
#ifdef DEBUG_BCE
  loaded_uniforms.insert({id, {std::pair<int, int>{tex, unit}}});
#endif
}

void ShaderProgram::load_texture_array(std::string id, GLuint tex, int unit) {
  GLint i;
  if (uniformCache.find(id) == uniformCache.end()) {
    uniformCache.insert({id, glGetUniformLocation(this->id, id.c_str())});
    i = uniformCache[id];
    if (i == -1) {
      log(WARNING, "Uniform \"" + id + "\" does not exist!");
      return;
    }
  } else
    i = uniformCache[id];
  if (unit < 0)
    unit = activeTextures;
  activeTextures++;
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D_ARRAY, tex);
  glUniform1i(i, unit);
#ifdef DEBUG_BCE
  loaded_uniforms.insert({id, {std::pair<int, int>{tex, unit}}});
#endif
}
void ComputeShader::dispatch(GLuint workGroupX, GLuint workGroupY,
                             GLuint workGroupZ) {
  glDispatchCompute(workGroupX, workGroupY, workGroupZ);
  drawingTextures = 0;
}
void ComputeShader::wait_for_barriers() const {
  glMemoryBarrier(GL_ALL_BARRIER_BITS);
}
/**
 *  Waits for all memory barriers and unbinds this shader
 */
void ComputeShader::stop() const {
  wait_for_barriers();
  glUseProgram(0);
}
/**
 *  Binds the texture to the n-th image unit
 *  Can be called multiple times. The first bound texture per iteration will
 * be bound to 0, the second to 1, ....
 */
void ComputeShader::bind_image(GLuint tex, GLenum access, GLenum format,
                               int unit) {
  if (unit < 0)
    unit = drawingTextures;
  drawingTextures = unit + 1;
  glBindImageTexture(unit, tex, 0, GL_FALSE, 0, access, format);
}
