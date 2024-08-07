#include "layer_viewer.h"

#include <imgui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "FlexEngine/Loader/shaders.h"
#include "FlexEngine/Loader/gltf.h"

namespace FlexEngine
{
  Shader shader;

  #pragma region copium and pasta

  struct IndirectDrawCommand {
    uint32_t count;
    uint32_t instanceCount;
    uint32_t firstIndex;
    int32_t baseVertex;
    uint32_t baseInstance;
  };

  struct Primitive {
    IndirectDrawCommand draw;
    GLenum primitiveType;
    GLenum indexType;
    GLuint vertexArray;

    size_t materialUniformsIndex;
    GLuint albedoTexture;
  };

  struct Mesh {
    GLuint drawsBuffer;
    std::vector<Primitive> primitives;
  };

  struct Texture {
    GLuint texture;
  };

  enum MaterialUniformFlags : uint32_t {
    None = 0 << 0,
    HasBaseColorTexture = 1 << 0,
  };

  struct MaterialUniforms {
    glm::fvec4 baseColorFactor;
    float alphaCutoff;
    uint32_t flags;
  };

  struct Viewer {
    fastgltf::Asset asset;

    std::vector<GLuint> buffers;
    std::vector<GLuint> bufferAllocations;
    std::vector<Mesh> meshes;
    std::vector<Texture> textures;

    std::vector<MaterialUniforms> materials;
    std::vector<GLuint> materialBuffers;

    glm::mat4 viewMatrix = glm::mat4(1.0f);
    glm::mat4 projectionMatrix = glm::mat4(1.0f);
    GLint viewProjectionMatrixUniform = GL_NONE;
    GLint modelMatrixUniform = GL_NONE;

    float lastFrame = 0.0f;
    float deltaTime = 0.0f;
    glm::vec3 accelerationVector = glm::vec3(0.0f);
    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::dvec2 lastCursorPosition = glm::dvec2(0.0f);
    glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
    float yaw = -90.0f;
    float pitch = 0.0f;
    bool firstMouse = true;
  };

  auto gltfFile = std::string_view{ "assets\\models\\trailer.glb" };
  Viewer viewer;

  constexpr glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
  std::vector<int> keys_held = { 0 };

  void updateCameraMatrix(Viewer* viewer) {
    glm::mat4 viewProjection = viewer->projectionMatrix * viewer->viewMatrix;
    glUniformMatrix4fv(viewer->viewProjectionMatrixUniform, 1, GL_FALSE, &viewProjection[0][0]);
  }

  void windowSizeCallback(GLFWwindow* window, int width, int height) {
    void* ptr = glfwGetWindowUserPointer(window);
    auto* m_viewer = static_cast<Viewer*>(ptr);

    m_viewer->projectionMatrix = glm::perspective(glm::radians(75.0f),
      static_cast<float>(width) / static_cast<float>(height),
      0.01f, 1000.0f);

    glViewport(0, 0, width, height);
  }

  void cursorCallback(GLFWwindow* window, double xpos, double ypos) {
    void* ptr = glfwGetWindowUserPointer(window);
    auto* m_viewer = static_cast<Viewer*>(ptr);

    if (m_viewer->firstMouse) {
      m_viewer->lastCursorPosition = { xpos, ypos };
      m_viewer->firstMouse = false;
    }

    auto offset = glm::vec2(xpos - m_viewer->lastCursorPosition.x, m_viewer->lastCursorPosition.y - ypos);
    m_viewer->lastCursorPosition = { xpos, ypos };
    offset *= 0.1f;

    m_viewer->yaw += offset.x;
    m_viewer->pitch += offset.y;
    m_viewer->pitch = glm::clamp(m_viewer->pitch, -89.0f, 89.0f);

    auto& direction = m_viewer->direction;
    direction.x = cos(glm::radians(m_viewer->yaw)) * cos(glm::radians(m_viewer->pitch));
    direction.y = sin(glm::radians(m_viewer->pitch));
    direction.z = sin(glm::radians(m_viewer->yaw)) * cos(glm::radians(m_viewer->pitch));
    direction = glm::normalize(direction);
  }

