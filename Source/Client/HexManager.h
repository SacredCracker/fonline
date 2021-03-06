//      __________        ___               ______            _
//     / ____/ __ \____  / (_)___  ___     / ____/___  ____ _(_)___  ___
//    / /_  / / / / __ \/ / / __ \/ _ \   / __/ / __ \/ __ `/ / __ \/ _ \
//   / __/ / /_/ / / / / / / / / /  __/  / /___/ / / / /_/ / / / / /  __/
//  /_/    \____/_/ /_/_/_/_/ /_/\___/  /_____/_/ /_/\__, /_/_/ /_/\___/
//                                                  /____/
// FOnline Engine
// https://fonline.ru
// https://github.com/cvet/fonline
//
// MIT License
//
// Copyright (c) 2006 - present, Anton Tsvetinskiy aka cvet <cvet@tut.by>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

// Todo: move HexManager to MapView?

#pragma once

#include "Common.h"

#include "CacheStorage.h"
#include "ClientScripting.h"
#include "CritterView.h"
#include "EffectManager.h"
#include "Entity.h"
#include "GeometryHelper.h"
#include "ItemHexView.h"
#include "ItemView.h"
#include "MapLoader.h"
#include "ProtoManager.h"
#include "ResourceManager.h"
#include "Settings.h"
#include "SpriteManager.h"
#include "Sprites.h"

constexpr int MAX_FIND_PATH = 600;

struct ViewField
{
    int HexX {};
    int HexY {};
    int ScrX {};
    int ScrY {};
    float ScrXf {};
    float ScrYf {};
};

struct LightSource
{
    ushort HexX {};
    ushort HexY {};
    uint ColorRGB {};
    uchar Distance {};
    uchar Flags {};
    int Intensity {};
    short* OffsX {};
    short* OffsY {};
    short LastOffsX {};
    short LastOffsY {};
};

struct Field
{
    struct Tile
    {
        AnyFrames* Anim {};
        short OffsX {};
        short OffsY {};
        uchar Layer {};
    };
    using TileVec = vector<Tile>;

    struct FlagsType
    {
        bool ScrollBlock : 1;
        bool IsWall : 1;
        bool IsWallTransp : 1;
        bool IsScen : 1;
        bool IsNotPassed : 1;
        bool IsNotRaked : 1;
        bool IsNoLight : 1;
        bool IsMultihex : 1;
    };

    Field() = default;
    ~Field();
    void AddItem(ItemHexView* item, ItemHexView* block_lines_item);
    void EraseItem(ItemHexView* item, ItemHexView* block_lines_item);
    Tile& AddTile(AnyFrames* anim, short ox, short oy, uchar layer, bool is_roof);
    void EraseTile(uint index, bool is_roof);
    uint GetTilesCount(bool is_roof);
    Tile& GetTile(uint index, bool is_roof);
    void AddDeadCrit(CritterView* cr);
    void EraseDeadCrit(CritterView* cr);
    void ProcessCache();
    void AddSpriteToChain(Sprite* spr);
    void UnvalidateSpriteChain();

    bool IsView {};
    Sprite* SpriteChain {};
    CritterView* Crit {};
    CritterViewVec* DeadCrits {};
    int ScrX {};
    int ScrY {};
    AnyFrames* SimplyTile[2] {};
    TileVec* Tiles[2] {};
    ItemHexViewVec* Items {};
    ItemHexViewVec* BlockLinesItems {};
    short RoofNum {};
    FlagsType Flags {};
    uchar Corner {};
};

class HexManager
{
public:
    HexManager(bool mapper_mode, HexSettings& sett, ProtoManager& proto_mngr, SpriteManager& spr_mngr,
        EffectManager& effect_mngr, ResourceManager& res_mngr, ClientScriptSystem& script_sys);
    ~HexManager();

    void ResizeField(ushort w, ushort h);
    Field& GetField(ushort hx, ushort hy) { return hexField[hy * maxHexX + hx]; }
    bool IsHexToDraw(ushort hx, ushort hy) { return hexField[hy * maxHexX + hx].IsView; }
    char& GetHexTrack(ushort hx, ushort hy) { return hexTrack[hy * maxHexX + hx]; }
    ushort GetWidth() { return maxHexX; }
    ushort GetHeight() { return maxHexY; }
    void ClearHexTrack() { memzero(hexTrack, maxHexX * maxHexY * sizeof(char)); }
    void SwitchShowTrack();
    bool IsShowTrack() { return isShowTrack; };

    bool FindPath(
        CritterView* cr, ushort start_x, ushort start_y, ushort& end_x, ushort& end_y, UCharVec& steps, int cut);
    bool CutPath(CritterView* cr, ushort start_x, ushort start_y, ushort& end_x, ushort& end_y, int cut);
    bool TraceBullet(ushort hx, ushort hy, ushort tx, ushort ty, uint dist, float angle, CritterView* find_cr,
        bool find_cr_safe, CritterViewVec* critters, int find_type, UShortPair* pre_block, UShortPair* block,
        UShortPairVec* steps, bool check_passed);
    void FindSetCenter(int cx, int cy);

