#include "rds_engine-pch.h"
#include "rdsMeshAsset.h"

#include "../../ecs/rdsScene.h"
#include "../../ecs/rdsEntity.h"
#include "../../ecs/component/rdsCTransform.h"
#include "../../ecs/component/rdsCRenderableMesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>
#include <assimp/vector2.h>
#include <assimp/quaternion.h>

namespace rds
{
namespace shaderInterop
{
#include "../../../built-in/shader/interop/rdsShaderInterop_Material.hlsl"
}
}

namespace rds
{

#if 0
#pragma mark --- rdsAssimpMeshLoader-Impl ---
#endif // 0
#if 1

template<class T> Vec3<T>		toVec3(		const aiVector3t<T>&	v);
template<class T> Mat4<T>		toMat4(		const aiMatrix4x4t<T>&	v);
template<class T> ColorRGBA<T>	toColor4(	const aiColor4t<T>&		v);
template<class T> Color4b		toColor4b(	const aiColor4t<T>&		v);

struct AssimpMeshLoader
{
public:
	static constexpr u32 s_kInvalidId = MeshAsset::s_kInvalidId;

public:
	static aiTextureType toAiTextureType(MaterialData_TextureType v);

public:
	bool load(MeshAsset* oMeshAsset, StrView filename, Shader* shader);

private:
	bool loadVertices(	EditMesh* oEdMesh, const aiMesh& aiMesh);
	bool loadIndices(	EditMesh* oEdMesh, const aiMesh& aiMesh);
	bool loadBones(		EditMesh* oEdMesh, const aiMesh& aiMesh);

	bool loadHeirarchyData(MeshAssetNodeList* nodeList, const aiNode* srcRootNode, const aiScene* srcScene);
	bool _loadHeirarchyData(int level, MeshAssetNode* dst, MeshAssetNodeList* nodeList, MeshAssetNode* parent, const Mat4f& worldTransform, const aiNode* srcNode, const aiScene* srcScene);

