#pragma once

#include <aw/engine/runtime/state.hpp>

#include <aw/engine/window.hpp>

#include <aw/graphics/core/camera.hpp>
#include <aw/graphics/core/frameBuffer.hpp>
#include <aw/graphics/core/intermediateRenderer.hpp>
#include <aw/graphics/core/shaderProgram.hpp>
#include <aw/graphics/core/texture2D.hpp>
#include <aw/graphics/core/vertexArrayObject.hpp>
#include <aw/graphics/core/vertexBuffer.hpp>

#include <aw/graphics/3d/directionalLight.hpp>

#include <aw/graphics/3d/mesh.hpp>
#include <aw/graphics/3d/meshInstance.hpp>

#include <aw/graphics/3d/orbitCameraController.hpp>

#include <aw/runtime/managers/MeshAnimationManager.hpp>
#include <aw/runtime/managers/MeshManager.hpp>
#include <aw/runtime/managers/TextureManager.hpp>

#include <aw/runtime/renderers/meshRenderer.hpp>

#include <aw/utils/math/vector.hpp>
#include <aw/utils/spatial/AABBTriangleIntersector.hpp>
#include <aw/utils/spatial/octree.hpp>

#include <aw/runtime/scene/sceneNode.hpp>

#include <array>
#include <vector>

namespace aw
{
class Engine;
} // namespace aw

struct MeshTriangle
{
  const aw::MeshObject* obj;
  const unsigned* triangle;
};

struct Intersector
{
  static int testCount;
  bool operator()(aw::AABB& box, const MeshTriangle& triangle)
  {
    aw::AABBTriangleIntersector i;
    testCount++;
    return i(box, triangle.obj->vertices[triangle.triangle[0]].position,
             triangle.obj->vertices[triangle.triangle[1]].position,
             triangle.obj->vertices[triangle.triangle[2]].position);
  }
};
class TestState : public aw::State
{
public:
  TestState(aw::StateMachine& stateMachine, aw::Engine& engine);
  virtual ~TestState();

  void update(float delta) final;
  void render() final;

  void processEvent(const aw::WindowEvent& event);

private:
private:
  aw::Engine& mEngine;
  aw::Window::EventListenerID mEventListenerId;

  aw::TextureManager mTextureManager;
  aw::MeshManager mMeshManager;
  aw::MeshAnimationManager mMeshAnimationManager;

  aw::FrameBuffer mFrameBuffer;

  aw::ShaderProgram mMeshShader;
  aw::ShaderProgram mMeshShadowShader;

  aw::Camera mCamera;
  aw::Camera mLightCam;
  aw::DirectionalLight mDirLight;

  aw::SceneNode mSceneNode;
  aw::MeshRenderer mMeshRenderer;

  aw::OrbitCameraController mCamController;

  aw::IntermediateRenderer<aw::VertexPosColor> mDebugRenderer;

  aw::Vec2 mOldMousePos;
  bool mMouseInit{false};

  std::unique_ptr<aw::Octree<MeshTriangle, Intersector>> mMapOctree;
};
