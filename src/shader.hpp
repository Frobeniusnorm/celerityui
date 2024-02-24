#ifndef SHADER_HPP
#define SHADER_HPP
#include <GL/glew.h>
#include <filesystem>
#include <fstream>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unordered_map>
#include <variant>
#include <vector>
#define STRINGIZE(...) #__VA_ARGS__
#define EXPAND_AND_STRINGIZE(...) STRINGIZE(__VA_ARGS__)

class ShaderProgram {
#if DEBUG_CEL
  std::vector<std::pair<std::string, GLuint>> shader;
  using uniform_var =
      std::variant<float, int, glm::vec2, glm::vec3, glm::vec4, glm::ivec2,
                   glm::ivec3, glm::ivec4, glm::mat2, glm::mat3, glm::mat4,
                   glm::mat2x3, glm::mat2x4, glm::mat3x2, glm::mat3x4,
                   glm::mat4x2, glm::mat4x3, std::pair<int, int>>;
  std::unordered_map<std::string, uniform_var> loaded_uniforms;
  std::string preproc;
#endif
protected:
  std::vector<std::string> attribs;
  std::unordered_map<std::string, GLint> uniformCache;
  uint activeTextures = 0;
  void _construct(
      std::vector<std::pair<std::string, GLuint>> shader,
      std::vector<std::string> predefattribs = std::vector<std::string>());
  ShaderProgram();

public:
  unsigned int id;
  /**
   *  Constructs a shader program consisting of possibly several shader stages
   * and optional predefined attributes
   *  @param shader vector entries each describe one shader stage as a pair of
   *  the shader source and the opengl shader type
   *  @param predefattribs if attributes should be directly bound to an index,
   * one can pass the list of atribute names in the correct order through this
   * parameter
   */
  ShaderProgram(
      std::vector<std::pair<std::string, GLuint>> shader,
      std::vector<std::string> predefattribs = std::vector<std::string>()) {
    _construct(shader, predefattribs);
  }
  ShaderProgram(const ShaderProgram &) = delete;
  ShaderProgram &operator=(const ShaderProgram &) = delete;
  ShaderProgram(ShaderProgram &&foo) = delete;
  ShaderProgram &operator=(ShaderProgram &&foo) = delete;
#if DEBUG_CEL
  void recompile();
#endif
  /**
   *  Associates the attribute name with a index which equals
   *  the number of attributes already present in this program
   */
  void add_attribute(std::string attr);
  /**
   *  Associates the attribute names with the index which equals
   *  the number of attributes already present in this program
   */
  void add_attributes(std::vector<std::string> attr);
  /**
   *  Does NOT happen in the destructor,
   *  so you can copy and move programs
   */
  void clean_up();
  /**
   *  Tells OpenGL to draw the following draw calls
   *  with this program
   */
  void start();
  /**
   *  Not necessary
   */
  void stop() const;
  /**
   *  Constructs a standard shader program consisting of vertex shader and
   * fragment shader.
   */
  ShaderProgram(
      std::string vertex_src, std::string fragment_src,
      std::vector<std::string> predefattribs = std::vector<std::string>()) {
    _construct(
        {{vertex_src, GL_VERTEX_SHADER}, {fragment_src, GL_FRAGMENT_SHADER}},
        predefattribs);
  }
  /**
   *  Loads a value to a uniform variable
   * @param id the identifier of that uniform variable
   */
  void load(std::string id, float value);
  /**
   *  Loads a value to a uniform variable
   * @param id the identifier of that uniform variable
   */
  void load(std::string id, glm::vec2 value);
  /**
   *  Loads a value to a uniform variable
   * @param id the identifier of that uniform variable
   */
  void load(std::string id, glm::vec3 value);
  /**
   *  Loads a value to a uniform variable
   * @param id the identifier of that uniform variable
   */
  void load(std::string id, glm::vec4 value);
  /**
   *  Loads a value to a uniform variable
   * @param id the identifier of that uniform variable
   */
  void load(std::string id, int value);
  /**
   *  Loads a value to a uniform variable
   * @param id the identifier of that uniform variable
   */
  void load(std::string id, glm::ivec2 value);
  /**
   *  Loads a value to a uniform variable
   * @param id the identifier of that uniform variable
   */
  void load(std::string id, glm::ivec3 value);
  /**
   *  Loads a value to a uniform variable
   * @param id the identifier of that uniform variable
   */
  void load(std::string id, glm::ivec4 value);
  /**
   *  Loads a value to a uniform variable
   * @param id the identifier of that uniform variable
   */
  void load(std::string id, glm::mat2 value);
  /**
   *  Loads a value to a uniform variable
   * @param id the identifier of that uniform variable
   */
  void load(std::string id, glm::mat3 value);
  /**
   *  Loads a value to a uniform variable
   * @param id the identifier of that uniform variable
   */
  void load(std::string id, glm::mat4 value);
  /**
   *  Loads a value to a uniform variable
   * @param id the identifier of that uniform variable
   */
  void load(std::string id, glm::mat2x3 value);
  /**
   *  Loads a value to a uniform variable
   * @param id the identifier of that uniform variable
   */
  void load(std::string id, glm::mat2x4 value);
  /**
   *  Loads a value to a uniform variable
   * @param id the identifier of that uniform variable
   */
  void load(std::string id, glm::mat3x2 value);
  /**
   *  Loads a value to a uniform variable
   * @param id the identifier of that uniform variable
   */
  void load(std::string id, glm::mat3x4 value);
  /**
   *  Loads a value to a uniform variable
   * @param id the identifier of that uniform variable
   */
  void load(std::string id, glm::mat4x2 value);
  /**
   *  Loads a value to a uniform variable
   * @param id the identifier of that uniform variable
   */
  void
  load(std::string id,
       glm::mat4x3 value); /**
                            *  Loads a value to a uniform variable
                            * @param id the identifier of that uniform variable
                            */

  /**
   * Loads an image to a uniform variable. The image is automatically bound.
   * @param id the identifier of that uniform variable
   * @param tex the opengl id for the texture
   * @param unit the texture socket this texture should be loaded to. Will be
   * automatically assigned if -1.
   */
  void load_texture(std::string id, GLuint tex, int unit = -1);
  /**
   * Loads an texture array to a uniform variable. The image is automatically
   * bound.
   * @param id the identifier of that uniform variable
   * @param tex the opengl id for the texture array
   * @param num_layers size of texture array
   * @param unit the texture socket this texture should be loaded to. Will be
   * automatically assigned if -1.
   */
  void load_texture_array(std::string id, GLuint tex, int unit = -1);
};
class ComputeShader : public ShaderProgram {
private:
  int drawingTextures = 0;

public:
  ComputeShader(std::string source) : ShaderProgram() {
    _construct({{source, GL_COMPUTE_SHADER}});
  }
  void dispatch(GLuint workGroupX, GLuint workGroupY, GLuint workGroupZ = 1);
  /**
   *  Waits for all memory barriers and unbinds this shader
   */
  void stop() const;
  void wait_for_barriers() const;
  /**
   *  Binds the texture to the n-th image unit
   *  Can be called multiple times. The first bound texture per iteration will
   * be bound to 0, the second to 1, ....
   */
  void bind_image(GLuint tex, GLenum access = GL_READ_WRITE,
                  GLenum format = GL_RGBA32F, int unit = -1);
};
#endif
