#include "testState.hpp"

#include <aw/engine/engine.hpp>
#include <aw/opengl/opengl.hpp>
#include <aw/utils/assetInputStream.hpp>

#include <aw/graphics/core/geometry.hpp>
#include <aw/graphics/core/image.hpp>
#include <aw/graphics/core/postProcessRenderer.hpp>

#include <aw/runtime/loaders/assimpLoader.hpp>
#include <aw/runtime/scene/meshNode.hpp>
#include <aw/runtime/scene/sceneLoader.hpp>
#include <aw/runtime/scene/sceneWriter.hpp>

#include <aw/utils/math/constants.hpp>
using namespace aw::constantsF;
#include <aw/utils/log.hpp>
DEFINE_LOG_CATEGORY(Test, aw::log::Debug, TestState)

#include <glm/gtc/matrix_transform.hpp>

#include <cassert>
#include <functional>
#include <stack>

#include "assimpImporter.hpp"

int Intersector::testCount = 0;

TestState::TestState(aw::StateMachine& stateMachine, aw::Engine& engine)
    : aw::State(stateMachine), mEngine(engine),
      // mCamera(aw::Camera::createOrthograpic(0, engine.getWindow().getSize().x, 0, engine.getWindow().getSize().y))
      mCamera(aw::Camera::createPerspective(800.f / 600.f, 60.f * TO_RAD)), mCamController(&mCamera),
      mLightCam(aw::Camera::createOrthograpic(-50.f, 50.f, -50.f, 50.f, 0.1, 500.f))
{
  mEventListenerId =
      mEngine.getWindow().registerEventListener(std::bind(&TestState::processEvent, this, std::placeholders::_1));

  glClearColor(0.1f, 0.25f, 0.25f, 1.f);
  mFrameBuffer.create(2048, 2048, 16, 0);
#ifdef AW_DESKTOP
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
#endif
  mLightCam.setPosition({0.f, 70.f, 30.f});
  mLightCam.setRotationEuler({-aw::constantsF::PI_2 * 0.75, 0.f, 0.f});

  using Type = aw::ShaderStage::Type;
  auto vMesh = aw::ShaderStage::loadFromAssetFile(Type::Vertex, "shaders/mesh.vert");
  auto fTexLight = aw::ShaderStage::loadFromAssetFile(Type::Fragment, "shaders/textureLight.frag");

  mMeshShader.link(*vMesh, *fTexLight);

  auto fShadow = aw::ShaderStage::loadFromAssetFile(aw::ShaderStage::Fragment, "shaders/texture.frag");
  mMeshShadowShader.link(*vMesh, *fShadow);

  auto vSimple = aw::ShaderStage::loadFromAssetFile(Type::Vertex, "shaders/simple.vert");
  auto fColor = aw::ShaderStage::loadFromAssetFile(Type::Fragment, "shaders/color.frag");
  mSingleColorShader.link(*vSimple, *fColor);
  mSingleColorShader.bind();
  mSingleColorShader.setUniform("fillColor", aw::Vec3{1.f, 0.f, 0.f});
  mSingleColorShader.unbind();

  mCamController.setViewAtPoint({0.f, 0.5f, 10.f});
  mCamController.setDistanceToViewPoint(10.f);

  mDirLight.color = aw::Colors::WHITE;
  mDirLight.energy = 0.75f;
  mDirLight.direction = aw::Vec3(0.25f, -0.5f, 0.f);
  mDirLight.direction = glm::normalize(mDirLight.direction);

  mCamController.setRotationHorizontal(0.61f);
  mCamController.setRotationVertical(3.19f);

  aw::SceneLoader::loadFromAssetFile("testMap.json", mSceneNode, mTextureManager, mMeshManager, mMeshAnimationManager);

  std::stack<aw::SceneNode*> searchStack;
  searchStack.push(&mSceneNode);
  while (!searchStack.empty())
  {
    auto* it = searchStack.top();
    searchStack.pop();
    auto* meshNode = dynamic_cast<aw::MeshNode*>(it);
    if (meshNode)
      mMeshRenderer.registerMesh(meshNode);
    for (auto& child : it->getChildren())
      searchStack.push(child);
  }

  auto* mapNode = (aw::MeshNode*)mSceneNode.findNodeByName("map");
  if (mapNode)
  {
    const auto& mesh = mapNode->meshInstance().getMesh();
    auto bounds = mesh.getBounds();
    mMapOctree = std::make_unique<aw::Octree<MeshTriangle, Intersector>>(bounds, 100, 5);
    Intersector intersector;
    for (int i = 0; i < mesh.getObjectCount(); i++)
    {
      const auto& meshObj = mesh.getObject(i);
      for (int j = 0; j < meshObj.indices.size(); j += 3)
      {
        MeshTriangle t{&meshObj, &meshObj.indices[j]};
        mMapOctree->addElement(t, intersector);
      }
    }
    LogTemp() << "Generating octree took: " << intersector.testCount << " AABB triangle tests";
  }
  else
  {
    LogTemp() << "Map not found!";
  }
}

TestState::~TestState()
{
  mEngine.getWindow().unregisterEventListener(mEventListenerId);
}

