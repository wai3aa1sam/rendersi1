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

#define RDS_USE_ASSIMP_LOAD_TANGENT 1

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

template<class T> AABBox3<T>	toAABBox3(	const aiAABB&			v);


struct AssimpMeshLoader
{
public:
	using AABBox3 = EditMesh::AABBox3;

public:
	static constexpr u32 s_kInvalidId = MeshAsset::s_kInvalidId;

public:
	static aiTextureType toAiTextureType(MaterialData_TextureType v);

public:
	bool load(MeshAsset* oMeshAsset, StrView filename, Shader* shader);

private:
	bool loadVertices(	EditMesh* oEdMesh, AABBox3* oAabbox, const aiMesh& aiMesh);
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
		case SRC::BaseColor:			{ return aiTextureType::aiTextureType_DIFFUSE; }			break;
		case SRC::Normal:				{ return aiTextureType::aiTextureType_NORMALS; }			break;
		case SRC::RoughnessMetalness:	{ return aiTextureType::aiTextureType_DIFFUSE_ROUGHNESS; }	break;
		case SRC::Emission:				{ return aiTextureType::aiTextureType_EMISSIVE; }			break;
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

	u32				loadFileFlags	 = aiProcess_Triangulate			| aiProcess_SortByPType 
									 | aiProcess_ValidateDataStructure	| aiProcess_JoinIdenticalVertices
									 | aiProcess_OptimizeMeshes			;

	if (false)		loadFileFlags	|= aiProcess_GlobalScale;			// convert cm to m

	#if RDS_USE_ASSIMP_LOAD_TANGENT
	if (true)		loadFileFlags	|= aiProcess_CalcTangentSpace;
	#endif

	if (true)		loadFileFlags	|= aiProcess_FlipUVs;
	if (true)		loadFileFlags	|= aiProcess_SortByPType;			// split by primitive type
	if (true)		loadFileFlags	|= aiProcess_GenSmoothNormals;
	if (true)		loadFileFlags	|= aiProcess_GenUVCoords;
	//if (true)		loadFileFlags	|= aiProcess_OptimizeGraph;			// TODO: opt for optimize load, also use Assimp::Exporter::Export to dump a bin, next time can load faster
	//if (true)		loadFileFlags	|= aiProcess_GenBoundingBoxes;
	//if (true)		loadFileFlags	|= aiProcess_MakeLeftHanded;
	//if (true)		loadFileFlags	|= aiProcess_ConvertToLeftHanded;

