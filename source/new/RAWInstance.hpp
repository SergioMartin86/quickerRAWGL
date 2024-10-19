#pragma once

#include "../RAWInstanceBase.hpp"
#include <string>
#include <vector>
#include <jaffarCommon/exceptions.hpp>
#include <jaffarCommon/file.hpp>
#include <jaffarCommon/serializers/base.hpp>
#include <jaffarCommon/serializers/contiguous.hpp>
#include <jaffarCommon/deserializers/base.hpp>

#include <engine.h>
#include <graphics.h>
#include <resource.h>
#include <systemstub.h>
#include <util.h>

extern SystemStub *SystemStub_SDL_create();
extern SystemStub *SystemStub_Dummy_create();
extern thread_local SystemStub *SDLStub ;//= System_SDL_create();
extern thread_local SystemStub *dummyStub ;//= System_Dummy_create();
extern thread_local Engine* e;
extern thread_local Graphics *graphics;
extern thread_local DisplayMode *dm;
extern thread_local bool _renderEnabled;
extern thread_local Scaler scaler;

const int graphicsType = GRAPHICS_SOFTWARE;
const bool defaultGraphics = true;
const bool demo3JoyInputs = false;

extern Graphics *createGraphics(int type);

static int getGraphicsType(Resource::DataType type) {
	switch (type) {
	case Resource::DT_15TH_EDITION:
	case Resource::DT_20TH_EDITION:
	case Resource::DT_3DO:
		return GRAPHICS_GL;
	default:
		return GRAPHICS_ORIGINAL;
	}
}

const Language lang = LANG_FR;
static const int DEFAULT_WINDOW_W = 640;
static const int DEFAULT_WINDOW_H = 400;

namespace rawspace
{

class EmuInstance : public EmuInstanceBase
{
 public:

  EmuInstance(const nlohmann::json &config) : EmuInstanceBase(config)
  {
    scaler.name[0] = 0;
    scaler.factor = 1;
  }

  ~EmuInstance()
  {
  }

  int16_t* getThreadsData() const override { return (int16_t*)e->_script._scriptStackCalls; }
  size_t getThreadsDataSize() const override { return VM_NUM_THREADS * sizeof(int16_t*); }
  int16_t* getScriptStackData() const override { return (int16_t*)e->_script._scriptStackCalls; }
  size_t getScriptStackDataSize() const override { return NUM_DATA_FIELDS * VM_NUM_THREADS * sizeof(int16_t*); }

  virtual void initializeImpl(const std::string& gameDataPath) override
  {
    int part = 16001;
    Language lang = LANG_FR;
    _renderEnabled = false;

    dm = new DisplayMode;
    dm->mode   = DisplayMode::WINDOWED;
    dm->width  = DEFAULT_WINDOW_W;
    dm->height = DEFAULT_WINDOW_H;
    dm->opengl = false;

    e = new Engine(gameDataPath.c_str(), part);

    if (graphicsType != GRAPHICS_GL && e->_res.getDataType() == Resource::DT_3DO) {
      // graphicsType = GRAPHICS_SOFTWARE;
      Graphics::_use555 = true;
    }

    graphics = createGraphics(graphicsType);
    if (e->_res.getDataType() == Resource::DT_20TH_EDITION) {
      switch (Script::_difficulty) {
      case DIFFICULTY_EASY:
        debug(DBG_INFO, "Using easy difficulty");
        break;
      case DIFFICULTY_NORMAL:
        debug(DBG_INFO, "Using normal difficulty");
        break;
      case DIFFICULTY_HARD:
        debug(DBG_INFO, "Using hard difficulty");
        break;
      }
    }

    if (e->_res.getDataType() == Resource::DT_15TH_EDITION || e->_res.getDataType() == Resource::DT_20TH_EDITION) {
      if (Script::_useRemasteredAudio) {
        debug(DBG_INFO, "Using remastered audio");
      } else {
        debug(DBG_INFO, "Using original audio");
      }
    }

    g_debugMask = DBG_INFO; // | DBG_VIDEO | DBG_SND | DBG_SCRIPT | DBG_BANK | DBG_SER;


     dummyStub = SystemStub_Dummy_create();
     dummyStub->init(e->getGameTitle(lang), dm);
     e->setSystemStub(dummyStub, graphics);
     e->setup(lang, graphicsType, scaler.name, scaler.factor);
  }

  void initializeVideoOutput() override
  {
    SDLStub = SystemStub_SDL_create();
    e->setSystemStub(SDLStub, graphics);
    SDLStub->init(e->getGameTitle(lang), dm);
    e->setup(lang, graphicsType, scaler.name, scaler.factor);
  }

  void finalizeVideoOutput() override
  {
    SDLStub->fini();
    delete SDLStub;
  }

  void enableRendering() override
  {
    _renderEnabled = true;
  }

  void disableRendering() override
  {
    _renderEnabled = false;
  }

  uint8_t* getPixelsPtr() const override
  {
     return (uint8_t*)graphics->getColorBuffer();
  }
  
  size_t getPixelsSize() const override 
  {
    return graphics->getColorBufferSize();
  }
  
  uint8_t* getPalettePtr() const override
  {
    return (uint8_t*)graphics->getPalettePtr();
  }

  size_t getPaletteSize() const override
  {
    return graphics->getPaletteBufferSize();
  }

  void serializeState(jaffarCommon::serializer::Base& s) const override
  {
    e->saveGameState(s.getOutputDataBuffer());
    s.pushContiguous(nullptr, _stateSize);
  }

  void deserializeState(jaffarCommon::deserializer::Base& d) override
  {
    e->loadGameState((uint8_t*)d.getInputDataBuffer());
    d.popContiguous(nullptr, _stateSize);
  }

  size_t getStateSizeImpl() const override
  {
    return e->saveGameState(nullptr);
  }

  void updateRenderer() override
  {
    graphics->dumpColorBuffer(SDLStub);
    SDLStub->updateScreen();
  }

  inline size_t getDifferentialStateSizeImpl() const override { return getStateSizeImpl(); }

void enableStateBlockImpl(const std::string& block)
  { 
    bool recognizedBlock = false;
    
    // if (block == "NVS") { e->_storeNonVMState = true; recognizedBlock = true; }

    if (recognizedBlock == false) { fprintf(stderr, "Unrecognized block type: %s\n", block.c_str()); exit(-1);}
  };


  void disableStateBlockImpl(const std::string& block)
  { 
    bool recognizedBlock = false;
    
    // if (block == "NVS") { e->_storeNonVMState = false; recognizedBlock = true; }

    if (recognizedBlock == false) { fprintf(stderr, "Unrecognized block type: %s\n", block.c_str()); exit(-1);}
  };

  void doSoftReset() override
  {
  }
  
  void doHardReset() override
  {
  }

  std::string getCoreName() const override { return "QuickerRAW"; }

  uint8_t* getRamPointer() const override { return (uint8_t*)e->_script._scriptVars; }

  void advanceStateImpl(const jaffar::input_t &input) override
  {
    e->run();
  }

  private:

};

} // namespace rawspace