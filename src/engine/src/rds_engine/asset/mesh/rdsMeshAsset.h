#pragma once

#include "rds_engine/common/rds_engine_common.h"
#include "../rdsAsset.h"

namespace rds
{

class Scene;

#if 0
#pragma mark --- rdsMeshAsset-Decl ---
#endif // 0
#if 1

#define MaterialData_TextureType_ENUM_LIST(E) \
	\
	E(Albedo, = 0) \
	E(Normal,) \
	E(RoughnessMetalness,) \
	E(Emissive,) \
	\
	E(_kCount,) \
//---
RDS_ENUM_CLASS(MaterialData_TextureType, u8);

struct MeshAssetMaterialData
{
public:
	static constexpr u32 s_kTextureTypeCount = enumInt(MaterialData_TextureType::_kCount);

public:
	String name;

	//Vector<Texture2D*, s_kTextureTypeCount> textures;

	Color4f albedo;
	Color4f emission;
	float	metalness;
	float	roughness;

	#if 1
	/*Texture2D* texAlbedo				= nullptr;
	Texture2D* texNormal				= nullptr;
	Texture2D* texRoughnessMetalness	= nullptr;
	Texture2D* texEmissive				= nullptr;*/

public:
	MeshAssetMaterialData()
	{
		_textures.resize(s_kTextureTypeCount);
	}

	Texture2D*	getTexture(		MaterialData_TextureType v) const { return _textures[enumInt(v)]; }

public:
	Texture2D*& _accessTexture(	MaterialData_TextureType v) { return _textures[enumInt(v)]; }

public:
	Vector<Texture2D*, s_kTextureTypeCount> _textures;
	#endif // 0
};

struct MeshAssetMaterialList
{
	RDS_ENGINE_COMMON_BODY();
public:
	static constexpr SizeType s_kLocalSize = 2;

public:
	Vector<MeshAssetMaterialData,	s_kLocalSize> materialData;
	Vector<SPtr<Texture2D>,			s_kLocalSize> textures;
	Vector<SPtr<Material>,			s_kLocalSize> materials;
};

struct MeshAssetNode
{
public:
	using Children = Vector<MeshAssetNode*, 1>;

public:
	String			name;
	u32				meshIdx			= Asset::s_kInvalidId;
	u32				materialIdx		= Asset::s_kInvalidId;
	MeshAssetNode*	parent			= nullptr;

	Children		children;
	Mat4f			localTransform;
	Mat4f			worldTransform;
};

struct MeshAssetNodeList
{
	using Traits = EngineTraits;
public:
	using Node	= MeshAssetNode;
	using Nodes = Vector<Node, 2>;

public:
	Node*	rootNode		= nullptr;
	u32		totalNodeCount	= 0;

public:
	MeshAssetNodeList()
	{
		_alloc.setChunkSize(4 * sizeof(MeshAssetNode));
	}

	~MeshAssetNodeList()
	{
		_alloc.destructAndClear<MeshAssetNode>(Traits::s_kDefaultAlign);
	}

public:
	Node* _makeNode() { totalNodeCount++; return sCast<Node*>(_alloc.alloc(sizeof(Node))); }

public:
	LinearAllocator _alloc;
	//Nodes			nodes;		// it will resize, don't add during load
};

class MeshAsset : public Asset
{
public:
	//static constexpr u32 s_kInvalidId = NumLimit<u32>::max();

public:
	String filename;

	RenderMesh				renderMesh;
	MeshAssetMaterialList	materialList;
	MeshAssetNodeList		nodeList;

public:
	MeshAsset();
	~MeshAsset();

	bool load(StrView filename, Shader* shader = nullptr);
	Entity* addToScene(Scene* scene);

protected:
	Entity* _addToScene(Scene* scene, MeshAssetNode* node, const Mat4f& matrix);
	void _setupEntity(Entity* dst, MeshAssetNode* node, const Mat4f& localMatrix);

};

#endif

}