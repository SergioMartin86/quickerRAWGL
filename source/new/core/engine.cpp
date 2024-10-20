
/*
 * Another World engine rewrite
 * Copyright (C) 2004-2005 Gregory Montoir (cyx@users.sourceforge.net)
 */

#include "engine.h"
#include "file.h"
#include "graphics.h"
#include "resource_nth.h"
#include "systemstub.h"
#include "util.h"

thread_local Graphics *graphics;
thread_local DisplayMode *dm;
extern thread_local bool _renderEnabled;

Engine::Engine(const char *dataDir, int partNum)
	: _graphics(0), _stub(0), _script(&_res, &_ply, &_vid), _res(&_vid, dataDir),
	_ply(&_res), _vid(&_res), _partNum(partNum) {
	_res.detectVersion();
}

static const int _restartPos[36 * 2] = {
	16008,  0, 16001,  0, 16002, 10, 16002, 12, 16002, 14,
	16003, 20, 16003, 24, 16003, 26, 16004, 30, 16004, 31,
	16004, 32, 16004, 33, 16004, 34, 16004, 35, 16004, 36,
	16004, 37, 16004, 38, 16004, 39, 16004, 40, 16004, 41,
	16004, 42, 16004, 43, 16004, 44, 16004, 45, 16004, 46,
	16004, 47, 16004, 48, 16004, 49, 16006, 64, 16006, 65,
	16006, 66, 16006, 67, 16006, 68, 16005, 50, 16006, 60,
	16007, 0
};

void Engine::setSystemStub(SystemStub *stub, Graphics *pgraphics) {
	_stub = stub;
	_script._stub = stub;
	graphics = pgraphics;
}

void Engine::run() {
	switch (_state) {
	case kStateLogo3DO:
		doThreeScreens();
		scrollText(0, 380, Video::_noteText3DO);
		playCinepak("Logo.Cine");
		playCinepak("Spintitle.Cine");
		break;
	case kStateTitle3DO:
		titlePage();
		break;
	case kStateEnd3DO:
		doEndCredits();
		break;
	case kStateGame:
		_script.setupTasks();
		_script.updateInput();
		processInput();
		_script.runTasks();
		if (_res.getDataType() == Resource::DT_3DO) {
			switch (_res._nextPart) {
			case 16009:
				_state = kStateEnd3DO;
				break;
			case 16000:
				_state = kStateTitle3DO;
				break;
			}
		}
		break;
	}
}

void Engine::setup(Language lang, int graphicsType, const char *scalerName, int scalerFactor) {
	_graphics = graphics;
	_vid._graphics = _graphics;
	int w = GFX_W * scalerFactor;
	int h = GFX_H * scalerFactor;
	if (_res.getDataType() != Resource::DT_3DO) {
		_vid._graphics->_fixUpPalette = FIXUP_PALETTE_REDRAW;
	}
	_vid.init();
	if (scalerFactor > 1) {
		_vid.setScaler(scalerName, scalerFactor);
	}
	_res._lang = lang;
	_res.allocMemBlock();
	_res.readEntries();
	_res.dumpEntries();
	const bool isNth = !Graphics::_is1991 && (_res.getDataType() == Resource::DT_15TH_EDITION || _res.getDataType() == Resource::DT_20TH_EDITION);
	if (isNth) {
		// get HD background bitmaps resolution
		_res._nth->getBitmapSize(&w, &h);
	}
	_graphics->init(w, h);
	if (isNth) {
		_res.loadFont();
		_res.loadHeads();
	} else {
		_vid.setDefaultFont();
	}
	_script.init();
	switch (_res.getDataType()) {
	case Resource::DT_DOS:
	case Resource::DT_AMIGA:
	case Resource::DT_ATARI:
	case Resource::DT_ATARI_DEMO:
		switch (lang) {
		case LANG_FR:
			_vid._stringsTable = Video::_stringsTableFr;
			break;
		case LANG_US:
		default:
			_vid._stringsTable = Video::_stringsTableEng;
			break;
		}
		break;
	case Resource::DT_WIN31:
	case Resource::DT_15TH_EDITION:
	case Resource::DT_20TH_EDITION:
		break;
	case Resource::DT_3DO:
		break;
	}
#ifndef BYPASS_PROTECTION
	switch (_res.getDataType()) {
	case Resource::DT_DOS:
		if (!_res._hasPasswordScreen) {
			break;
		}
		/* fall-through */
	case Resource::DT_AMIGA:
	case Resource::DT_ATARI:
	case Resource::DT_WIN31:
		_partNum = kPartCopyProtection;
		break;
	default:
		break;
	}
#endif
	if (_res.getDataType() == Resource::DT_3DO && _partNum == kPartIntro) {
		_state = kStateLogo3DO;
	} else {
		_state = kStateGame;
		const int num = _partNum;
		if (num < 36) {
			_script.restartAt(_restartPos[num * 2], _restartPos[num * 2 + 1]);
		} else {
			_script.restartAt(num);
		}
	}
}

