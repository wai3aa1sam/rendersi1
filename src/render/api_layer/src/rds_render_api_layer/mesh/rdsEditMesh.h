#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rds_render_api_layer/vertex/rdsVertexLayoutManager.h"

namespace rds
{

#if 0
#pragma mark --- rdsEditMesh-Impl ---
#endif // 0
#if 1

struct PackedVtxDataHelper;

class EditMesh
{
public:
	using PosType		= Tuple3f;
	using ColorType		= Color4b;
	using UvType		= Tuple2f;
	using NormalType	= Tuple3f;
	using TangentType	= Tuple3f;
	using BinormalType	= Tuple3f;
	using AABBox3		= AABBox3<PosType::ElementType>;

	using SizeType = RenderApiLayerTraits::SizeType;

public:
	static constexpr u8 s_kPosCount			= 1;
	static constexpr u8 s_kColorCount		= 1;
	static constexpr u8 s_kUvCount			= 8;
	static constexpr u8 s_kNormalCount		= 1;
	static constexpr u8 s_kTangentCount		= 1;
	static constexpr u8 s_kBinormalCount	= 1;

public:
	RenderPrimitiveType	primitive = RenderPrimitiveType::Triangle;

	Vector<Tuple3f>	pos;
	Vector<Color4b>	color;
	Vector<Tuple2f>	uvs[s_kUvCount];
	Vector<Tuple3f>	normal;
	Vector<Tuple3f>	tangent;
	Vector<Tuple3f>	binormal;

	Vector<u32> indices;

public:
	template<size_t N>	const VertexLayout*	createPackedVtxData(Vector<u8, N>& out, AABBox3& oAabbox) const;
	template<size_t N>	void				createPackedVtxData(Vector<u8, N>& out, AABBox3& oAabbox, const VertexLayout* vertexLayout) const;
						Vector<u8>			makePackedVtxData() const;

	const VertexLayout* getVertexLayout() const;

	void addColors(const Color4b& c);

	void clear();
};

template<size_t N> inline
const VertexLayout*
EditMesh::createPackedVtxData(Vector<u8, N>& out, AABBox3& oAabbox) const
{
	return PackedVtxDataHelper::createPackedVtxData(out, oAabbox
		, &pos,			s_kPosCount
		, &color,		s_kColorCount
		, uvs,			s_kUvCount
		, &normal,		s_kNormalCount
		, &tangent,		s_kTangentCount
		, &binormal,	s_kBinormalCount
	);
}

template<size_t N> inline 
void
EditMesh::createPackedVtxData(Vector<u8, N>& out, AABBox3& oAabbox, const VertexLayout* vertexLayout) const
{
	return PackedVtxDataHelper::createPackedVtxData(out, oAabbox
		, vertexLayout
		, &pos,			s_kPosCount
		, &color,		s_kColorCount
		, uvs,			s_kUvCount
		, &normal,		s_kNormalCount
		, &tangent,		s_kTangentCount
		, &binormal,	s_kBinormalCount
	);
}

inline Vector<u8> EditMesh::makePackedVtxData() const 
{
	Vector<u8> o; 
	AABBox3 oAabbox3; 
	createPackedVtxData(o, oAabbox3); 
	return o; 
}

#endif

#if 0
#pragma mark --- rdsEditMeshUtil-Decl ---
#endif // 0
#if 1

class EditMeshUtil
{
public:
	static EditMesh getFullScreenTriangle();
};

#endif // 1

#if 0
#pragma mark --- rdsEditMeshT-Impl ---
#endif // 0
#if 1

template< class POS_TYPE,		u8 POS_COUNT
		, class COLOR_TYPE,		u8 COLOR_COUNT
		, class UV_TYPE,		u8 UV_COUNT
		, class NORMAL_TYPE,	u8 NORMAL_COUNT
		, class TANGNET_TYPE,	u8 TANGENT_COUNT
		, class BINORMAL_TYPE,	u8 BINORMAL_COUNT>
class EditMeshTC : public NonCopyable
{
public:
	using PosType		= POS_TYPE;
	using ColorType		= COLOR_TYPE;
	using UvType		= UV_TYPE;
	using NormalType	= NORMAL_TYPE;
	using TangentType	= TANGNET_TYPE;
	using BinormalType	= BINORMAL_TYPE;
	using AABBox3		= AABBox3<typename PosType::ElementType>;