	bool loadMaterials(Shader* shader, const aiScene* srcScene);

private:
	StrView		_filename;
	MeshAsset*	_oMeshAsset	= nullptr;
};

inline
aiTextureType 
AssimpMeshLoader::toAiTextureType(MaterialData_TextureType v)
{
	using SRC = MaterialData_TextureType;
	switch (v)
	{
		case SRC::Albedo:				{ return aiTextureType::aiTextureType_DIFFUSE; }			break;
		case SRC::Normal:				{ return aiTextureType::aiTextureType_NORMALS; }			break;
		case SRC::RoughnessMetalness:	{ return aiTextureType::aiTextureType_DIFFUSE_ROUGHNESS; }	break;
		case SRC::Emissive:				{ return aiTextureType::aiTextureType_EMISSIVE; }			break;
		default: { RDS_THROW("unsupport type {}, {}", v, RDS_SRCLOC); }	break;
	}
}

inline
bool 
AssimpMeshLoader::load(MeshAsset* oMeshAsset, StrView filename, Shader* shader)
{
	//RDS_DUMP_VAR(Path::realpath(filepath));
	{
		_filename	= filename;
		_oMeshAsset	= oMeshAsset;
	}

	Assimp::Importer importer;

	TempString buf;
	buf = filename;

	u32				loadFileFlags	 = aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_GenUVCoords | aiProcess_GenNormals | aiProcess_OptimizeMeshes;
	if (false)		loadFileFlags	|= aiProcess_CalcTangentSpace;
	if (false)		loadFileFlags	|= aiProcess_GlobalScale;			// convert cm to m

	if (true)		loadFileFlags	|= aiProcess_FlipUVs;
	if (true)		loadFileFlags	|= aiProcess_SortByPType;			// split by primitive type
	if (true)		loadFileFlags	|= aiProcess_ValidateDataStructure;

	const aiScene* srcScene = importer.ReadFile(buf.c_str(), loadFileFlags);
	bool isLoadFailed = !srcScene || srcScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !srcScene->mRootNode;
	if (isLoadFailed)
	{
		RDS_CORE_LOG_ERROR("load mesh failed: {}", importer.GetErrorString());
		return false;
	}

	u32 nMeshes					= srcScene->mNumMeshes;
	u32 startBaseVertexIndex	= 0;
	u32 startBaseIndex			= 0;
	u32 totalVertices			= 0;
	u32 totalIndices			= 0;

	auto& rdMesh = oMeshAsset->renderMesh;
	// load subMeshes
	{
		rdMesh.setSubMeshCount(nMeshes);
		{
			for (u32 i = 0; i < nMeshes; i++)
			{
				const aiMesh* pAiMesh = srcScene->mMeshes[i];
				const u32 nVertices = pAiMesh->mNumVertices;
				const u32 nIndices	= pAiMesh->mNumFaces * 3;	// triangle only

				auto& mesh = rdMesh.subMeshes()[i];
				mesh.create(startBaseVertexIndex, nVertices, startBaseIndex, nIndices);

				startBaseVertexIndex += nVertices;
				startBaseIndex		 += nIndices;
			}
			totalVertices	= startBaseVertexIndex;
			totalIndices	= startBaseIndex;
		}
	}

	EditMesh edMesh;
	edMesh.pos.reserve(totalVertices);
	edMesh.indices.reserve(totalIndices);
	for (u32 i = 0; i < nMeshes; i++)
	{
		const aiMesh* pAiMesh = srcScene->mMeshes[i]; 
		if (!loadVertices(	&edMesh, *pAiMesh)) return false;
		if (!loadIndices(	&edMesh, *pAiMesh)) return false;
		if (!loadBones(		&edMesh, *pAiMesh)) return false;
	}
	rdMesh.create(edMesh, nMeshes);

	bool isSuccess = false;
	isSuccess = loadHeirarchyData(&oMeshAsset->nodeList, srcScene->mRootNode, srcScene);
	if (!isSuccess)
	{
		return false;
	}

	isSuccess = loadMaterials(shader, srcScene);
	if (!isSuccess)
	{
		return false;
	}

	return isSuccess;
}

inline
bool 
AssimpMeshLoader::loadVertices(EditMesh* oEdMesh, const aiMesh& aiMesh)
{
	auto&	edMesh		= *oEdMesh;
	u32		nVertices	= aiMesh.mNumVertices;

	//auto& mesh = subMeshes[iMeshes];
	bool hasTangents	= aiMesh.mTangents	!= nullptr && aiMesh.mNumVertices > 0;
	bool hasBitangents	= aiMesh.mBitangents	!= nullptr && aiMesh.mNumVertices > 0;

	if (aiMesh.HasPositions())
	{
		for (u32 iVertex = 0; iVertex < nVertices; iVertex++)
		{
			edMesh.pos.emplace_back(toVec3(aiMesh.mVertices[iVertex]));
		}
	}

	if (aiMesh.HasNormals())
	{
		for (u32 iVertex = 0; iVertex < nVertices; iVertex++)
		{
			edMesh.normal.emplace_back(toVec3(aiMesh.mNormals[iVertex]));
		}
	}

	if (hasTangents)
	{
		for (u32 iVertex = 0; iVertex < nVertices; iVertex++)
		{
			edMesh.tangent.emplace_back(toVec3(aiMesh.mTangents[iVertex]));
		}
	}

	if (hasBitangents)
	{
		for (u32 iVertex = 0; iVertex < nVertices; iVertex++)
		{
			edMesh.binormal.emplace_back(	toVec3(aiMesh.mBitangents[iVertex]));
		}
	}

	auto uvCount = math::min<u32>(AI_MAX_NUMBER_OF_TEXTURECOORDS, EditMesh::s_kUvCount);
	for (u32 i = 0; i < uvCount; i++)
	{
		bool hasUv = aiMesh.HasTextureCoords(i);
		if (hasUv)
		{
			for (u32 iVertex = 0; iVertex < nVertices; iVertex++)
			{
				edMesh.uvs[i].emplace_back(toVec3(aiMesh.mTextureCoords[i][iVertex]).toVec2());
			}
		}
	}

	auto colorCount = math::min<u32>(AI_MAX_NUMBER_OF_COLOR_SETS, EditMesh::s_kColorCount);
	for (u32 i = 0; i < colorCount; i++)
	{
		bool hasColor = aiMesh.HasVertexColors(i);
		if (hasColor)
		{
			for (u32 iVertex = 0; iVertex < nVertices; iVertex++)
			{
				edMesh.color.emplace_back(toColor4b(aiMesh.mColors[i][iVertex]));
			}
		}
	}
	/*
	* useless, just as a reference
	*/
	#if 0
	for (u32 iVertex = 0; iVertex < nVertices; iVertex++)
	{
		if (aiMesh.HasPositions())		edMesh.pos.emplace_back(		toVec3(aiMesh.mVertices[iVertex]));
		if (aiMesh.HasTextureCoords(0))	edMesh.uvs[0].emplace_back(		toVec3(aiMesh.mTextureCoords[0][iVertex]).toVec2());
		if (aiMesh.HasNormals())		edMesh.normal.emplace_back(		toVec3(aiMesh.mNormals[iVertex]));
		if (hasTangents)				edMesh.tangent.emplace_back(	toVec3(aiMesh.mTangents[iVertex]));
		if (hasBitangents)				edMesh.binormal.emplace_back(	toVec3(aiMesh.mBitangents[iVertex]));
	}
	#endif // 0

	return true;
}

inline
bool 
AssimpMeshLoader::loadIndices(EditMesh* oEdMesh, const aiMesh& aiMesh)
{
	auto&	edMesh	= *oEdMesh;
	u32		nFaces	= aiMesh.mNumFaces;

	auto& indices	= edMesh.indices;
	for (u32 iIndex = 0; iIndex < nFaces; iIndex++)
	{
		const aiFace* face = &aiMesh.mFaces[iIndex];
		if (face->mNumIndices != 3)
		{
			RDS_THROW("load mesh: {} error, face index count is not 3 (triangle)", _filename);
		}
		indices.emplace_back(face->mIndices[0]);
		indices.emplace_back(face->mIndices[1]);
		indices.emplace_back(face->mIndices[2]);
	}

	return true;
}

inline
bool 
AssimpMeshLoader::loadBones(EditMesh* oEdMesh, const aiMesh& aiMesh)
{
	// load bone data for mesh
	//auto& boneMapping = model_.m_boneMapping;
	static constexpr int s_kInvalidBoneId = -1;
	#if 0
	for (u32 boneIndex = 0; boneIndex < aiMesh.mNumBones; ++boneIndex)
	{
		int			boneID		= s_kInvalidBoneId;
		auto&		aiBone		= aiMesh.mBones[boneIndex];
		const char* boneName	= aiMesh.mBones[boneIndex]->mName.C_Str();

		//if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			BoneInfo newBoneInfo;
			newBoneInfo.id			= boneCounter;
			newBoneInfo.offset		= toMat4From(aiMesh.mBones[boneIndex]->mOffsetMatrix);
			boneID					= boneCounter;
			boneInfoMap[boneName]	= newBoneInfo;
			boneCounter++;
		}
		else
		{
			boneID = boneInfoMap.at(boneName).id;
			}

			RDS_CORE_ASSERT(boneID != s_kInvalidBoneId, "invalid bone id");
			const auto*	pBone		= aiMesh.mBones[boneIndex];
			const auto&	weights		= pBone->mWeights;
			const u32	numWeights	= pBone->mNumWeights;

			for (u32 weightIndex = 0; weightIndex < numWeights; ++weightIndex)
			{
				u32		vertexId	= mesh.baseVertexIndex + weights[weightIndex].mVertexId;
				float	weight		= weights[weightIndex].mWeight;
				RDS_CORE_ASSERT(vertexId <= animatedVertices.size(), "Invalid vertex id");
				animatedVertices[vertexId].addVertexBoneData(boneID, weight);
			}
	}
	#endif // 0
	return true;
}