void Engine::finish() {
	_graphics->fini();
	_ply.stop();
	_res.freeMemBlock();
}

uint8_t saveData[65535];

void Engine::processInput() {
	if (_stub->_pi.save == true)
	{
		saveGameState(saveData);
		_stub->_pi.save = false;
	}

	if (_stub->_pi.load == true)
	{
		loadGameState(saveData);
		_stub->_pi.load = false;
	}

	if (_stub->_pi.fastMode) {
		_script._fastMode = !_script._fastMode;
		_stub->_pi.fastMode = false;
	}
	if (_stub->_pi.screenshot) {
		_vid.captureDisplay();
		_stub->_pi.screenshot = false;
	}
}

void Engine::doThreeScreens() {
	_script.snd_playMusic(1, 0, 0);
	static const int bitmaps[] = { 67, 68, 69, -1 };
	for (int i = 0; bitmaps[i] != -1 && !_stub->_pi.quit; ++i) {
		if (_renderEnabled == true) _res.loadBmp(bitmaps[i]);
		if (_renderEnabled == true) _vid.updateDisplay(0, _stub);
		while (!_stub->_pi.quit) {
			_stub->processEvents();
			if (_stub->_pi.action) {
				_stub->_pi.action = false;
				break;
			}
			if (_renderEnabled == true) _stub->sleep(50);
		}
	}
	_state = kStateTitle3DO;
}

void Engine::doEndCredits() {
	scrollText(0, 380, Video::_endText3DO);
	_script.snd_playMusic(0, 0, 0);
	playCinepak("ootw2.cine");
	_state = kStateTitle3DO;
}

void Engine::playCinepak(const char *name) {
}

void Engine::scrollText(int a, int b, const char *text) {
}

void Engine::titlePage() {
	if (_renderEnabled == true)  _res.loadBmp(70);
	static const int kCursorColor = 0;
if (_renderEnabled == true) 	_vid.setPaletteColor(kCursorColor, 255, 0, 0);
	static const int yPos[] = { 97, 123, 149 };
	int y = 0;
	while (!_stub->_pi.quit) {
		if (_renderEnabled == true) _vid.copyPage(0, 1, 0);
		if (_renderEnabled == true) _vid.drawRect(1, kCursorColor, 97, yPos[y], 210, yPos[y + 1]);
		_stub->processEvents();
		if (_stub->_pi.dirMask & PlayerInput::DIR_DOWN) {
			_stub->_pi.dirMask &= ~PlayerInput::DIR_DOWN;
			_partNum = kPartPassword;
			y = 1;
		}
		if (_stub->_pi.dirMask & PlayerInput::DIR_UP) {
			_stub->_pi.dirMask &= ~PlayerInput::DIR_UP;
			_partNum = kPartIntro;
			y = 0;
		}
		if (_stub->_pi.action) {
			_stub->_pi.action = false;
			_script.restartAt(_partNum);
			break;
		}
		if (_renderEnabled == true) _vid.updateDisplay(1, _stub);
		if (_renderEnabled == true) _stub->sleep(50);
	}
	_state = kStateGame;
}

size_t Engine::saveGameState(uint8_t* stateData)
{
  size_t pos = 0;

		if (stateData != nullptr) memcpy(&stateData[pos], &_script._scriptVars,       sizeof(_script._scriptVars));       pos += sizeof(_script._scriptVars);
		if (stateData != nullptr) memcpy(&stateData[pos], &_script._scriptStackCalls, sizeof(_script._scriptStackCalls)); pos += sizeof(_script._scriptStackCalls);
		if (stateData != nullptr) memcpy(&stateData[pos], &_script._scriptTasks,      sizeof(_script._scriptTasks));      pos += sizeof(_script._scriptTasks);
		if (stateData != nullptr) memcpy(&stateData[pos], &_script._scriptStates,     sizeof(_script._scriptStates));     pos += sizeof(_script._scriptStates);

		return pos;
}

void Engine::loadGameState(const uint8_t* stateData)
{
	 size_t pos = 0;

		if (stateData != nullptr) memcpy(&_script._scriptVars,       &stateData[pos],  sizeof(_script._scriptVars));       pos += sizeof(_script._scriptVars);
		if (stateData != nullptr) memcpy(&_script._scriptStackCalls, &stateData[pos],  sizeof(_script._scriptStackCalls)); pos += sizeof(_script._scriptStackCalls);
		if (stateData != nullptr) memcpy(&_script._scriptTasks,      &stateData[pos],  sizeof(_script._scriptTasks));      pos += sizeof(_script._scriptTasks);
		if (stateData != nullptr) memcpy(&_script._scriptStates,     &stateData[pos],  sizeof(_script._scriptStates));     pos += sizeof(_script._scriptStates);
}


thread_local Engine* e;