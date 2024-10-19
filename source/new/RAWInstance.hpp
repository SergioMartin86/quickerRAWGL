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
extern thread_local SystemStub *stub ;//= System_SDL_create();
extern thread_local Engine* e;
extern thread_local Graphics *graphics;
extern thread_local DisplayMode *dm;

const int graphicsType = GRAPHICS_SOFTWARE;

extern Graphics *createGraphics(int type);

struct Scaler {
	char name[32];
	int factor;
};

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
  }

  void initializeVideoOutput() override
  {
  	Scaler scaler;
    scaler.name[0] = 0;
    scaler.factor = 1;
    bool defaultGraphics = true;
    bool demo3JoyInputs = false;
    g_debugMask = DBG_INFO; // | DBG_VIDEO | DBG_SND | DBG_SCRIPT | DBG_BANK | DBG_SER;


    stub = SystemStub_SDL_create();
    stub->init(e->getGameTitle(lang), dm);
    e->setSystemStub(stub, graphics);
    // if (demo3JoyInputs && e->_res.getDataType() == Resource::DT_DOS) {
    //   e->_res.readDemo3Joy();
    // }
    e->setup(lang, graphicsType, scaler.name, scaler.factor);
  }

  void finalizeVideoOutput() override
  {
    stub->fini();
    delete stub;
  }

  void enableRendering() override
  {
    // e->vm._doRendering = true;
    // e->video._doRendering = true;
  }

  void disableRendering() override
  {
    // e->vm._doRendering = false;
    // e->video._doRendering = false;
  }

  uint8_t* getPixelsPtr() const override
  {
     return nullptr;
    //  return stub->getPixelsPtr();
  }
  
  size_t getPixelsSize() const override 
  {
    return 0;
    //  return stub->getPixelsSize();
  }
  
  uint8_t* getPalettePtr() const override
  {
    return nullptr;
    //  return stub->getPalettePtr();
  }

  size_t getPaletteSize() const override
  {
    return 0;
    //  return stub->getPaletteSize();
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
    // stub->applyPalette();
    // stub->updateRenderer();
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
		// e->_script.checkThreadRequests();

		// e->_script.inp_updatePlayer(input.buttonUp, input.buttonDown, input.buttonLeft, input.buttonRight, input.buttonFire);

		// e->_script.hostFrame();
  }

  private:

};

} // namespace rawspace