inline
bool 
AssimpMeshLoader::loadHeirarchyData(MeshAssetNodeList* nodeList, const aiNode* srcRootNode, const aiScene* srcScene)
{
	if (!srcRootNode)
	{
		return true;
	}

	//auto& nodes = nodeList->nodes;
	//nodes.clear();
	nodeList->rootNode = nodeList->_makeNode();
	return _loadHeirarchyData(0, nodeList->rootNode, nodeList, nullptr, Mat4f::s_identity(), srcRootNode, srcScene);
}

inline
bool 
AssimpMeshLoader::_loadHeirarchyData(int level, MeshAssetNode* dst, MeshAssetNodeList* nodeList, MeshAssetNode* parent, const Mat4f& worldTransform, const aiNode* srcNode, const aiScene* srcScene)
{
	auto	nMeshes			= srcNode->mNumMeshes;
	int		extraMeshCount	= (nMeshes > 0 ? nMeshes - 1 : 0);

	auto nChildren	= srcNode->mNumChildren;
	//nodes->reserve(nodes->size() + nChildren + extraMeshCount);
	RDS_TODO("_loadHeirarchyData, fmtTo(node.name, , srcNode->mName.C_Str()); failed, node.name == 0, but the length is not 0");
	{
		auto& node = *dst;
		//fmtTo(node.name,		"{}", srcNode->mName.C_Str());
		node.name			= srcNode->mName.C_Str();
		node.parent			= parent;
		node.localTransform = toMat4(srcNode->mTransformation);
		node.worldTransform = worldTransform * dst->localTransform;
		if (nMeshes > 0)
		{
			node.meshIdx		= srcNode->mMeshes[0];
			node.materialIdx	= srcScene->mMeshes[node.meshIdx]->mMaterialIndex;
		}
	}

	// flatten the meshes, each node only has 1 mesh
	{
		auto& parentsChildren = parent->children; 

		if (parent)
			parentsChildren.reserve(parentsChildren.size() + extraMeshCount);
		for (u32 i = 1; i < nMeshes; i++)
		{
			auto& node = *nodeList->_makeNode();

			TempString buf;
			fmtTo(buf,		"{}-({})", srcNode->mName.C_Str(), i);
			//fmtTo(node.name,	"{}-({})", srcNode->mName.C_Str(), i);
			node.name			= buf;
			node.parent			= parent;
			node.localTransform = dst->localTransform;
			node.worldTransform = dst->worldTransform;
			node.meshIdx		= srcNode->mMeshes[i];
			node.materialIdx	= srcScene->mMeshes[node.meshIdx]->mMaterialIndex;

			if (parent)
				parentsChildren.emplace_back(&node);
		}
	}

	for (u32 i = 0; i < nChildren; i++)
	{
		auto& node = *nodeList->_makeNode();
		dst->children.emplace_back(&node);
		if (!_loadHeirarchyData(level + 1, &node, nodeList, dst, dst->worldTransform, srcNode->mChildren[i], srcScene))
		{
			return false;
		}
	}
	return true;
}

