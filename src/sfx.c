#include "sfx.h"

#include "fk-engine-core/audio.h"
#include "fk-engine-core/util.h"

void loadSFX()
{
    loadSampleIndex(SFX_LEVER, RESOURCE("res/sfx/click20.ogg"));
    loadSampleIndex(SFX_TNT, RESOURCE("res/sfx/explosion.ogg"));
    loadSampleIndex(SFX_DOOR, RESOURCE("res/sfx/wooded_box_open.ogg"));
    loadSampleIndex(SFX_MENU, RESOURCE("res/sfx/click1.ogg"));
    loadSampleIndex(SFX_PRESSURE_PLATE, RESOURCE("res/sfx/click27.ogg"));
    loadSampleIndex(SFX_PRESSURE_PLATE_OFF, RESOURCE("res/sfx/click28.ogg"));
    //Noteblock notes
    loadSampleIndex(SFX_NOTEBLOCK_0, RESOURCE("res/sfx/noteblock/key05.ogg"));
    loadSampleIndex(SFX_NOTEBLOCK_0 + 1, RESOURCE("res/sfx/noteblock/key06.ogg"));
    loadSampleIndex(SFX_NOTEBLOCK_0 + 2, RESOURCE("res/sfx/noteblock/key07.ogg"));
    loadSampleIndex(SFX_NOTEBLOCK_0 + 3, RESOURCE("res/sfx/noteblock/key08.ogg"));
    loadSampleIndex(SFX_NOTEBLOCK_0 + 4, RESOURCE("res/sfx/noteblock/key09.ogg"));
    loadSampleIndex(SFX_NOTEBLOCK_0 + 5, RESOURCE("res/sfx/noteblock/key10.ogg"));
    loadSampleIndex(SFX_NOTEBLOCK_0 + 6, RESOURCE("res/sfx/noteblock/key11.ogg"));
    loadSampleIndex(SFX_NOTEBLOCK_0 + 7, RESOURCE("res/sfx/noteblock/key12.ogg"));
    //Noteblock notes end
}