	using SizeType = RenderApiLayerTraits::SizeType;


public:
	static constexpr u8 s_kPosCount			= POS_COUNT;
	static constexpr u8 s_kColorCount		= COLOR_COUNT;
	static constexpr u8 s_kUvCount			= UV_COUNT;
	static constexpr u8 s_kNormalCount		= NORMAL_COUNT;
	static constexpr u8 s_kTangentCount		= TANGENT_COUNT;
	static constexpr u8 s_kBinormalCount	= BINORMAL_COUNT;

public:
	template<size_t N>	void		createPackedVtxData(Vector<u8, N>& out, AABBox3& oAabbox);
						Vector<u8>	makePackedVtxData();

public:
	Vector<PosType>			positions[	s_kPosCount];
	Vector<ColorType>		colors[		s_kColorCount];
	Vector<UvType>			uvs[		s_kUvCount];
	Vector<NormalType>		normals[	s_kNormalCount];
	Vector<TangentType>		tangents[	s_kTangentCount];
	Vector<BinormalType>	binormals[	s_kBinormalCount];

};

template<u8 POS_COUNT, u8 COLOR_COUNT, u8 UV_COUNT, u8 NORMAL_COUNT, u8 TANGENT_COUNT, u8 BINORMAL_COUNT> 
using EditMeshC = EditMeshTC<Tuple3f, POS_COUNT
							, Color4b, COLOR_COUNT
							, Tuple2f, UV_COUNT
							, Tuple3f, NORMAL_COUNT
							, Tuple3f, TANGENT_COUNT
							, Tuple3f, BINORMAL_COUNT
							>;

template< class PT,	u8 PC
		, class CT,	u8 CC
		, class UT, u8 UC
		, class NT,	u8 NC
		, class TT,	u8 TC
		, class BT,	u8 BC>
template<size_t N> inline
void
EditMeshTC<PT, PC, CT, CC, UT, UC, NT, NC, TT, TC, BT, BC>::createPackedVtxData(Vector<u8, N>& out, AABBox3& oAabbox)
{
	PackedVtxDataHelper::createPackedVtxData(out, oAabbox
												, positions,	s_kPosCount
												, colors,		s_kColorCount
												, uvs,			s_kUvCount
												, normals,		s_kNormalCount
												, tangents,		s_kTangentCount
												, binormals,	s_kBinormalCount
											);
}

template< class PT,	u8 PC
	, class CT,	u8 CC
	, class UT, u8 UC
	, class NT,	u8 NC
	, class TT,	u8 TC
	, class BT,	u8 BC> inline
Vector<u8> 
EditMeshTC<PT, PC, CT, CC, UT, UC, NT, NC, TT, TC, BT, BC>::makePackedVtxData()
{
	Vector<u8>	o;
	AABBox3		oAabbox;
	createPackedVtxData(o, oAabbox); 
	return o;
}

#endif

#if 0
#pragma mark --- rdsPackedVtxDataHelper-Impl ---
#endif // 0
#if 1

struct PackedVtxDataHelper
{
	using SizeType = RenderApiLayerTraits::SizeType;


