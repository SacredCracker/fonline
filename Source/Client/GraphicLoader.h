#pragma once

#include "Common.h"

class AnimSet;
struct Bone;
using BoneVec = vector< Bone* >;
struct MeshTexture;
using MeshTextureVec = vector< MeshTexture* >;
struct Effect;
using EffectVec = vector< Effect* >;
struct EffectDefault;
struct EffectPass;
class File;

class GraphicLoader
{
    // Models
public:
    static Bone*    LoadModel( const string& fname );
    static void     DestroyModel( Bone* root_bone );
    static AnimSet* LoadAnimation( const string& anim_fname, const string& anim_name );

private:
    static StrVec  processedFiles;
    static BoneVec loadedModels;
    static StrVec  loadedModelNames;
    static PtrVec  loadedAnimations;   // Pointers of AnimSet

    // Textures
public:
    static MeshTexture* LoadTexture( const string& texture_name, const string& model_path );
    static void         DestroyTextures();

private:
    static MeshTextureVec loadedMeshTextures;

    // Effects
public:
    static Effect* LoadEffect( const string& effect_name, bool use_in_2d, const string& defines = "", const string& model_path = "", EffectDefault* defaults = nullptr, uint defaults_count = 0 );
    static void    EffectProcessVariables( EffectPass& effect_pass, bool start, float anim_proc = 0.0f, float anim_time = 0.0f, MeshTexture** textures = nullptr );
    static bool    LoadMinimalEffects();
    static bool    LoadDefaultEffects();
    static bool    Load3dEffects();

private:
    static EffectVec loadedEffects;

    static bool LoadEffectPass( Effect* effect, const string& fname, File& file, uint pass, bool use_in_2d, const string& defines, EffectDefault* defaults, uint defaults_count );
};