inline
bool 
AssimpMeshLoader::loadMaterials(Shader* shader, const aiScene* srcScene)
{
	// local cache map to find textures by name
	StringMap<u32> textureIndexMap;

	auto	isSuccess	= [](aiReturn v) { return v == AI_SUCCESS; };
	auto&	meshAsset	= *_oMeshAsset;

	// assimp only support materials to have texture, it will not load all texture first
	if (srcScene->HasTextures() || srcScene->HasMaterials())
	{
		TempString texFilename;
		TempString dir = Path::dirname(_filename);

		const u32 nTextures	= srcScene->mNumTextures;
		auto& textures		= meshAsset.materialList.textures;
		textures.reserve(nTextures);

		for (u32 i = 0; i < nTextures; i++)
		{
			const auto& srcTexture = *srcScene->mTextures[i];

			const char* texName = srcTexture.mFilename.C_Str();
			fmtToNew(texFilename, "{}/{}", dir, texName);
			if (!Path::isExist(dir))
				continue;

			auto& dst = textures.emplace_back();
			auto cDesc = Texture2D::makeCDesc(RDS_SRCLOC);	
			cDesc.create(texFilename);
			dst = Renderer::rdDev()->createTexture2D(cDesc);

			u32 texIdx = sCast<u32>(textureIndexMap.size());
			textureIndexMap[texName] = texIdx;
		}

		// TODO: assimp only support materials to have texture, it will not load all texture first
		{
			const u32 nMaterials	= srcScene->mNumMaterials;
			for (u32 iMtl = 0; iMtl < nMaterials; iMtl++)
			{
				const auto& srcMaterial = *srcScene->mMaterials[iMtl];

				aiString temp;

				auto nTexType = MeshAssetMaterialData::s_kTextureTypeCount;
				for (u32 iType = 0; iType < nTexType; iType++)
				{
					auto	texType		= sCast<MaterialData_TextureType>(iType);
					auto	aiTexType	= toAiTextureType(texType);
					u32		nTex		= srcMaterial.GetTextureCount(aiTexType);

					u32 maxTextureSupportCount = true ? 1 : nTex;
					for (u32 i = 0; i < maxTextureSupportCount; i++)
					{
						if (isSuccess(srcMaterial.GetTexture(aiTexType, i, &temp)))
						{
							const char* texName = temp.C_Str();
							auto it = textureIndexMap.find(texName);
							if (it == textureIndexMap.end())
							{
								fmtToNew(texFilename, "{}/{}", dir, texName);
								if (!Path::isExist(dir))
									continue;

								auto& dst = textures.emplace_back();
								auto cDesc = Texture2D::makeCDesc(RDS_SRCLOC);	
								cDesc.create(texFilename);
								dst = Renderer::rdDev()->createTexture2D(cDesc);

								u32 texIdx = sCast<u32>(textureIndexMap.size());
								textureIndexMap[texName] = texIdx;
							}
						}
					}
				}
			}
		}
	}

	if (srcScene->HasMaterials())
	{
		const u32	nMaterials	= srcScene->mNumMaterials;

		meshAsset.materialList.materialData.reserve(nMaterials);
		auto& textures = meshAsset.materialList.textures;

		for (u32 iMtl = 0; iMtl < nMaterials; iMtl++)
		{
			const auto& srcMaterial = *srcScene->mMaterials[iMtl];
			
			auto& materialDatum = meshAsset.materialList.materialData.emplace_back();
			materialDatum.name = srcMaterial.GetName().C_Str();
			//fmtTo(materialDatum.name, "{}", srcMaterial.GetName().C_Str());
			
			// load material data
			{
				aiColor4D	bufColor4f;
				float		bufFloat;

				if (isSuccess(srcMaterial.Get(AI_MATKEY_COLOR_DIFFUSE, bufColor4f)))
				{
					materialDatum.albedo = toColor4(bufColor4f);
				}

				if (isSuccess(srcMaterial.Get(AI_MATKEY_COLOR_EMISSIVE, bufColor4f)))
				{
					materialDatum.emission = toColor4(bufColor4f);
				}

				if (isSuccess(srcMaterial.Get(AI_MATKEY_ROUGHNESS_FACTOR, bufFloat)))
				{
					materialDatum.roughness = bufFloat;
				}

				if (isSuccess(srcMaterial.Get(AI_MATKEY_REFLECTIVITY, bufFloat)))
				{
					materialDatum.metalness = bufFloat;
				}
			}
			
			// load textures
			{
				aiString texName;


				auto nTexType = MeshAssetMaterialData::s_kTextureTypeCount;
				for (u32 iType = 0; iType < nTexType; iType++)
				{
					auto	texType		= sCast<MaterialData_TextureType>(iType);
					auto	aiTexType	= toAiTextureType(texType);
					u32		nTex		= srcMaterial.GetTextureCount(aiTexType);

					u32 maxTextureSupportCount = true ? 1 : nTex;
					for (u32 i = 0; i < maxTextureSupportCount; i++)
					{
						if (isSuccess(srcMaterial.GetTexture(aiTexType, i, &texName)))
						{
							auto it = textureIndexMap.find(texName.C_Str());
							if (it == textureIndexMap.end())
								continue;

							u32 textureIdx = it->second;
							materialDatum._accessTexture(texType) = textures[textureIdx].ptr();
						}
					}
				}
			}
		}
	}

	// set materials with material data
	if (shader)
	{
		auto& materialList	= meshAsset.materialList;
		auto& materials		= materialList.materials;
		auto& materialData	= materialList.materialData;

		auto nMaterial = materialData.size();
		materials.reserve(nMaterial);
		for (u32 i = 0; i < nMaterial; i++)
		{
			const auto& mtlDatum = materialData[i];

			auto& mtl = materials.emplace_back();
			mtl = Renderer::rdDev()->createMaterial(shader);
			mtl->setDebugName(mtlDatum.name);

			if (auto* tex = mtlDatum.getTexture(MaterialData_TextureType::Albedo))				mtl->setParam(RDS_STRINGIFY(RDS_MATERIAL_TEXTURE_Albedo),				tex);
			if (auto* tex = mtlDatum.getTexture(MaterialData_TextureType::Normal))				mtl->setParam(RDS_STRINGIFY(RDS_MATERIAL_TEXTURE_Normal),				tex);
			if (auto* tex = mtlDatum.getTexture(MaterialData_TextureType::RoughnessMetalness))	mtl->setParam(RDS_STRINGIFY(RDS_MATERIAL_TEXTURE_RoughnessMetalness),	tex);
			if (auto* tex = mtlDatum.getTexture(MaterialData_TextureType::Emissive))			mtl->setParam(RDS_STRINGIFY(RDS_MATERIAL_TEXTURE_Emissive),				tex);

			mtl->setParam(RDS_STRINGIFY(RDS_MATERIAL_PROPERTY_albedo),				mtlDatum.albedo);
			mtl->setParam(RDS_STRINGIFY(RDS_MATERIAL_PROPERTY_emission),			mtlDatum.emission);
			mtl->setParam(RDS_STRINGIFY(RDS_MATERIAL_PROPERTY_metalness),			mtlDatum.metalness);
			mtl->setParam(RDS_STRINGIFY(RDS_MATERIAL_PROPERTY_roughness),			mtlDatum.roughness);
		}
	}

	#if 0
	for (auto& e : textureIndexMap)
	{
		RDS_LOG_ERROR("{}", e.first);
	}
	#endif // 0

	return true;
}