	template<size_t N
		, class POS_TYPE
		, class COLOR_TYPE
		, class UV_TYPE
		, class NORMAL_TYPE
		, class TANGNET_TYPE
		, class BINORMAL_TYPE
	>
	static const VertexLayout* createPackedVtxData(Vector<u8, N>& out, AABBox3<typename POS_TYPE::ElementType>& oAabbox
									, const Vector<POS_TYPE>*		positions,		u8 posCount
									, const Vector<COLOR_TYPE>*		colors,			u8 colorCount
									, const Vector<UV_TYPE>*		uvs,			u8 uvCount
									, const Vector<NORMAL_TYPE>*	normals,		u8 normalCount
									, const Vector<TANGNET_TYPE>*	tangents,		u8 tangentCount
									, const Vector<BINORMAL_TYPE>*	binormals,		u8 binormalCount
		);

	template<size_t N
		, class POS_TYPE
		, class COLOR_TYPE
		, class UV_TYPE
		, class NORMAL_TYPE
		, class TANGNET_TYPE
		, class BINORMAL_TYPE
	>
	static void createPackedVtxData(Vector<u8, N>& out, AABBox3<typename POS_TYPE::ElementType>& oAabbox
									, const VertexLayout* vertexLayout
									, const Vector<POS_TYPE>*		positions,		u8 posCount
									, const Vector<COLOR_TYPE>*		colors,			u8 colorCount
									, const Vector<UV_TYPE>*		uvs,			u8 uvCount
									, const Vector<NORMAL_TYPE>*	normals,		u8 normalCount
									, const Vector<TANGNET_TYPE>*	tangents,		u8 tangentCount
									, const Vector<BINORMAL_TYPE>*	binormals,		u8 binormalCount
		);

	template< class POS_TYPE
			, class COLOR_TYPE
			, class UV_TYPE
			, class NORMAL_TYPE
			, class TANGNET_TYPE
			, class BINORMAL_TYPE
	>
	static const VertexLayout* getVertexLayout(const Vector<POS_TYPE>*			positions,		u8 posCount
											  , const Vector<COLOR_TYPE>*		colors,			u8 colorCount
											  , const Vector<UV_TYPE>*			uvs,			u8 uvCount
											  , const Vector<NORMAL_TYPE>*		normals,		u8 normalCount
											  , const Vector<TANGNET_TYPE>*		tangents,		u8 tangentCount
											  , const Vector<BINORMAL_TYPE>*	binormals,		u8 binormalCount);


	template<class T>
	static u8 validVtxAttrCount(SizeType vtxCount, const Vector<T>* pElements, size_t count)
	{
		u8 attrCount = 0;
		for (size_t i = 0; i < count; i++)
		{
			auto& elements	= pElements[i];
			bool isValid	= elements.size() <= vtxCount;
			throwIf(!isValid, "invalid vertex attr");
			if (elements.size() > 0)
			{
				attrCount++;
			}
		}
		return attrCount;
	}