  void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // hard coded key held feature
    // add key to held list
    switch (action)
    {
      case GLFW_PRESS:
        keys_held.push_back(key);
        break;
      case GLFW_RELEASE:
        keys_held.erase(std::find(keys_held.begin(), keys_held.end(), key));
        break;
      default:
        break;
    }

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
  }

  glm::mat4 getTransformMatrix(const fastgltf::Node& node, glm::mat4x4& base) {
    /** Both a matrix and TRS values are not allowed
     * to exist at the same time according to the spec */
    if (const auto* pMatrix = std::get_if<fastgltf::Node::TransformMatrix>(&node.transform)) {
      return base * glm::mat4x4(glm::make_mat4x4(pMatrix->data()));
    }

    if (const auto* pTransform = std::get_if<fastgltf::Node::TRS>(&node.transform)) {
      // Warning: The quaternion to mat4x4 conversion here is not correct with all versions of glm.
      // glTF provides the quaternion as (x, y, z, w), which is the same layout glm used up to version 0.9.9.8.
      // However, with commit 59ddeb7 (May 2021) the default order was changed to (w, x, y, z).
      // You could either define GLM_FORCE_QUAT_DATA_XYZW to return to the old layout,
      // or you could use the recently added static factory constructor glm::quat::wxyz(w, x, y, z),
      // which guarantees the parameter order.
      return base
        * glm::translate(glm::mat4(1.0f), glm::make_vec3(pTransform->translation.data()))
        * glm::toMat4(glm::make_quat(pTransform->rotation.data()))
        * glm::scale(glm::mat4(1.0f), glm::make_vec3(pTransform->scale.data()));
    }

    return base;
  }

  bool loadGltf(Viewer* viewer, std::string_view _cPath) {
    auto cPath = std::filesystem::absolute(_cPath);

    if (!std::filesystem::exists(cPath)) {
      std::cout << "Failed to find " << cPath << '\n';
      return false;
    }

    std::cout << "Loading " << cPath << '\n';

    // Parse the glTF file and get the constructed asset
    {
      fastgltf::Parser parser(fastgltf::Extensions::KHR_mesh_quantization);

      auto path = std::filesystem::path{ cPath };

      constexpr auto gltfOptions =
        fastgltf::Options::DontRequireValidAssetMember |
        fastgltf::Options::AllowDouble |
        fastgltf::Options::LoadGLBBuffers |
        fastgltf::Options::LoadExternalBuffers |
        fastgltf::Options::LoadExternalImages |
        fastgltf::Options::GenerateMeshIndices;

      fastgltf::GltfDataBuffer data;
      data.loadFromFile(path);

      auto type = fastgltf::determineGltfFileType(&data);
      fastgltf::Expected<fastgltf::Asset> asset(fastgltf::Error::None);
      if (type == fastgltf::GltfType::glTF) {
        asset = parser.loadGLTF(&data, path.parent_path(), gltfOptions);
      }
      else if (type == fastgltf::GltfType::GLB) {
        asset = parser.loadBinaryGLTF(&data, path.parent_path(), gltfOptions);
      }
      else {
        std::cerr << "Failed to determine glTF container" << '\n';
        return false;
      }

      if (asset.error() != fastgltf::Error::None) {
        std::cerr << "Failed to load glTF: " << fastgltf::getErrorMessage(asset.error()) << '\n';
        return false;
      }

      viewer->asset = std::move(asset.get());
    }

    // Some buffers are already allocated during parsing of the glTF, like e.g. base64 buffers
    // through our callback functions. Therefore, we only resize our output buffer vector, but
    // create our buffer handles later on.
    auto& buffers = viewer->asset.buffers;
    viewer->buffers.reserve(buffers.size());

    for (auto& buffer : buffers) {
      constexpr GLuint bufferUsage = GL_STATIC_DRAW;

      std::visit(fastgltf::visitor{
          [](auto& arg) {}, // Covers FilePathWithOffset, BufferView, ... which are all not possible
          [&](fastgltf::sources::Vector& vector) {
              GLuint glBuffer;
              glCreateBuffers(1, &glBuffer);
              glNamedBufferData(glBuffer, static_cast<int64_t>(buffer.byteLength),
                                vector.bytes.data(), bufferUsage);
              viewer->buffers.emplace_back(glBuffer);
          },
          [&](fastgltf::sources::CustomBuffer& customBuffer) {
        // We don't need to do anything special here, the buffer has already been created.
        viewer->buffers.emplace_back(static_cast<GLuint>(customBuffer.id));
    },
        }, buffer.data);
    }

    return true;
  }

  bool loadMesh(Viewer* viewer, fastgltf::Mesh& mesh) {
    auto& asset = viewer->asset;
    Mesh outMesh = {};
    outMesh.primitives.resize(mesh.primitives.size());

    for (auto it = mesh.primitives.begin(); it != mesh.primitives.end(); ++it) {
      auto* positionIt = it->findAttribute("POSITION");
      // A mesh primitive is required to hold the POSITION attribute.
      assert(positionIt != it->attributes.end());

      // We only support indexed geometry.
      if (!it->indicesAccessor.has_value()) {
        return false;
      }

      // Generate the VAO
      GLuint vao = GL_NONE;
      glCreateVertexArrays(1, &vao);

      // Get the output primitive
      auto index = std::distance(mesh.primitives.begin(), it);
      auto& primitive = outMesh.primitives[index];
      primitive.primitiveType = fastgltf::to_underlying(it->type);
      primitive.vertexArray = vao;
      if (it->materialIndex.has_value()) {
        primitive.materialUniformsIndex = it->materialIndex.value() + 1; // Adjust for default material
        auto& material = viewer->asset.materials[it->materialIndex.value()];
        if (material.pbrData.baseColorTexture.has_value()) {
          auto& texture = viewer->asset.textures[material.pbrData.baseColorTexture->textureIndex];
          if (!texture.imageIndex.has_value())
            return false;
          primitive.albedoTexture = viewer->textures[texture.imageIndex.value()].texture;
        }
      }
      else {
        primitive.materialUniformsIndex = 0;
      }

      {
        // Position
        auto& positionAccessor = asset.accessors[positionIt->second];
        if (!positionAccessor.bufferViewIndex.has_value())
          continue;

        glEnableVertexArrayAttrib(vao, 0);
        glVertexArrayAttribFormat(vao, 0,
          static_cast<GLint>(fastgltf::getNumComponents(positionAccessor.type)),
          fastgltf::getGLComponentType(positionAccessor.componentType),
          GL_FALSE, 0);
        glVertexArrayAttribBinding(vao, 0, 0);

        auto& positionView = asset.bufferViews[positionAccessor.bufferViewIndex.value()];
        auto offset = positionView.byteOffset + positionAccessor.byteOffset;
        if (positionView.byteStride.has_value()) {
          glVertexArrayVertexBuffer(vao, 0, viewer->buffers[positionView.bufferIndex],
            static_cast<GLintptr>(offset),
            static_cast<GLsizei>(positionView.byteStride.value()));
        }
        else {
          glVertexArrayVertexBuffer(vao, 0, viewer->buffers[positionView.bufferIndex],
            static_cast<GLintptr>(offset),
            static_cast<GLsizei>(fastgltf::getElementByteSize(positionAccessor.type, positionAccessor.componentType)));
        }
      }

      {
        // Tex coord
        auto texcoord0 = it->findAttribute("TEXCOORD_0");
        auto& texCoordAccessor = asset.accessors[texcoord0->second];
        if (!texCoordAccessor.bufferViewIndex.has_value())
          continue;

        glEnableVertexArrayAttrib(vao, 1);
        glVertexArrayAttribFormat(vao, 1, static_cast<GLint>(fastgltf::getNumComponents(texCoordAccessor.type)),
          fastgltf::getGLComponentType(texCoordAccessor.componentType),
          GL_FALSE, 0);
        glVertexArrayAttribBinding(vao, 1, 1);

        auto& texCoordView = asset.bufferViews[texCoordAccessor.bufferViewIndex.value()];
        auto offset = texCoordView.byteOffset + texCoordAccessor.byteOffset;
        if (texCoordView.byteStride.has_value()) {
          glVertexArrayVertexBuffer(vao, 1, viewer->buffers[texCoordView.bufferIndex],
            static_cast<GLintptr>(offset),
            static_cast<GLsizei>(texCoordView.byteStride.value()));
        }
        else {
          glVertexArrayVertexBuffer(vao, 1, viewer->buffers[texCoordView.bufferIndex],
            static_cast<GLintptr>(offset),
            static_cast<GLsizei>(fastgltf::getElementByteSize(texCoordAccessor.type, texCoordAccessor.componentType)));
        }
      }

      // Generate the indirect draw command
      auto& draw = primitive.draw;
      draw.instanceCount = 1;
      draw.baseInstance = 0;
      draw.baseVertex = 0;

      auto& indices = asset.accessors[it->indicesAccessor.value()];
      if (!indices.bufferViewIndex.has_value())
        return false;
      draw.count = static_cast<uint32_t>(indices.count);

      auto& indicesView = asset.bufferViews[indices.bufferViewIndex.value()];
      draw.firstIndex = static_cast<uint32_t>(indices.byteOffset + indicesView.byteOffset) / fastgltf::getElementByteSize(indices.type, indices.componentType);
      primitive.indexType = getGLComponentType(indices.componentType);
      glVertexArrayElementBuffer(vao, viewer->buffers[indicesView.bufferIndex]);
    }

    // Create the buffer holding all of our primitive structs.
    glCreateBuffers(1, &outMesh.drawsBuffer);
    glNamedBufferData(outMesh.drawsBuffer, static_cast<GLsizeiptr>(outMesh.primitives.size() * sizeof(Primitive)),
      outMesh.primitives.data(), GL_STATIC_DRAW);

    viewer->meshes.emplace_back(outMesh);

    return true;
  }

  bool loadImage(Viewer* viewer, fastgltf::Image& image) {
    auto getLevelCount = [](int width, int height) -> GLsizei {
      return static_cast<GLsizei>(1 + floor(log2(width > height ? width : height)));
    };

    GLuint texture;
    glCreateTextures(GL_TEXTURE_2D, 1, &texture);
    std::visit(fastgltf::visitor{
        [](auto& arg) {},
        [&](fastgltf::sources::URI& filePath) {
            assert(filePath.fileByteOffset == 0); // We don't support offsets with stbi.
            assert(filePath.uri.isLocalPath()); // We're only capable of loading local files.
            int width, height, nrChannels;

            const std::string path(filePath.uri.path().begin(), filePath.uri.path().end()); // Thanks C++.
            unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);
            glTextureStorage2D(texture, getLevelCount(width, height), GL_RGBA8, width, height);
            glTextureSubImage2D(texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        },
        [&](fastgltf::sources::Vector& vector) {
            int width, height, nrChannels;
            unsigned char* data = stbi_load_from_memory(vector.bytes.data(), static_cast<int>(vector.bytes.size()), &width, &height, &nrChannels, 4);
            glTextureStorage2D(texture, getLevelCount(width, height), GL_RGBA8, width, height);
            glTextureSubImage2D(texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        },
        [&](fastgltf::sources::BufferView& view) {
            auto& bufferView = viewer->asset.bufferViews[view.bufferViewIndex];
            auto& buffer = viewer->asset.buffers[bufferView.bufferIndex];
            // Yes, we've already loaded every buffer into some GL buffer. However, with GL it's simpler
            // to just copy the buffer data again for the texture. Besides, this is just an example.
            std::visit(fastgltf::visitor {
              // We only care about VectorWithMime here, because we specify LoadExternalBuffers, meaning
              // all buffers are already loaded into a vector.
              [](auto& arg) {},
              [&](fastgltf::sources::Vector& vector) {
                  int width, height, nrChannels;
                  unsigned char* data = stbi_load_from_memory(vector.bytes.data() + bufferView.byteOffset, static_cast<int>(bufferView.byteLength), &width, &height, &nrChannels, 4);
                  glTextureStorage2D(texture, getLevelCount(width, height), GL_RGBA8, width, height);
                  glTextureSubImage2D(texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
                  stbi_image_free(data);
              }
          }, buffer.data);
      },
      }, image.data);

    glGenerateTextureMipmap(texture);

    viewer->textures.emplace_back(Texture{ texture });
    return true;
  }

  bool loadMaterial(Viewer* viewer, fastgltf::Material& material) {
    MaterialUniforms uniforms = {};
    uniforms.alphaCutoff = material.alphaCutoff;

    uniforms.baseColorFactor = glm::make_vec4(material.pbrData.baseColorFactor.data());
    if (material.pbrData.baseColorTexture.has_value()) {
      uniforms.flags |= MaterialUniformFlags::HasBaseColorTexture;
    }

    viewer->materials.emplace_back(uniforms);
    return true;
  }

  void drawMesh(Viewer* viewer, size_t meshIndex, glm::mat4 matrix) {
    auto& mesh = viewer->meshes[meshIndex];

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mesh.drawsBuffer);

    glUniformMatrix4fv(viewer->modelMatrixUniform, 1, GL_FALSE, &matrix[0][0]);

    for (auto i = 0U; i < mesh.primitives.size(); ++i) {
      auto& prim = mesh.primitives[i];

      auto& material = viewer->materialBuffers[prim.materialUniformsIndex];
      glBindTextureUnit(0, prim.albedoTexture);
      glBindBufferBase(GL_UNIFORM_BUFFER, 0, material);
      glBindVertexArray(prim.vertexArray);

      glDrawElementsIndirect(prim.primitiveType, prim.indexType,
        reinterpret_cast<const void*>(i * sizeof(Primitive)));
    }
  }

  void drawNode(Viewer* viewer, size_t nodeIndex, glm::mat4 matrix) {
    auto& node = viewer->asset.nodes[nodeIndex];
    matrix = getTransformMatrix(node, matrix);

    if (node.meshIndex.has_value()) {
      drawMesh(viewer, node.meshIndex.value(), matrix);
    }

    for (auto& child : node.children) {
      drawNode(viewer, child, matrix);
    }
  }

  #pragma endregion

  ViewerLayer::ViewerLayer()
    : Layer("glTF Renderer")
  {
  }

  void ViewerLayer::OnAttach()
  {
    // load a shader
    shader.SetBasePath("assets/shaders")
      ->CreateVertexShader("viewer.vert")
      ->CreateFragmentShader("viewer.frag")
      ->Link();

    // load a glTF file
    //fastgltf::Asset model = glTF::Load("src/assets/models/pony_cartoon.glb");

    // vertex buffer objects (VBO)
    // indices (IBO)
    // vertex array object (VAO)

    #pragma region copium and pasta

    // set user pointer
    glfwSetWindowUserPointer(Application::Get().GetGLFWWindow(), &viewer);

    // set glfw callbacks
    glfwSetKeyCallback(Application::Get().GetGLFWWindow(), keyCallback);
    glfwSetCursorPosCallback(Application::Get().GetGLFWWindow(), cursorCallback);
    glfwSetWindowSizeCallback(Application::Get().GetGLFWWindow(), windowSizeCallback);

    // Hide the cursor
    glfwSetInputMode(Application::Get().GetGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Load the glTF file
    auto start = std::chrono::high_resolution_clock::now();
    if (!loadGltf(&viewer, gltfFile)) {
      std::cerr << "Failed to parse glTF" << '\n';
      abort();
    }

    // Add a default material
    auto& defaultMaterial = viewer.materials.emplace_back();
    defaultMaterial.baseColorFactor = glm::vec4(1.0f);
    defaultMaterial.alphaCutoff = 0.0f;
    defaultMaterial.flags = 0;

    // We load images first.
    auto& asset = viewer.asset;
    for (auto& image : asset.images) {
      loadImage(&viewer, image);
    }
    for (auto& material : asset.materials) {
      loadMaterial(&viewer, material);
    }
    for (auto& mesh : asset.meshes) {
      loadMesh(&viewer, mesh);
    }
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
    std::cout << "Loaded glTF file in " << diff.count() << "ms." << '\n';

    // Create the material uniform buffer
    viewer.materialBuffers.resize(viewer.materials.size(), GL_NONE);
    glCreateBuffers(static_cast<GLsizei>(viewer.materials.size()), viewer.materialBuffers.data());
    for (auto i = 0UL; i < viewer.materialBuffers.size(); ++i) {
      glNamedBufferStorage(viewer.materialBuffers[i], static_cast<GLsizeiptr>(sizeof(MaterialUniforms)),
        &viewer.materials[i], GL_MAP_WRITE_BIT);
    }

    viewer.modelMatrixUniform = glGetUniformLocation(shader.Get(), "modelMatrix");
    viewer.viewProjectionMatrixUniform = glGetUniformLocation(shader.Get(), "viewProjectionMatrix");
    glUseProgram(shader.Get());

    {
      // We just emulate the initial sizing of the window with a manual call.
      int width, height;
      glfwGetWindowSize(Application::Get().GetGLFWWindow(), &width, &height);
      windowSizeCallback(Application::Get().GetGLFWWindow(), width, height);
    }

    glEnable(GL_BLEND);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);

    viewer.lastFrame = static_cast<float>(glfwGetTime());

    #pragma endregion

  }

  void ViewerLayer::OnDetach()
  {

    #pragma region copium and pasta

    for (auto& mesh : viewer.meshes) {
      glDeleteBuffers(1, &mesh.drawsBuffer);

      for (auto& prim : mesh.primitives) {
        glDeleteVertexArrays(1, &prim.vertexArray);
      }
    }

    #pragma endregion

    shader.Destroy();

    #pragma region copium and pasta

    glDeleteBuffers(static_cast<GLint>(viewer.buffers.size()), viewer.buffers.data());

    #pragma endregion

  }

  void ViewerLayer::OnUpdate()
  {
    #pragma region copium and pasta

    auto currentFrame = static_cast<float>(glfwGetTime());
    viewer.deltaTime = currentFrame - viewer.lastFrame;
    viewer.lastFrame = currentFrame;

    // Reset the acceleration
    viewer.accelerationVector = glm::vec3(0.0f);

    // Updates the acceleration vector and direction vectors.
    // handle each key in held list
    for (int k : keys_held)
    {
      float dt = 0.1f;
      switch (k)
      {
      case GLFW_KEY_W:
        viewer.accelerationVector += dt * viewer.direction;
        break;
      case GLFW_KEY_S:
        viewer.accelerationVector -= dt * viewer.direction;
        break;
      case GLFW_KEY_D:
        viewer.accelerationVector += dt * glm::normalize(glm::cross(viewer.direction, cameraUp));
        break;
      case GLFW_KEY_A:
        viewer.accelerationVector -= dt * glm::normalize(glm::cross(viewer.direction, cameraUp));
        break;
      case GLFW_KEY_Q:
      case GLFW_KEY_SPACE:
        viewer.accelerationVector += dt * cameraUp;
        break;
      case GLFW_KEY_Z:
      case GLFW_KEY_LEFT_CONTROL:
        viewer.accelerationVector -= dt * cameraUp;
        break;
      default:
        break;
      }
    }

    // Factor the deltaTime into the amount of acceleration
    viewer.velocity += (viewer.accelerationVector * 50.0f) * viewer.deltaTime;
    // Lerp the velocity to 0, adding deceleration.
    viewer.velocity = viewer.velocity + (2.0f * viewer.deltaTime) * (glm::vec3(0.0f) - viewer.velocity);
    // Add the velocity into the position
    viewer.position += viewer.velocity * viewer.deltaTime;
    viewer.viewMatrix = glm::lookAt(viewer.position, viewer.position + viewer.direction, glm::vec3(0.0f, 1.0f, 0.0f));
    updateCameraMatrix(&viewer);

    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    std::size_t sceneIndex = 0;
    if (viewer.asset.defaultScene.has_value())
      sceneIndex = viewer.asset.defaultScene.value();
    auto& scene = viewer.asset.scenes[sceneIndex];
    for (auto& node : scene.nodeIndices) {
      drawNode(&viewer, node, glm::mat4(1.0f));
    }
    
    #pragma endregion

  }

  void ViewerLayer::OnImGuiRender()
  {
    ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoTitleBar |
      ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoScrollbar |
      ImGuiWindowFlags_NoSavedSettings |
      ImGuiWindowFlags_NoInputs
    ;

    ImGui::Begin("Viewer", NULL, window_flags);
    //ImGui::SetWindowPos(ImVec2(0, 0));
    ImGui::Text("glTF Renderer");
    ImGui::End();
  }

}