#pragma once

#include <aw/engine/runtime/state.hpp>

#include <aw/engine/window.hpp>

#include <aw/graphics/core/camera.hpp>
#include <aw/graphics/core/frameBuffer.hpp>
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

#include <aw/runtime/scene/sceneNode.hpp>

#include <array>
#include <vector>

namespace aw
{
class Engine;
} // namespace aw

class TestState : public aw::State
{
public:
  TestState(aw::StateMachine& stateMachine, aw::Engine& engine);
  ~TestState();

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

  aw::Vec2 mOldMousePos;
  bool mMouseInit{false};
};