	template<class T>
	static void copyVertexData(u8* dst, const T* src, size_t count, size_t stride, size_t offset)
	{
		u8* p = dst + offset;
		for (size_t i = 0; i < count; i++)
		{
			*reinCast<T*>(p) = src[i];
			p += stride;
		}
	}
};

template< class POS_TYPE
		, class COLOR_TYPE
		, class UV_TYPE
		, class NORMAL_TYPE
		, class TANGNET_TYPE
		, class BINORMAL_TYPE
> inline
const VertexLayout*
PackedVtxDataHelper::getVertexLayout(const Vector<POS_TYPE>*		positions,		u8 posCount
								   , const Vector<COLOR_TYPE>*		colors,			u8 colorCount
								   , const Vector<UV_TYPE>*			uvs,			u8 uvCount
								   , const Vector<NORMAL_TYPE>*		normals,		u8 normalCount
								   , const Vector<TANGNET_TYPE>*	tangents,		u8 tangentCount
								   , const Vector<BINORMAL_TYPE>*	binormals,		u8 binormalCount)
{
	using PosType		= POS_TYPE;
	using ColorType_	= COLOR_TYPE;
	using UvType		= UV_TYPE;
	using NormalType	= NORMAL_TYPE;
	using TangentType	= TANGNET_TYPE;
	using BinormalType	= BINORMAL_TYPE;

	using Helper	= PackedVtxDataHelper;
	using SizeType	= Helper::SizeType;

	RDS_CORE_ASSERT(posCount == 1, "only 1 pos count supported");
	static_assert(IsSame<NormalType, TangentType> && IsSame<NormalType, BinormalType>, "");

	SizeType vtxCount = positions[0].size();
	auto validPosCount		= Helper::validVtxAttrCount(vtxCount, positions,	posCount); RDS_UNUSED(validPosCount);
	auto validColorCount	= Helper::validVtxAttrCount(vtxCount, colors,		colorCount);
	auto validUvCount		= Helper::validVtxAttrCount(vtxCount, uvs,			uvCount);
	auto validNormalCount	= Helper::validVtxAttrCount(vtxCount, normals,		normalCount);
	auto validTangentCount	= Helper::validVtxAttrCount(vtxCount, tangents,		tangentCount);
	auto validBinormalCount	= Helper::validVtxAttrCount(vtxCount, binormals,	binormalCount);

	VertexType vertexType = VertexTypeUtil::make( RenderDataTypeUtil::get<PosType>()
		, RenderDataTypeUtil::get<ColorType_>(),	validColorCount
		, RenderDataTypeUtil::get<UvType>(),		validUvCount
		, RenderDataTypeUtil::get<NormalType>(),	validNormalCount, validTangentCount, validBinormalCount
	);

	const auto*	vtxLayout = VertexLayoutManager::instance()->get(vertexType);
	return vtxLayout;
}

template<size_t N
		, class POS_TYPE
		, class COLOR_TYPE
		, class UV_TYPE
		, class NORMAL_TYPE
		, class TANGNET_TYPE
		, class BINORMAL_TYPE
		> inline
const VertexLayout* 
PackedVtxDataHelper::createPackedVtxData(Vector<u8, N>& out, AABBox3<typename POS_TYPE::ElementType>& oAabbox
							, const Vector<POS_TYPE>*		positions,		u8 posCount
							, const Vector<COLOR_TYPE>*		colors,			u8 colorCount
							, const Vector<UV_TYPE>*		uvs,			u8 uvCount
							, const Vector<NORMAL_TYPE>*	normals,		u8 normalCount
							, const Vector<TANGNET_TYPE>*	tangents,		u8 tangentCount
							, const Vector<BINORMAL_TYPE>*	binormals,		u8 binormalCount
					)
{
	using PosType		= POS_TYPE;
	using ColorType_	= COLOR_TYPE;
	using UvType		= UV_TYPE;
	using NormalType	= NORMAL_TYPE;
	using TangentType	= TANGNET_TYPE;
	using BinormalType	= BINORMAL_TYPE;

	using Helper	= PackedVtxDataHelper;
	using SizeType	= Helper::SizeType;

	RDS_CORE_ASSERT(posCount == 1, "only 1 pos count supported");
	static_assert(IsSame<NormalType, TangentType> && IsSame<NormalType, BinormalType>, "");

	const auto*	vtxLayout = Helper::getVertexLayout(positions,	posCount
												, colors,		colorCount
												, uvs,			uvCount
												, normals,		normalCount
												, tangents,		tangentCount
												, binormals,	binormalCount);
	createPackedVtxData(out, oAabbox
		, vtxLayout
		, positions,	posCount
		, colors,		colorCount
		, uvs,			uvCount
		, normals,		normalCount
		, tangents,		tangentCount
		, binormals,	binormalCount);

	return vtxLayout;
}

template<size_t N
	, class POS_TYPE
	, class COLOR_TYPE
	, class UV_TYPE
	, class NORMAL_TYPE
	, class TANGNET_TYPE
	, class BINORMAL_TYPE
>
inline
void
PackedVtxDataHelper::createPackedVtxData(Vector<u8, N>& out, AABBox3<typename POS_TYPE::ElementType>& oAabbox
										, const VertexLayout* vertexLayout
										, const Vector<POS_TYPE>*		positions,		u8 posCount
										, const Vector<COLOR_TYPE>*		colors,			u8 colorCount
										, const Vector<UV_TYPE>*		uvs,			u8 uvCount
										, const Vector<NORMAL_TYPE>*	normals,		u8 normalCount
										, const Vector<TANGNET_TYPE>*	tangents,		u8 tangentCount
										, const Vector<BINORMAL_TYPE>*	binormals,		u8 binormalCount
)
{
	using PosType		= POS_TYPE;
	using ColorType_	= COLOR_TYPE;
	using UvType		= UV_TYPE;
	using NormalType	= NORMAL_TYPE;
	using TangentType	= TANGNET_TYPE;
	using BinormalType	= BINORMAL_TYPE;

	using Helper	= PackedVtxDataHelper;
	using SizeType	= Helper::SizeType;
	using Tuple3	= Tuple3<typename PosType::ElementType>;


	RDS_CORE_ASSERT(posCount == 1, "only 1 pos count supported");
	static_assert(IsSame<NormalType, TangentType> && IsSame<NormalType, BinormalType>, "");

	const auto* vtxLayout = vertexLayout;
		
	RDS_CORE_ASSERT(vtxLayout 
					==	Helper::getVertexLayout(positions,	posCount
												, colors,		colorCount
												, uvs,			uvCount
												, normals,		normalCount
												, tangents,		tangentCount
												, binormals,	binormalCount), "");

	auto stride = vtxLayout->stride();

	SizeType vtxCount = positions[0].size();

	out.clear();
	out.resize(vtxCount * stride);
	auto* o = out.data();

	for (auto& e : vtxLayout->elements())
	{
		using SemanticT = VertexSemanticType;

		//auto& e = vtxLayout->elements(i);
		auto offset			= e.offset;
		auto semanticType	= VertexSemanticUtil::getSemantic(e.semantic);
		auto semanticIdx	= VertexSemanticUtil::getSemanticIdx(e.semantic);

		switch (semanticType)
		{
			case SemanticT::SV_Position:
			case SemanticT::POSITION:	
			{ 
				auto& src = positions[semanticIdx];	
				//Helper::copyVertexData(o, src.data(), src.size(), stride, offset); 
				auto*	srcData = src.data();
				u8*		dstData = o + offset;
				for (size_t i = 0; i < vtxCount; i++) 
				{
					*reinCast<Tuple3*>(dstData) = *srcData;
					oAabbox.encapsulate(*srcData);

					srcData++;
					dstData += stride;
				}
			} break;
			case SemanticT::COLOR:		{ auto& src = colors[	semanticIdx];	Helper::copyVertexData(o, src.data(), src.size(), stride, offset); } break;
			case SemanticT::TEXCOORD:	{ auto& src = uvs[		semanticIdx];	Helper::copyVertexData(o, src.data(), src.size(), stride, offset); } break;
			case SemanticT::NORMAL:		{ auto& src = normals[	semanticIdx];	Helper::copyVertexData(o, src.data(), src.size(), stride, offset); } break;
			case SemanticT::TANGENT:	{ auto& src = tangents[	semanticIdx];	Helper::copyVertexData(o, src.data(), src.size(), stride, offset); } break;
			case SemanticT::BINORMAL:	{ auto& src = binormals[semanticIdx];	Helper::copyVertexData(o, src.data(), src.size(), stride, offset); } break;
		}
	}

	//return vtxLayout;
}


#endif // 1

inline
const VertexLayout* 
EditMesh::getVertexLayout() const
{
	return PackedVtxDataHelper::getVertexLayout(
		&pos,			s_kPosCount
		, &color,		s_kColorCount
		, uvs,			s_kUvCount
		, &normal,		s_kNormalCount
		, &tangent,		s_kTangentCount
		, &binormal,	s_kBinormalCount
	);
}

}