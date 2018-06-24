#include <iostream>

#include <aw/engine/engine.hpp>
#include <aw/utils/log.hpp>

#include <aw/graphics/core/image.hpp>
#include <aw/utils/assetInputStream.hpp>
#include <aw/utils/path.hpp>

#include "testState.hpp"

#include <string>

int main()
{
  aw::LOG_INITIALIZE();
  aw::Engine engine;
  //  engine.run();
  aw::priv::saveSettings(engine.getSettings());

  LogTemp() << (aw::path::getAssetPath() + "assets/test.txt");
  aw::AssetInputStream testFile("test.txt");
  if (testFile.isOpen())
  {
    std::string line;
    while (std::getline(testFile, line))
      LogTemp() << "Line: " << line;
  }
  else
    LogTemp() << "File not found!";
  aw::Image img;
  aw::AssetInputStream imgFile("image.png");
  if (imgFile.isOpen())
  {
    LogTemp() << "File open!";
    img.loadFromStream(imgFile);
    LogTemp() << "Image found: " << img.getWidth() << "|" << img.getHeight() << ": " << img.getNumChannels();
  }
  else
  {
    LogTemp() << "File not found!";
  }

  engine.getStateMachine().pushState(std::make_shared<TestState>(engine.getStateMachine(), engine));
  engine.run();
  return 0;
}