void TestState::update(float delta)
{
  // LogTest() << delta;
  // mCamera.rotateEuler(aw::Vec3{0.f, 0.f, 3.14f} * delta);
  // mCamController.rotateVertical(PI_2 * delta);
  // mCamController.setRotationHorizontal(PI_4 * 0.5f);
  mCamController.update(delta);
  // LogTest() << "Position: " << mCamera.getPosition();
  // LogTest() << "Cam controller: " << mCamController.getHorizontalRotation() << " | "
  //          << mCamController.getVerticalRotation();
  // LogTest() << "Rotation: " << mCamera.getRotationEuler() * TO_DEG;
  // mSceneNode.getChildren().back()->localTransform().move(aw::Vec3{10.f, 0.f, 0.f} * delta);
  std::stack<aw::SceneNode*> searchStack;
  searchStack.push(&mSceneNode);
  while (!searchStack.empty())
  {
    auto* it = searchStack.top();
    searchStack.pop();
    auto* meshNode = dynamic_cast<aw::MeshNode*>(it);
    if (meshNode)
      meshNode->meshInstance().update(delta);
    for (auto& child : it->getChildren())
      searchStack.push(child);
  }

  // Octree test
  auto* sphereNode = (aw::MeshNode*)mSceneNode.findNodeByName("sphere");
  if (sphereNode)
  {
    auto localBounds = sphereNode->meshInstance().getMesh().getBounds();
    auto globalBounds = aw::AABB::createFromTransform(localBounds, sphereNode->getGlobalTransform());

    auto print = [](MeshTriangle t) {
      return;
      LogTemp() << "Collison with: " << t.triangle[0];
    };
    Intersector inter;
    inter.testCount = 0;
    mMapOctree->traverseElements(print, globalBounds, inter);
    LogTemp() << "Traversing octree for cube: " << inter.testCount << " AABB triangle tests";
  }
}

void TestState::render()
{
  mFrameBuffer.bind();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  // glEnable(GL_CULL_FACE);

  mMeshRenderer.renderShadowMap(mLightCam, mMeshShadowShader, mDirLight);

  mFrameBuffer.unbind();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // aw::PostProcessRenderer::render(mFrameBuffer.getColorTexture());
  mMeshRenderer.renderForwardPassWithShadow(mCamera, mLightCam, mFrameBuffer.getDepthTexture(), mMeshShader, mDirLight);

  mSingleColorShader.bind();
  mSingleColorShader.setUniform("mvp_matrix", mCamera.getVPMatrix());
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glDisable(GL_CULL_FACE);
  mDebugRenderer.clear();
  auto* sphereNode = (aw::MeshNode*)mSceneNode.findNodeByName("sphere");
  // auto bounds = mMapOctree->getBounds();

  // Iterate over all nodes
  mMapOctree->traverseNodes(
      [this](const MeshOctree& node) {
        auto bounds = node.getBounds();
        auto boundsCube = aw::geo::cube<aw::VertexPos>(bounds.getCenter(), bounds.getSize());
        mDebugRenderer.addVertices(boundsCube.begin(), boundsCube.end());
      },
      mMapOctree->getBounds());

  mDebugRenderer.render();
  // glEnable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  // glDisable(GL_DEPTH_TEST);
  // aw::PostProcessRenderer::render(mFrameBuffer.getColorTexture());
  // glEnable(GL_DEPTH_TEST);
}

void TestState::processEvent(const aw::WindowEvent& event)
{
  if (event.type == aw::WindowEvent::Closed)
    mEngine.terminate();

  if (event.type == aw::WindowEvent::MouseLeft)
    mMouseInit = false;
  if (event.type == aw::WindowEvent::MouseMoved)
  {
    if (!mMouseInit)
    {
      mOldMousePos = {event.mouseMove.x, event.mouseMove.y};
      mMouseInit = true;
    }
    else
    {
      aw::Vec2 currentPos{event.mouseMove.x, event.mouseMove.y};
      mCamController.rotateVertical(mOldMousePos.x - currentPos.x);
      mCamController.rotateHorizontal(currentPos.y - mOldMousePos.y);
      mOldMousePos = currentPos;
    }
  }

  if (event.type == aw::WindowEvent::TouchEnded)
    mMouseInit = false;
  if (event.type == aw::WindowEvent::TouchMoved)
  {
    if (!mMouseInit)
    {
      mOldMousePos = {event.touch.x, event.touch.y};
      mMouseInit = true;
    }
    else
    {
      aw::Vec2 currentPos{event.touch.x, event.touch.y};
      mCamController.rotateVertical(mOldMousePos.x - currentPos.x);
      mCamController.rotateHorizontal(currentPos.y - mOldMousePos.y);
      mOldMousePos = currentPos;
    }
  }
  if (event.type == aw::WindowEvent::MouseWheelScrolled)
  {
    mCamController.zoom(event.mouseWheelScroll.delta);
  }
  if (event.type == aw::WindowEvent::KeyPressed)
  {
    const float moveFactor = 10.f;
    if (event.key.code == sf::Keyboard::W)
      mCamController.rotateHorizontal(moveFactor);
    if (event.key.code == sf::Keyboard::S)
      mCamController.rotateHorizontal(-moveFactor);
    if (event.key.code == sf::Keyboard::A)
      mCamController.rotateVertical(moveFactor);
    if (event.key.code == sf::Keyboard::D)
      mCamController.rotateVertical(-moveFactor);
    if (event.key.code == sf::Keyboard::Escape)
      mEngine.terminate();

    auto* sphereNode = (aw::MeshNode*)mSceneNode.findNodeByName("sphere");
    if (sphereNode)
    {
      if (event.key.code == sf::Keyboard::Left)
        sphereNode->localTransform().move({-0.5f, 0.f, 0.f});
      if (event.key.code == sf::Keyboard::Right)
        sphereNode->localTransform().move({0.5f, 0.f, 0.f});
      if (event.key.code == sf::Keyboard::Up)
        sphereNode->localTransform().move({0.f, 0.5f, 0.f});
      if (event.key.code == sf::Keyboard::Down)
        sphereNode->localTransform().move({0.f, -0.5f, 0.f});

      LogTemp() << sphereNode->localTransform().getPosition();
    }
  }
}