    bool IsMapLoaded() { return curPidMap != 0; }
    bool LoadMap(CacheStorage& cache, hash map_pid);
    void UnloadMap();
    void GetMapHash(CacheStorage& cache, hash map_pid, hash& hash_tiles, hash& hash_scen);
    void GenerateItem(uint id, hash proto_id, Properties& props);
    int GetDayTime();
    int GetMapTime();
    int* GetMapDayTime();
    uchar* GetMapDayColor();
    void OnResolutionChanged();

    void ReloadSprites();

    void ChangeZoom(int zoom); // <0 in, >0 out, 0 normalize
    void GetScreenHexes(int& sx, int& sy);
    void GetHexCurrentPosition(ushort hx, ushort hy, int& x, int& y);
    bool ProcessHexBorders(ItemHexView* item);

    void RebuildMap(int rx, int ry);
    void RebuildMapOffset(int ox, int oy);
    void DrawMap();
    void SetFog(PointVec& look_points, PointVec& shoot_points, short* offs_x, short* offs_y);
    Sprites& GetDrawTree() { return mainTree; }
    void RefreshMap() { RebuildMap(screenHexX, screenHexY); }

private:
    int GetViewWidth();
    int GetViewHeight();

    void PrepareFogToDraw();
    void InitView(int cx, int cy);
    void ResizeView();
    bool IsVisible(uint spr_id, int ox, int oy);
    bool ProcessHexBorders(uint spr_id, int ox, int oy, bool resize_map);

    HexSettings& settings;
    GeometryHelper geomHelper;
    ProtoManager& protoMngr;
    SpriteManager& sprMngr;
    EffectManager& effectMngr;
    ResourceManager& resMngr;
    ClientScriptSystem& scriptSys;

    ushort maxHexX {};
    ushort maxHexY {};
    Field* hexField {};
    char* hexTrack {};
    AnyFrames* picTrack1 {};
    AnyFrames* picTrack2 {};
    AnyFrames* picHexMask {};
    bool isShowTrack {};
    bool isShowHex {};
    AnyFrames* picHex[3] {};
    string curDataPrefix {};

    hash curPidMap {};
    int curMapTime {-1};
    int dayTime[4] {};
    uchar dayColor[12] {};
    hash curHashTiles {};
    hash curHashScen {};

    bool mapperMode {};
    RenderTarget* rtMap {};
    RenderTarget* rtLight {};
    RenderTarget* rtFog {};
    uint rtScreenOX {};
    uint rtScreenOY {};
    Sprites mainTree;
    ViewField* viewField {};

    int screenHexX {};
    int screenHexY {};
    int hTop {};
    int hBottom {};
    int wLeft {};
    int wRight {};
    int wVisible {};
    int hVisible {};

    short* fogOffsX {};
    short* fogOffsY {};
    short fogLastOffsX {};
    short fogLastOffsY {};
    bool fogForceRerender {};
    PointVec fogLookPoints {};
    PointVec fogShootPoints {};

    // Scroll
public:
    struct AutoScrollInfo
    {
        bool Active {};
        bool CanStop {};
        float OffsX {};
        float OffsY {};
        float OffsXStep {};
        float OffsYStep {};
        float Speed {};
        uint HardLockedCritter {};
        uint SoftLockedCritter {};
        ushort CritterLastHexX {};
        ushort CritterLastHexY {};
    };

    bool Scroll();
    void ScrollToHex(int hx, int hy, float speed, bool can_stop);
    void ScrollOffset(int ox, int oy, float speed, bool can_stop);

    AutoScrollInfo AutoScroll {};

    // Weather
public:
    void SwitchShowHex();
    void SwitchShowRain();
    void SetWeather(int time, uchar rain);

private:
    bool ScrollCheckPos(int (&positions)[4], int dir1, int dir2);
    bool ScrollCheck(int xmod, int ymod);

    // Critters
public:
    void SetCritter(CritterView* cr);
    bool TransitCritter(CritterView* cr, int hx, int hy, bool animate, bool force);
    CritterView* GetCritter(uint crid);
    CritterView* GetChosen();
    void AddCritter(CritterView* cr);
    void RemoveCritter(CritterView* cr);
    void DeleteCritter(uint crid);
    void DeleteCritters();
    void GetCritters(ushort hx, ushort hy, CritterViewVec& crits, int find_type);
    CritterViewMap& GetCritters() { return allCritters; }
    void SetCritterContour(uint crid, int contour);
    void SetCrittersContour(int contour);
    void SetMultihex(ushort hx, ushort hy, uint multihex, bool set);

private:
    CritterViewMap allCritters {};
    uint chosenId {};
    uint critterContourCrId {};
    int critterContour {};
    int crittersContour {};