	importer.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80.0f);

	bool isExcludePointAndLine = true;
	if (isExcludePointAndLine)
		importer.SetPropertyInteger( AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE );

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

	Vector<AABBox3, 8> aabboxs;
	aabboxs.resize(nMeshes);

	EditMesh edMesh;
	edMesh.pos.reserve(totalVertices);
	edMesh.indices.reserve(totalIndices);
	for (u32 i = 0; i < nMeshes; i++)
	{
		auto& aabbox = aabboxs[i];
		const aiMesh* pAiMesh = srcScene->mMeshes[i]; 
		if (!loadIndices(	&edMesh,			*pAiMesh)) return false;
		if (!loadVertices(	&edMesh, &aabbox,	*pAiMesh)) return false;
		if (!loadBones(		&edMesh,			*pAiMesh)) return false;
		//aabbox = toAABBox3<f32>(pAiMesh->mAABB);
	}

	rdMesh.create(edMesh, nMeshes, aabboxs);
	for (u32 i = 0; i < nMeshes; i++)
	{
		auto&			e		= rdMesh.subMeshes()[i];
		const aiMesh*	srcMesh = srcScene->mMeshes[i]; 
		e.setName(srcMesh->mName.C_Str());
	}

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
AssimpMeshLoader::loadVertices(EditMesh* oEdMesh, AABBox3* oAabbox, const aiMesh& aiMesh)
{
	auto&	edMesh		= *oEdMesh;
	auto&	aabbox		= *oAabbox;
	u32		nVertices	= aiMesh.mNumVertices;

	//auto& mesh = subMeshes[iMeshes];
	bool hasTangents	= aiMesh.mTangents		!= nullptr && aiMesh.mNumVertices > 0;
	bool hasBitangents	= aiMesh.mBitangents	!= nullptr && aiMesh.mNumVertices > 0;
	#if !RDS_USE_ASSIMP_LOAD_TANGENT
	hasTangents   = true;
	hasBitangents = true;
	#endif // !RDS_USE_ASSIMP_LOAD_TANGENT


	if (aiMesh.HasPositions())
	{
		//aabbox.reset(toVec3(aiMesh.mVertices[0]), toVec3(aiMesh.mVertices[0]));
		for (u32 iVertex = 0; iVertex < nVertices; iVertex++)
		{
			auto pos = toVec3(aiMesh.mVertices[iVertex]);
			edMesh.pos.emplace_back(pos);
			aabbox.encapsulate(pos);
		}
	}

	if (aiMesh.HasNormals())
	{
		for (u32 iVertex = 0; iVertex < nVertices; iVertex++)
		{
			edMesh.normal.emplace_back(toVec3(aiMesh.mNormals[iVertex]));
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

	#if !RDS_USE_ASSIMP_LOAD_TANGENT

	if (hasTangents)
	{
		auto&		tangents	= edMesh.tangent;
		auto&		binormals	= edMesh.binormal;

		const auto& positions	= edMesh.pos;
		const auto& uv			= edMesh.uvs[0];
		const auto& normals		= edMesh.normal;


		auto&	indices		= edMesh.indices;

		u32		vtxCount	= sCast<u32>(positions.size());
		u32		idxCount	= sCast<u32>(indices.size());

		tangents.resize( vtxCount);
		binormals.resize(vtxCount);

		for (u32 i = 0; i < idxCount; i += 3)
		{
			u32 i0 = indices[i + 0];
			u32 i1 = indices[i + 1];
			u32 i2 = indices[i + 2];

			auto edge1 = Vec3f{positions[i1]} - Vec3f{positions[i0]};
			auto edge2 = Vec3f{positions[i2]} - Vec3f{positions[i0]};

			auto deltaUv1 = Vec2f{ uv[i1] } - Vec2f{ uv[i0] };
			auto deltaUv2 = Vec2f{ uv[i2] } - Vec2f{ uv[i0] };
			
			/*
			* references:
			* ~ http://www.thetenthplanet.de/archives/1180
			*/
			#if 1

			auto normal = Vec3f{normals[i0]};

			Vec3f dp2perp = edge2.cross(normal);
			Vec3f dp1perp = normal.cross(edge1);

			Vec3f tangent	= dp2perp * deltaUv1.x + dp1perp * deltaUv2.x;
			Vec3f binormal	= dp2perp * deltaUv1.y + dp1perp * deltaUv2.y;

			float invmax = math::rsqrt(math::max(tangent.dot(tangent), binormal.dot(binormal)));
			tangent		= invmax * tangent;
			binormal	= invmax * binormal;

			tangents[i0] = (tangent);
			tangents[i1] = (tangent);
			tangents[i2] = (tangent);

			binormals[i0] = (binormal);
			binormals[i1] = (binormal);
			binormals[i2] = (binormal);

			#else

			float f = 1.0f / (deltaUv1.x * deltaUv2.y - deltaUv2.x * deltaUv1.y);		// f may == nan 

			Vec3f tangent;
			tangent.x = f * (deltaUv2.y * edge1.x - deltaUv1.y * edge2.x);
			tangent.y = f * (deltaUv2.y * edge1.y - deltaUv1.y * edge2.y);
			tangent.z = f * (deltaUv2.y * edge1.z - deltaUv1.y * edge2.z);

			f32 handedness = ((deltaUv1.y * deltaUv2.x - deltaUv2.y * deltaUv1.x) < 0.0f) ? -1.0f : 1.0f;

			tangent = tangent.normalize();
			tangent = tangent * handedness;

			tangents[i0] = (tangent);
			tangents[i1] = (tangent);
			tangents[i2] = (tangent);

			#if 1
			Vec3f binormal;
			binormal.x = f * (-deltaUv2.x * edge1.x + deltaUv1.x * edge2.x);
			binormal.y = f * (-deltaUv2.x * edge1.y + deltaUv1.x * edge2.y);
			binormal.z = f * (-deltaUv2.x * edge1.z + deltaUv1.x * edge2.z);

			binormal = binormal.normalize();
			binormal = binormal * handedness;

			binormals[i0] = (binormal);
			binormals[i1] = (binormal);
			binormals[i2] = (binormal);
			#endif // 0

			#endif // 0

		}
	}
	#else

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
			edMesh.binormal.emplace_back(toVec3(aiMesh.mBitangents[iVertex]));
		}
	}
	#endif

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

	//RDS_LOG("loaded {}", srcNode->mName.C_Str());
	{
		auto& node = *dst;
		fmtTo(node.name,		"{}", srcNode->mName.C_Str());
		node.parent			= parent;
		node.localTransform = toMat4(srcNode->mTransformation);
		node.worldTransform = worldTransform * node.localTransform;
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

			fmtTo(node.name,	"{}-({})", srcNode->mName.C_Str(), i);
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
			dst = Renderer::renderDevice()->createTexture2D(cDesc);

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
								
								if (texType == MaterialData_TextureType::BaseColor
									|| texType == MaterialData_TextureType::Emission
									)
									cDesc.isSrgb = true;

								dst = Renderer::renderDevice()->createTexture2D(cDesc);
								dst->setDebugName(texFilename);

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
					materialDatum.baseColor = toColor4(bufColor4f);
				}

				if (isSuccess(srcMaterial.Get(AI_MATKEY_COLOR_EMISSIVE, bufColor4f)))
				{
					materialDatum.emission = toColor4(bufColor4f);
				}

				if (isSuccess(srcMaterial.Get(AI_MATKEY_ROUGHNESS_FACTOR, bufFloat)))
				{
					materialDatum.roughness = bufFloat;
				}

				if (isSuccess(srcMaterial.Get(AI_MATKEY_METALLIC_FACTOR, bufFloat)))
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

		TempString buf;

		auto nMaterial = materialData.size();
		materials.reserve(nMaterial);
		for (u32 i = 0; i < nMaterial; i++)
		{
			const auto& mtlDatum = materialData[i];
			fmtToNew(buf, "{}[{}]", mtlDatum.name, i);

			auto& mtl = materials.emplace_back();
			mtl = Renderer::renderDevice()->createMaterial(shader);
			mtl->setDebugName(buf);
			meshAsset.materialList.setupMaterial(mtl, i);
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

template<class T> inline
AABBox3<T>	
toAABBox3(const aiAABB& v)
{
	AABBox3<T> o;
	o.min = toVec3(v.mMin);
	o.max = toVec3(v.mMax);
	return o;
}

#endif

#if 0
#pragma mark --- rdsMeshAssetMaterialList-Impl ---
#endif // 0
#if 1

void 
MeshAssetMaterialList::setupMaterial(Material* mtl, SizeType mtlIdx)
{
	auto* shader = mtl->shader();
	if (!shader || mtlIdx >= materialData.size())
		return;

	const auto& mtlDatum = materialData[mtlIdx];

	if (auto* tex = mtlDatum.getTexture(MaterialData_TextureType::BaseColor))			{ mtl->setParam(RDS_STRINGIFY(RDS_MATERIAL_TEXTURE_baseColor),			tex); mtl->setParam(RDS_STRINGIFY(RDS_MATERIAL_TEXTURE_baseColor),			SamplerState::makeLinearRepeat());		mtl->setParam(RDS_STRINGIFY(RDS_MATERIAL_PROPERTY_useTexBaseColor),				sCast<u32>(1)); }
	if (auto* tex = mtlDatum.getTexture(MaterialData_TextureType::Normal))				{ mtl->setParam(RDS_STRINGIFY(RDS_MATERIAL_TEXTURE_normal),				tex); mtl->setParam(RDS_STRINGIFY(RDS_MATERIAL_TEXTURE_normal),				SamplerState::makeLinearRepeat());		mtl->setParam(RDS_STRINGIFY(RDS_MATERIAL_PROPERTY_useTexNormal),				sCast<u32>(1)); }
	if (auto* tex = mtlDatum.getTexture(MaterialData_TextureType::RoughnessMetalness))	{ mtl->setParam(RDS_STRINGIFY(RDS_MATERIAL_TEXTURE_roughnessMetalness),	tex); mtl->setParam(RDS_STRINGIFY(RDS_MATERIAL_TEXTURE_roughnessMetalness),	SamplerState::makeLinearRepeat());		mtl->setParam(RDS_STRINGIFY(RDS_MATERIAL_PROPERTY_useTexRoughnessMetalness),	sCast<u32>(1)); }
	if (auto* tex = mtlDatum.getTexture(MaterialData_TextureType::Emission))			{ mtl->setParam(RDS_STRINGIFY(RDS_MATERIAL_TEXTURE_emission),			tex); mtl->setParam(RDS_STRINGIFY(RDS_MATERIAL_TEXTURE_emission),			SamplerState::makeLinearRepeat());		mtl->setParam(RDS_STRINGIFY(RDS_MATERIAL_PROPERTY_useTexEmisson),				sCast<u32>(1)); }

	mtl->setParam(RDS_STRINGIFY(RDS_MATERIAL_PROPERTY_baseColor),			mtlDatum.baseColor);
	mtl->setParam(RDS_STRINGIFY(RDS_MATERIAL_PROPERTY_metalness),			mtlDatum.metalness);
	mtl->setParam(RDS_STRINGIFY(RDS_MATERIAL_PROPERTY_roughness),			mtlDatum.roughness);
	mtl->setParam(RDS_STRINGIFY(RDS_MATERIAL_PROPERTY_emission),			mtlDatum.emission);
}


#endif // 1


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
MeshAsset::addToScene(Scene* scene, const Mat4f& matrix)
{
	auto* rootNode = nodeList.rootNode;
	if (!rootNode)
	{
		return nullptr;
	}

	auto* node		= rootNode;
	auto* rootEnt	= scene->addEntity(node->name);
	_addToScene(scene, rootEnt, node, matrix);
	return rootEnt;
}

Entity*
MeshAsset::_addToScene(Scene* scene, Entity* parent, MeshAssetNode* node, const Mat4f& matrix)
{
	Entity* ent = nullptr;
	if (node->meshIdx != Asset::s_kInvalidId/* && StrUtil::ignoreCaseCompare(node->name, "sponza_117_sponza_117_floor") == 0*/)
	{
		ent	= scene->addEntity(node->name);
		_setupEntity(ent, node, matrix);
	}
	
	for (auto& e : node->children)
	{
		// set parent in this line
		_addToScene(scene, ent ? ent : parent, e, matrix);
	}
	return ent;
}

void 
MeshAsset::_setupEntity(Entity* dst, MeshAssetNode* node, const Mat4f& localMatrix)
{
	auto* ent = dst;

	RDS_TODO("decompose TRS then set to entity transform");
	Mat4f dstLocalMatrix = localMatrix * node->localTransform;
	
	Vec3f scale;
	Quat4f rotation;
	Vec3f translation;
	Vec3f skew;
	Vec4f perspective;
	glm::decompose(dstLocalMatrix, scale, rotation, translation, skew, perspective);

	auto* transform = ent->getComponent<CTransform>();
	transform->setLocalTRS(translation, rotation, scale);

	auto* rdableMesh = ent->addComponent<CRenderableMesh>();
	rdableMesh->meshAsset		= this;
	rdableMesh->subMeshIndex	= node->meshIdx;
	rdableMesh->materialIndex	= node->materialIdx;
	rdableMesh->material		= node->materialIdx < materialList.materials.size() ? materialList.materials[node->materialIdx] : nullptr;
}

#endif

}