template<class T> inline
Vec3<T> 
toVec3(const aiVector3t<T>& v)
{
	auto o = Vec3<T>{ v.x, v.y, v.z };
	return o;
}

template<class T> inline
Mat4<T>
toMat4(const aiMatrix4x4t<T>& v)
{
	Mat4f o;
	o[0][0] = v.a1; o[0][1] = v.b1;  o[0][2] = v.c1; o[0][3] = v.d1;
	o[1][0] = v.a2; o[1][1] = v.b2;  o[1][2] = v.c2; o[1][3] = v.d2;
	o[2][0] = v.a3; o[2][1] = v.b3;  o[2][2] = v.c3; o[2][3] = v.d3;
	o[3][0] = v.a4; o[3][1] = v.b4;  o[3][2] = v.c4; o[3][3] = v.d4;
	return o;
};

template<class T> inline
ColorRGBA<T> 
toColor4(const aiColor4t<T>& v)
{
	auto o = ColorRGBA<T>{ v.r, v.g, v.b, v.a };
	return o;
}

template<class T> inline
Color4b
toColor4b(const aiColor4t<T>& v)
{
	using U = Color4b::ElementType;
	auto o = toColor4(v);
	return Color4b{ sCast<U>(o.r), sCast<U>(o.g), sCast<U>(o.b), sCast<U>(o.a)};
}

