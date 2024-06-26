#include "Renderer/buffer.h"

#include "Renderer/OpenGL/openglbuffer.h"

namespace FlexEngine
{

  // In the future, support for other graphics APIs can be added here.

  VertexArray* VertexArray::Create(unsigned int binding_point)
  {
    return new OpenGLVertexArray(binding_point);
  }

  VertexBuffer* VertexBuffer::Create(Vertex* vertices, std::size_t size, unsigned int binding_point)
  {
    return new OpenGLVertexBuffer(vertices, size, binding_point);
  }

  IndexBuffer* IndexBuffer::Create(unsigned int* indices, GLsizei count, unsigned int binding_point)
  {
    return new OpenGLIndexBuffer(indices, count, binding_point);
  }

}
