#include <Pokitto.h>
#include <LibAudio>
#include <File>
#include <miloslav.h>
#include <Tilemap.hpp>
#include <SDFileSystem.h>
#include "sprites.h"
#include "Smile.h"
#include "maps.h"

using PC = Pokitto::Core;
using PD = Pokitto::Display;
using PB = Pokitto::Buttons;

// stuff for adpcm playing
uint8_t ADPCMBuffer[4096]; // 44100 sample rate uses 1024 byte packets I think. So this could be smaller
File wavFile;
const char* adpcmFilename = "/rockandroll.wav";
#include "adpcm.h"
uint8_t myByte=0;
int byteCount=0;
int nibbleCount=0;
int bufferOffset = 0;
int currentBuffer = 0;
int completeBuffer = 0;
int audioCount = 0;
////////////////////////////////////////////


Tilemap tileMap;
int camX = 64, camY = 64, speed = 3, recolor = 0;
Sprite player;
auto playerWidth = player.getFrameWidth();
auto playerHeight = player.getFrameHeight();
auto playerX = LCDWIDTH / 2 - playerWidth / 2;
auto playerY = LCDHEIGHT / 2 - playerHeight / 2;

constexpr auto tune = SIMPLE_TUNE(C#/4, D/4, E/2).tempo(170);

void init() {
    PD::loadRGBPalette(miloslav);

    tileMap.set(gardenPath[0], gardenPath[1], gardenPath + 2);
    for (int i = 0; i < sizeof(tiles) / (POK_TILE_W * POK_TILE_H); i++)
        tileMap.setTile(i, POK_TILE_W, POK_TILE_H, tiles + i * POK_TILE_W * POK_TILE_H);

    player.play(dude, Dude::walkS);
    
    readADPCMHeader();
    
    // read first part ot .wav file.
    wavFile.read(&ADPCMBuffer[0], wavHeader.block_align);
    
    Audio::play(+[](Audio::u32 t)->Audio::u8{ 

        signed short sixteenBit;
    	if(byteCount == wavHeader.block_align){
    
    		byteCount = 4;
    		bufferOffset = wavHeader.block_align*(1-completeBuffer);
    
            prevsample = *reinterpret_cast<const uint16_t*>(&ADPCMBuffer[bufferOffset]);
            prevsample = (signed short) prevsample; // is this needed?
            previndex = ADPCMBuffer[bufferOffset + 2];
    
    	}else{
    
        	if(nibbleCount == 0){
        	    nibbleCount = 1;
                myByte = ADPCMBuffer[bufferOffset + byteCount];
        		sixteenBit = ImaAdpcmDecode(myByte & 15);
        	}else{
        		byteCount = byteCount + 1;	
        	    nibbleCount = 0;
        		sixteenBit = ImaAdpcmDecode((myByte >> 4));
                if(++audioCount >= wavHeader.block_align * 2){
                    audioCount=0;
                }
                currentBuffer = audioCount / wavHeader.block_align;
        	}
    	}    

        return(((sixteenBit+32768)>>8)&255);

    });

}

void update() {

    int oldX = camX;
    int oldY = camY;

    if (PB::rightBtn()) {
        camX += speed;
        if (player.animation != Dude::walkE)
            player.play(dude, Dude::walkE);
    } else if (PB::leftBtn()) {
        camX -= speed;
        if (player.animation != Dude::walkW)
            player.play(dude, Dude::walkW);
    }

    if (PB::upBtn()) {
        camY -= speed;
        if (player.animation != Dude::walkN)
            player.play(dude, Dude::walkN);
    } else if (PB::downBtn()) {
        camY += speed;
        if (player.animation != Dude::walkS)
            player.play(dude, Dude::walkS);
    }

    int tileX = (camX + playerX + PROJ_TILE_W / 2) / PROJ_TILE_W;
    int tileY = (camY + playerY + playerHeight) / PROJ_TILE_H;
    auto tile = gardenPathEnum(tileX, tileY);

    if (tile & Collide) {
        camX = oldX;
        camY = oldY;
    }

    if (tile & WalkOnGrass) {
        recolor++;
    } else {
        recolor = 0;
    }

    tileMap.draw(-camX, -camY);
    PD::drawSprite(-camX, -camY, Smile, false, false, recolor);
    player.draw(playerX, playerY, false, false, recolor);
    
    
    
    if( currentBuffer != completeBuffer){
    	wavFile.read(&ADPCMBuffer[wavHeader.block_align*completeBuffer], wavHeader.block_align);
        completeBuffer = currentBuffer;
    }

}