#endif

#if 0
#pragma mark --- rdsMeshAsset-Impl ---
#endif // 0
#if 1

MeshAsset::MeshAsset()
{

}

MeshAsset::~MeshAsset()
{

}

bool 
MeshAsset::load(StrView filename_, Shader* shader)
{
	AssimpMeshLoader loader;
	bool isSuccess = loader.load(this, filename_, shader);
	return isSuccess;
}

Entity*
MeshAsset::addToScene(Scene* scene)
{
	auto* rootNode = nodeList.rootNode;
	if (!rootNode)
	{
		return nullptr;
	}

	auto* node		= rootNode;
	auto* rootEnt	= _addToScene(scene, node, Mat4f::s_identity());
	return rootEnt;
}

Entity*
MeshAsset::_addToScene(Scene* scene, MeshAssetNode* node, const Mat4f& matrix)
{
	//RDS_LOG("{}", node->name);

	auto* ent	= scene->addEntity(node->name);
	_setupEntity(ent, node, matrix);

	for (auto& e : node->children)
	{
		// set parent in this line

		_addToScene(scene, e, node->localTransform);
	}
	return ent;
}

void 
MeshAsset::_setupEntity(Entity* dst, MeshAssetNode* node, const Mat4f& localMatrix)
{
	auto* ent = dst;

	RDS_TODO("decompose TRS then set to entity transform");
	//Mat4f dstLocalMatrix = localMatrix * node->localTransform;
	//auto* transform = ent->getComponent<CTransform>();
	//transform->setLocalMatrix(worldMatrix);

	auto* rdableMesh = ent->addComponent<CRenderableMesh>();
	rdableMesh->meshAsset		= this;
	rdableMesh->subMeshIndex	= node->meshIdx;
	rdableMesh->material		= node->materialIdx < materialList.materials.size() ? materialList.materials[node->materialIdx] : nullptr;
}



#endif

}