    // Items
public:
    uint AddItem(uint id, hash pid, ushort hx, ushort hy, bool is_added, UCharVecVec* data);
    void FinishItem(uint id, bool is_deleted);
    void DeleteItem(ItemHexView* item, bool destroy_item = true, ItemHexViewVec::iterator* it_hex_items = nullptr);
    void PushItem(ItemHexView* item);
    ItemHexView* GetItem(ushort hx, ushort hy, hash pid);
    ItemHexView* GetItemById(ushort hx, ushort hy, uint id);
    ItemHexView* GetItemById(uint id);
    void GetItems(ushort hx, ushort hy, ItemHexViewVec& items);
    ItemHexViewVec& GetItems() { return hexItems; }
    Rect GetRectForText(ushort hx, ushort hy);
    void ProcessItems();
    void SkipItemsFade();

private:
    void AddFieldItem(ushort hx, ushort hy, ItemHexView* item);
    void EraseFieldItem(ushort hx, ushort hy, ItemHexView* item);

    ItemHexViewVec hexItems {};

    // Light
public:
    void ClearHexLight() { memzero(hexLight, maxHexX * maxHexY * sizeof(uchar) * 3); }
    uchar* GetLightHex(ushort hx, ushort hy) { return &hexLight[hy * maxHexX * 3 + hx * 3]; }
    void RebuildLight() { requestRebuildLight = requestRenderLight = true; }
    vector<LightSource>& GetLights() { return lightSources; }

private:
    void PrepareLightToDraw();
    void MarkLight(ushort hx, ushort hy, uint inten);
    void MarkLightEndNeighbor(ushort hx, ushort hy, bool north_south, uint inten);
    void MarkLightEnd(ushort from_hx, ushort from_hy, ushort to_hx, ushort to_hy, uint inten);
    void MarkLightStep(ushort from_hx, ushort from_hy, ushort to_hx, ushort to_hy, uint inten);
    void TraceLight(ushort from_hx, ushort from_hy, ushort& hx, ushort& hy, int dist, uint inten);
    void ParseLightTriangleFan(LightSource& ls);
    void RealRebuildLight();
    void CollectLightSources();

    bool requestRebuildLight {};
    bool requestRenderLight {};
    uchar* hexLight {};
    uint lightPointsCount {};
    vector<PointVec> lightPoints {};
    PointVec lightSoftPoints {};
    vector<LightSource> lightSources {};
    vector<LightSource> lightSourcesScen {};

    int lightCapacity {};
    int lightMinHx {};
    int lightMaxHx {};
    int lightMinHy {};
    int lightMaxHy {};
    int lightProcentR {};
    int lightProcentG {};
    int lightProcentB {};

public:
    void RebuildTiles();
    void RebuildRoof();
    void SetSkipRoof(int hx, int hy);
    void MarkRoofNum(int hx, int hy, int num);
    bool GetHexPixel(int x, int y, ushort& hx, ushort& hy);
    ItemHexView* GetItemPixel(int x, int y, bool& item_egg); // With transparent egg
    CritterView* GetCritterPixel(int x, int y, bool ignore_dead_and_chosen);
    void GetSmthPixel(int x, int y, ItemHexView*& item, CritterView*& cr);
    bool RunEffect(hash eff_pid, ushort from_hx, ushort from_hy, ushort to_hx, ushort to_hy);
    void ProcessRain();
    void SetRainAnimation(const char* fall_anim_name, const char* drop_anim_name);
    void SetCursorPos(int x, int y, bool show_steps, bool refresh);
    void DrawCursor(uint spr_id);
    void DrawCursor(const char* text);

private:
    struct Drop
    {
        uint CurSprId {};
        short OffsX {};
        short OffsY {};
        short GroundOffsY {};
        short DropCnt {};
    };

    bool CheckTilesBorder(Field::Tile& tile, bool is_roof);

    Sprites tilesTree;
    Sprites roofTree;
    int roofSkip {};
    vector<Drop*> rainData {};
    int rainCapacity {};
    string picRainFallName {};
    string picRainDropName {};
    AnyFrames* picRainFall {};
    AnyFrames* picRainDrop {};
    Sprites roofRainTree;

    int drawCursorX {};
    AnyFrames* cursorPrePic {};
    AnyFrames* cursorPostPic {};
    AnyFrames* cursorXPic {};
    int cursorX {};
    int cursorY {};

    // Editor stuff
public:
    using TileVecVec = vector<MapTileVec>;

    bool SetProtoMap(ProtoMap& pmap);
    void GetProtoMap(ProtoMap& pmap);
    MapTileVec& GetTiles(ushort hx, ushort hy, bool is_roof);
    void ClearSelTiles();
    void ParseSelTiles();
    void SetTile(hash name, ushort hx, ushort hy, short ox, short oy, uchar layer, bool is_roof, bool select);
    void EraseTile(ushort hx, ushort hy, uchar layer, bool is_roof, uint skip_index);
    void AddFastPid(hash pid);
    bool IsFastPid(hash pid);
    void ClearFastPids();
    void AddIgnorePid(hash pid);
    void SwitchIgnorePid(hash pid);
    bool IsIgnorePid(hash pid);
    void ClearIgnorePids();
    void GetHexesRect(const Rect& rect, UShortPairVec& hexes);
    void MarkPassedHexes();

    uchar SelectAlpha {100};
    ProtoMap* CurProtoMap {};
    TileVecVec TilesField {};
    TileVecVec RoofsField {};

private:
    HashSet fastPids {};
    HashSet ignorePids {};
};
