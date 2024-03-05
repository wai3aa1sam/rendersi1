#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"



namespace rds
{

#if 0
#pragma mark --- rdsVertexType-Impl ---
#endif // 0
#if 1

enum class VertexType : u64 { None };
template<>
struct TypeBitMixture_Impl<VertexType>
{
	using Types								= Tuple<
												RenderDataType					// posType,
												, RenderDataType, u8			// colorType,	bitCount
												, RenderDataType, u8			// uvType,		bitCount
												, RenderDataType, u8, u8, u8	// normalType,	normalBitCount, tangentBitCount, binormalBitCount
												//, u8							// vertexId (onoff)
											>;
	static constexpr size_t s_kBitCounts[]	= {  8
												 , 8, 2
												 , 8, 8
												 , 8, 2, 2, 2
												 //, 1
												};
};
struct VertexTypeUtil
{
public:
	using Type		= VertexType;
	using IntType	= EnumInt<VertexType>;
	using SizeType	= RenderApiLayerTraits::SizeType;

public:
	static constexpr SizeType s_kPosTypeIdx = 0;

	static constexpr SizeType s_kColorTypeIdx	= 1;
	static constexpr SizeType s_kColorCountIdx	= 2;
	static constexpr SizeType s_kColorCount		= TBM<VertexType>::maxValues(s_kColorCountIdx) + 1;

	static constexpr SizeType s_kUvTypeIdx		= 3;
	static constexpr SizeType s_kUvCountIdx		= 4;
	static constexpr SizeType s_kUvCount		= TBM<VertexType>::maxValues(s_kUvCountIdx) + 1;

	static constexpr SizeType s_kNormalTypeIdx		= 5;
	static constexpr SizeType s_kNormalCountIdx		= 6;
	static constexpr SizeType s_kTangentCountIdx	= 7;
	static constexpr SizeType s_kBinormalCountIdx	= 8;
	static constexpr SizeType s_kNormalCount		= TBM<VertexType>::maxValues(s_kNormalCountIdx)		+ 1;
	static constexpr SizeType s_kTangentCount		= TBM<VertexType>::maxValues(s_kTangentCountIdx)	+ 1;
	static constexpr SizeType s_kBinormalCount		= TBM<VertexType>::maxValues(s_kBinormalCountIdx)	+ 1;


public:
	static constexpr VertexType addPos		(VertexType t, RenderDataType posType);
	static constexpr VertexType addColor	(VertexType t, RenderDataType colorType,	u8 count);
	static constexpr VertexType addUv		(VertexType t, RenderDataType uvType,		u8 count);
	static constexpr VertexType addNormal	(VertexType t, RenderDataType normalType,	u8 count);
	static constexpr VertexType addTangent	(VertexType t, u8 count);
	static constexpr VertexType addBinormal	(VertexType t, u8 count);

	RDS_NODISCARD static constexpr VertexType make(RenderDataType posType
										, RenderDataType colorType,		u8 colorCount
										, RenderDataType uvType,		u8 uvCount
										, RenderDataType normalType,	u8 normalCount,	u8 tangentCount, u8 binormalCount
									);
};

inline constexpr VertexTypeUtil::Type VertexTypeUtil::addPos		(VertexType t, RenderDataType posType)					{ return TBM<Type>::addElement<s_kPosTypeIdx>(t, posType); }
inline constexpr VertexTypeUtil::Type VertexTypeUtil::addColor		(VertexType t, RenderDataType colorType,	u8 count)	{ auto ret = TBM<Type>::addElement<s_kColorTypeIdx>(t, colorType);		return TBM<Type>::addElement<s_kColorCountIdx>(ret, count); }
inline constexpr VertexTypeUtil::Type VertexTypeUtil::addUv			(VertexType t, RenderDataType uvType,		u8 count)	{ auto ret = TBM<Type>::addElement<s_kUvTypeIdx>(t, uvType);			return TBM<Type>::addElement<s_kUvCountIdx>(ret, count); }
inline constexpr VertexTypeUtil::Type VertexTypeUtil::addNormal		(VertexType t, RenderDataType normalType,	u8 count)	{ auto ret = TBM<Type>::addElement<s_kNormalTypeIdx>(t, normalType);	return TBM<Type>::addElement<s_kNormalCountIdx>(ret, count); }
inline constexpr VertexTypeUtil::Type VertexTypeUtil::addTangent	(VertexType t, u8 count)								{ return TBM<Type>::addElement<s_kTangentCountIdx>(t, count); }
inline constexpr VertexTypeUtil::Type VertexTypeUtil::addBinormal	(VertexType t, u8 count)								{ return TBM<Type>::addElement<s_kBinormalCountIdx>(t, count); }

inline constexpr 
VertexTypeUtil::Type 
VertexTypeUtil::make(RenderDataType posType
					, RenderDataType colorType,		u8 colorCount
					, RenderDataType uvType,		u8 uvCount
					, RenderDataType normalType,	u8 normalCount,	u8 tangentCount, u8 binormalCount
)
{
	VertexType t = addPos(VertexType::None, posType);
	if (colorCount)
		t = addColor(t, colorType, colorCount);
	if (uvCount)
		t = addUv(t, uvType, uvCount);
	if (normalCount)
	{
		t =   addNormal(t, normalType, normalCount);
		t =  addTangent(t, tangentCount);
		t = addBinormal(t, binormalCount);
	}
	return t;
}

#endif

#if 0
#pragma mark --- VertexSemantic-Impl ---
#endif // 0
#if 1

#define VertexSemanticType_ENUM_LIST(E) \
	E(None, = 0) \
	\
	E(POSITION,) \
	E(COLOR,) \
	E(TEXCOORD,) \
	E(NORMAL,) \
	E(TANGENT,) \
	E(BINORMAL,) \
	\
	E(SV_Position,) \
	E(SV_VertexID,) \
	E(SV_Target,) \
	\
	E(_kCount,) \
//---
RDS_ENUM_CLASS(VertexSemanticType, u8);

enum class VertexSemantic : u16;
template<>
struct TypeBitMixture_Impl<VertexSemantic>
{
	using Types								= Tuple<u8, VertexSemanticType>;	// u8 (count) first ensure the TEX0 and TEX1, ... are next to each other
	static constexpr size_t s_kBitCounts[]	= { 8, 8 };
};

struct VertexSemanticUtil
{
public:
	using ValueType = VertexSemantic;
	using IntType	= EnumInt<VertexSemantic>;

	using SemanticT	= VertexSemanticType;

public:
	VertexSemanticUtil() = delete;

public:
	static constexpr ValueType make(VertexSemanticType t, u8 idx) { return sCast<ValueType>(makeInt(t, idx)); }

	static constexpr IntType makeInt(VertexSemanticType t, u8 idx)
	{
		return sCast<IntType>(t) << TBM<VertexSemantic>::offsets(1) | sCast<IntType>(idx);
	}

	static constexpr SemanticT	getSemantic		(VertexSemantic x) { return TBM<VertexSemantic>::getElementValue<1>(x); }
	static constexpr u8			getSemanticIdx	(VertexSemantic x) { return TBM<VertexSemantic>::getElementValue<0>(x); }

	static VertexSemantic parse(StrView v);
};

#define VertexSemantic_ENUM_LIST(E) \
	E(None, = 0) \
	\
	E(POSITION,		= VertexSemanticUtil::makeInt(VertexSemanticType::POSITION, 0)) \
	\
	E(COLOR0,		= VertexSemanticUtil::makeInt(VertexSemanticType::COLOR, 0)) \
	E(COLOR1,		= VertexSemanticUtil::makeInt(VertexSemanticType::COLOR, 1)) \
	E(COLOR2,		= VertexSemanticUtil::makeInt(VertexSemanticType::COLOR, 2)) \
	E(COLOR3,		= VertexSemanticUtil::makeInt(VertexSemanticType::COLOR, VertexTypeUtil::s_kColorCount - 1)) \
	\
	E(TEXCOORD0,	= VertexSemanticUtil::makeInt(VertexSemanticType::TEXCOORD, 0)) \
	E(TEXCOORD1,	= VertexSemanticUtil::makeInt(VertexSemanticType::TEXCOORD, 1)) \
	E(TEXCOORD2,	= VertexSemanticUtil::makeInt(VertexSemanticType::TEXCOORD, 2)) \
	E(TEXCOORD3,	= VertexSemanticUtil::makeInt(VertexSemanticType::TEXCOORD, 3)) \
	E(TEXCOORD4,	= VertexSemanticUtil::makeInt(VertexSemanticType::TEXCOORD, 4)) \
	E(TEXCOORD5,	= VertexSemanticUtil::makeInt(VertexSemanticType::TEXCOORD, 5)) \
	E(TEXCOORD6,	= VertexSemanticUtil::makeInt(VertexSemanticType::TEXCOORD, 6)) \
	E(TEXCOORD7,	= VertexSemanticUtil::makeInt(VertexSemanticType::TEXCOORD, 7)) \
	E(TEXCOORD8,	= VertexSemanticUtil::makeInt(VertexSemanticType::TEXCOORD, 8)) \
	E(TEXCOORD9,	= VertexSemanticUtil::makeInt(VertexSemanticType::TEXCOORD, 9)) \
	\
	E(TEXCOORD10,	= VertexSemanticUtil::makeInt(VertexSemanticType::TEXCOORD, 10)) \
	E(TEXCOORD11,	= VertexSemanticUtil::makeInt(VertexSemanticType::TEXCOORD, 11)) \
	E(TEXCOORD12,	= VertexSemanticUtil::makeInt(VertexSemanticType::TEXCOORD, 12)) \
	E(TEXCOORD13,	= VertexSemanticUtil::makeInt(VertexSemanticType::TEXCOORD, 13)) \
	E(TEXCOORD14,	= VertexSemanticUtil::makeInt(VertexSemanticType::TEXCOORD, 14)) \
	E(TEXCOORD15,	= VertexSemanticUtil::makeInt(VertexSemanticType::TEXCOORD, 15)) \
	E(TEXCOORD16,	= VertexSemanticUtil::makeInt(VertexSemanticType::TEXCOORD, 16)) \
	E(TEXCOORD17,	= VertexSemanticUtil::makeInt(VertexSemanticType::TEXCOORD, 17)) \
	E(TEXCOORD18,	= VertexSemanticUtil::makeInt(VertexSemanticType::TEXCOORD, 18)) \
	E(TEXCOORD19,	= VertexSemanticUtil::makeInt(VertexSemanticType::TEXCOORD, 19)) \
	\
	E(TEXCOORD255,	= VertexSemanticUtil::makeInt(VertexSemanticType::TEXCOORD, VertexTypeUtil::s_kUvCount - 1)) \
	\
	E(NORMAL0,		= VertexSemanticUtil::makeInt(VertexSemanticType::NORMAL, 0)) \
	E(NORMAL1,		= VertexSemanticUtil::makeInt(VertexSemanticType::NORMAL, 1)) \
	E(NORMAL2,		= VertexSemanticUtil::makeInt(VertexSemanticType::NORMAL, VertexTypeUtil::s_kNormalCount - 1)) \
	\
	E(TANGENT0,		= VertexSemanticUtil::makeInt(VertexSemanticType::TANGENT, 0)) \
	E(TANGENT1,		= VertexSemanticUtil::makeInt(VertexSemanticType::TANGENT, 1)) \
	E(TANGENT2,		= VertexSemanticUtil::makeInt(VertexSemanticType::TANGENT, VertexTypeUtil::s_kTangentCount - 1)) \
	\
	E(BINORMAL0,	= VertexSemanticUtil::makeInt(VertexSemanticType::BINORMAL, 0)) \
	E(BINORMAL1,	= VertexSemanticUtil::makeInt(VertexSemanticType::BINORMAL, 1)) \
	E(BINORMAL2,	= VertexSemanticUtil::makeInt(VertexSemanticType::BINORMAL, VertexTypeUtil::s_kBinormalCount - 1)) \
	\
	E(SV_Position,	= VertexSemanticUtil::makeInt(VertexSemanticType::SV_Position, 0)) \
	E(SV_VertexID,	= VertexSemanticUtil::makeInt(VertexSemanticType::SV_VertexID, 0)) \
	\
	E(SV_TARGET0,	= VertexSemanticUtil::makeInt(VertexSemanticType::SV_Target, 0)) \
	E(SV_TARGET1,	= VertexSemanticUtil::makeInt(VertexSemanticType::SV_Target, 1)) \
	E(SV_TARGET2,	= VertexSemanticUtil::makeInt(VertexSemanticType::SV_Target, 2)) \
	E(SV_TARGET3,	= VertexSemanticUtil::makeInt(VertexSemanticType::SV_Target, 3)) \
	E(SV_TARGET4,	= VertexSemanticUtil::makeInt(VertexSemanticType::SV_Target, 4)) \
	E(SV_TARGET5,	= VertexSemanticUtil::makeInt(VertexSemanticType::SV_Target, 5)) \
	\
	E(_kCount,) \
//---
RDS_ENUM_CLASS(VertexSemantic, u16);
RDS_ENUM_ALL_OPERATOR(VertexSemantic);

inline
VertexSemantic 
VertexSemanticUtil::parse(StrView v)
{
	if (StrUtil::ignoreCaseCompare(v, "POSITION")		== 0)	{ return VertexSemantic::POSITION; }
	if (StrUtil::ignoreCaseCompare(v, "COLOR")			== 0)	{ return VertexSemantic::COLOR0; }
	if (StrUtil::ignoreCaseCompare(v, "TEXTURE")		== 0)	{ return VertexSemantic::TEXCOORD0; }
	if (StrUtil::ignoreCaseCompare(v, "NORMAL")			== 0)	{ return VertexSemantic::NORMAL0; }
	if (StrUtil::ignoreCaseCompare(v, "TANGENT")		== 0)	{ return VertexSemantic::TANGENT0; }
	if (StrUtil::ignoreCaseCompare(v, "BINORMAL")		== 0)	{ return VertexSemantic::BINORMAL0; }

	if (StrUtil::ignoreCaseCompare(v, "SV_Position")	== 0)	{ return VertexSemantic::SV_Position; }
	if (StrUtil::ignoreCaseCompare(v, "SV_Target")		== 0)	{ return VertexSemantic::SV_TARGET0; }
	if (StrUtil::ignoreCaseCompare(v, "SV_VertexID")	== 0)	{ return VertexSemantic::SV_VertexID; }
	
	TempString buf;
	StrUtil::toUpperCase(buf, v);

	VertexSemantic o;
	throwIf(!enumTryParse(o, buf), "VertexSemantic parse failed");
	return o;
}

#endif

#if 0
#pragma mark --- VertexLayout-Impl ---
#endif // 0
#if 1

struct VertexLayoutElement
{
	RenderDataType dataType	= RenderDataType::None;
	VertexSemantic semantic	= VertexSemantic::None;
	u16 offset = 0;
};

class VertexLayout
{
	friend class VertexLayoutManager;
	template<class T>						friend struct VertexT_Pos;
	template<class T, size_t N, class BASE> friend struct VertexT_Color;
	template<class T, size_t N, class BASE>	friend struct VertexT_Uv;
	template<class T, size_t N, class BASE>	friend struct VertexT_Normal;
	template<class T, size_t N, class BASE>	friend struct VertexT_Tangent;
	template<class T, size_t N, class BASE>	friend struct VertexT_Binormal;

public:
	using SizeType	= RenderApiLayerTraits::SizeType;
	using Element	= VertexLayoutElement;

public:
	static bool	isPositionSemantic(VertexSemantic smt) { return smt == VertexSemantic::POSITION || smt == VertexSemantic::SV_Position; }

public:
	Span<const VertexLayoutElement>	elements()				const { return _elements.span(); }
	const VertexLayoutElement&		elements(SizeType i)	const { return _elements[i]; }

	VertexType						vertexType()			const { return _vertexType; };
	SizeType						stride()				const { return _stride; };

	const Element* find(VertexSemantic semantic) const
	{
		if (isPositionSemantic(semantic))
		{
			for (const auto& e : elements())
			{
				if (isPositionSemantic(e.semantic))
				{
					return &e;
				}
			}
		}
		for (const auto& e : elements())
		{
			if (e.semantic == semantic)
			{
				return &e;
			}
			
		}
		return nullptr;
	}

protected:
	template<class VERTEX, class ELEMENT>
	void addElement(VertexSemantic semantic, ELEMENT VERTEX::* e)
	{
		if constexpr (IsArray<ELEMENT>)
		{
			auto count = ArraySize<ELEMENT>;
			for (size_t i = 0; i < count; i++)
			{
				_addElement(semantic + sCast<VertexSemantic>(i), e, i);
			}
		}
		else
		{
			_addElement(semantic, e, 0);
		}
	}

	template<class VERTEX, class ELEMENT>
	void _addElement(VertexSemantic vtxSmt, ELEMENT VERTEX::* e, size_t i)
	{
		using ElementType = RemoveArr<ELEMENT>;

		RDS_CORE_ASSERT(!(VertexSemanticUtil::getSemantic(vtxSmt) == VertexSemanticType::SV_Position && i > 0), "not support POS_COUNT > 1, if posCount > 1, the VertexSemantic should be POSITION0/1/2...");

		auto& element = _elements.emplace_back();
		element.dataType	= RenderDataTypeUtil::get<ElementType>();
		element.semantic	= vtxSmt;
		element.offset		= sCast<decltype(element.offset)>(memberOffset(e) + sizeof(ElementType) * i);
		
		//RDS_DUMP_VAR(element.dataType, semantic, element.offset);
	}

private:
	VertexType	_vertexType;
	SizeType	_stride;
	Vector<VertexLayoutElement, 12> _elements;
};


#endif


#if 0
#pragma mark --- VertexT_Base-Decl ---
#endif // 0
#if 1

struct VertexT_Base
{
public:
	using SizeType = RenderApiLayerTraits::SizeType;
	using Semantic = VertexSemantic;

public:
	static constexpr RenderDataType s_kPosType		= RenderDataType::None;
	static constexpr RenderDataType s_kColorType	= RenderDataType::None;
	static constexpr RenderDataType s_kUvType		= RenderDataType::None;
	static constexpr RenderDataType s_kNormalType	= RenderDataType::None;

	static constexpr SizeType s_kColorCount			= 0;
	static constexpr SizeType s_kUvCount			= 0;
	static constexpr SizeType s_kNormalCount		= 0;
	static constexpr SizeType s_kTangentCount		= 0;
	static constexpr SizeType s_kBinormalCount		= 0;

	static constexpr VertexType		s_kVertexType = VertexType::None;
	
public:

	static void _internal_onRegister(VertexLayout* out) {}

private:

};

template<class POS_TYPE>
struct VertexT_Pos : public VertexT_Base
{
public:
	using Base = VertexT_Base;
	using This = VertexT_Pos<POS_TYPE>;

	using PosType = POS_TYPE;

public:
	static constexpr RenderDataType s_kPosType		= RenderDataTypeUtil::get<PosType>();

	static constexpr VertexType		s_kVertexType	= VertexTypeUtil::addPos(Base::s_kVertexType, s_kPosType);

public:
	PosType position;

public:
	static const VertexLayout* vertexLayout()
	{
		static const VertexLayout* p = VertexLayoutManager::instance()->get(s_kVertexType);
		return p;
	}

	static void _internal_onRegister(VertexLayout* out)
	{
		Base::_internal_onRegister(out);
		out->addElement(Semantic::SV_Position, &This::position);
	}
};

template<class COLOR_TYPE, size_t COLOR_COUNT, class BASE>
struct VertexT_Color : public BASE
{
public:
	using Base = BASE;
	using This = VertexT_Color<COLOR_TYPE, COLOR_COUNT, BASE>;

	using typename Base::Semantic;
	using typename Base::SizeType;

	using ColorType = COLOR_TYPE;

public:
	static constexpr RenderDataType s_kColorType	= RenderDataTypeUtil::get<ColorType>();
	static constexpr size_t			s_kColorCount	= COLOR_COUNT;

	static constexpr VertexType		s_kVertexType = VertexTypeUtil::addColor(Base::s_kVertexType, s_kColorType, s_kColorCount);

public:
	ColorType colors[s_kColorCount];

public:
	static const VertexLayout* vertexLayout()
	{
		static const VertexLayout* p = VertexLayoutManager::instance()->get(s_kVertexType);
		return p;
	}

	static void _internal_onRegister(VertexLayout* out)
	{
		RDS_S_ASSERT(This::s_kColorCount <= VertexTypeUtil::s_kColorCount, "");
		Base::_internal_onRegister(out);
		out->addElement(Semantic::COLOR0, &This::colors);
	}
};

template<class UV_TYPE, size_t UV_COUNT, class BASE>
struct VertexT_Uv : public BASE
{
public:
	using Base = BASE;
	using This = VertexT_Uv<UV_TYPE, UV_COUNT, BASE>;

	using typename Base::Semantic;
	using typename Base::SizeType;

	using UvType = UV_TYPE;

public:
	static constexpr RenderDataType s_kUvType	= RenderDataTypeUtil::get<UvType>();
	static constexpr size_t			s_kUvCount	= UV_COUNT;

	static constexpr VertexType		s_kVertexType = VertexTypeUtil::addUv(Base::s_kVertexType, s_kUvType, s_kUvCount);

public:
	UvType uvs[s_kUvCount];

public:
	static const VertexLayout* vertexLayout()
	{
		static const VertexLayout* p = VertexLayoutManager::instance()->get(s_kVertexType);
		return p;
	}

	static void _internal_onRegister(VertexLayout* out)
	{
		RDS_S_ASSERT(This::s_kUvCount <= VertexTypeUtil::s_kUvCount, "");
		Base::_internal_onRegister(out);
		out->addElement(Semantic::TEXCOORD0, &This::uvs);
	}
};

template<class NORMAL_TYPE, size_t NORMAL_COUNT, class BASE>
struct VertexT_Normal : public BASE
{
public:
	using Base = BASE;
	using This = VertexT_Normal<NORMAL_TYPE, NORMAL_COUNT, BASE>;

	using typename Base::Semantic;
	using typename Base::SizeType;

	using NormalType = NORMAL_TYPE;

public:
	static constexpr RenderDataType s_kNormalType	= RenderDataTypeUtil::get<NormalType>();
	static constexpr size_t			s_kNormalCount	= NORMAL_COUNT;

	static constexpr VertexType		s_kVertexType = VertexTypeUtil::addNormal(Base::s_kVertexType, s_kNormalType, s_kNormalCount);

public:
	NormalType normals[s_kNormalCount];

public:
	static const VertexLayout* vertexLayout()
	{
		static const VertexLayout* p = VertexLayoutManager::instance()->get(s_kVertexType);
		return p;
	}

	static void _internal_onRegister(VertexLayout* out)
	{
		RDS_S_ASSERT(This::s_kNormalCount <= VertexTypeUtil::s_kNormalCount, "");
		Base::_internal_onRegister(out);
		out->addElement(Semantic::NORMAL0, &This::normals);
	}
};

template<class TANGENT_TYPE, size_t TANGENT_COUNT, class BASE>
struct VertexT_Tangent : public BASE
{
public:
	using Base = BASE;
	using This = VertexT_Tangent<TANGENT_TYPE, TANGENT_COUNT, BASE>;
	
	using typename Base::Semantic;
	using typename Base::SizeType;

	using TangentType = TANGENT_TYPE;
	using typename Base::NormalType;

public:
	static constexpr RenderDataType s_kTangentType	= RenderDataTypeUtil::get<TangentType>();
	static constexpr size_t			s_kTangentCount	= TANGENT_COUNT;

	static constexpr VertexType		s_kVertexType = VertexTypeUtil::addTangent(Base::s_kVertexType, s_kTangentCount);

public:
	TangentType tangents[s_kTangentCount];

public:
	static const VertexLayout* vertexLayout()
	{
		static const VertexLayout* p = VertexLayoutManager::instance()->get(s_kVertexType);
		return p;
	}

	static void _internal_onRegister(VertexLayout* out)
	{
		static_assert(IsSame<TangentType, NormalType>, "TangentType != NormalType");
		RDS_S_ASSERT(This::s_kTangentCount <= VertexTypeUtil::s_kTangentCount, "");
		Base::_internal_onRegister(out);
		out->addElement(Semantic::TANGENT0, &This::tangents);
	}
};

template<class BINROMAL_TYPE, size_t BINROMAL_COUNT, class BASE>
struct VertexT_Binormal : public BASE
{
public:
	using Base = BASE;
	using This = VertexT_Binormal<BINROMAL_TYPE, BINROMAL_COUNT, BASE>;

	using typename Base::Semantic;
	using typename Base::SizeType;

	using BinormalType = BINROMAL_TYPE;
	using typename Base::NormalType;

public:
	static constexpr RenderDataType s_kBinormalType		= RenderDataTypeUtil::get<BinormalType>();
	static constexpr size_t			s_kBinormalCount	= BINROMAL_COUNT;

	static constexpr VertexType		s_kVertexType = VertexTypeUtil::addBinormal(Base::s_kVertexType, s_kBinormalCount);

public:
	BinormalType binormals[s_kBinormalCount];

public:
	static const VertexLayout* vertexLayout()
	{
		static const VertexLayout* p = VertexLayoutManager::instance()->get(s_kVertexType);
		return p;
	}

	static void _internal_onRegister(VertexLayout* out)
	{
		static_assert(IsSame<BinormalType, NormalType>, "BinormalType != NormalType");
		RDS_S_ASSERT(This::s_kBinormalCount <= VertexTypeUtil::s_kBinormalCount, "");
		Base::_internal_onRegister(out);
		out->addElement(Semantic::BINORMAL0, &This::binormals);
	}
};

using DefaultPosType		= Tuple3f;
using DefaultColorType		= Color4b;
using DefaultUvType			= Tuple2f;
using DefaultNormalType		= Tuple3f;
using DefaultTangentType	= DefaultNormalType;
using DefaultBinormalType	= DefaultNormalType;

using Vertex_Pos2f	= VertexT_Pos<Tuple2f>;
using Vertex_Pos	= VertexT_Pos<DefaultPosType>;
using Vertex_ImGui	= VertexT_Color<Color4b, 1, VertexT_Uv<Tuple2f, 1, Vertex_Pos2f> >;

template<size_t UV_COUNT> using Vectex_Pos2fColor4fUv = VertexT_Uv<DefaultUvType, UV_COUNT, VertexT_Color<Color4f, 1, Vertex_Pos2f> >;
template<size_t UV_COUNT> using Vectex_Pos2fUvColor4f = VertexT_Color<Color4f, 1, VertexT_Uv<DefaultUvType, UV_COUNT, Vertex_Pos2f> >;


template<class UV_TYPE, size_t UV_COUNT, size_t COLOR_COUNT = 1>	using VertexT_Pos2fColorUv	= VertexT_Color<DefaultUvType, 1, VertexT_Uv<Tuple2f, UV_COUNT, Vertex_Pos2f> >;
template<size_t UV_COUNT, size_t COLOR_COUNT = 1>					using Vertex_Pos2fColorUv	= VertexT_Pos2fColorUv<DefaultUvType, UV_COUNT, COLOR_COUNT>;

template<class COLOR_TYPE, size_t COLOR_COUNT>		using VertexT_PosColor	= VertexT_Color<COLOR_TYPE, COLOR_COUNT, Vertex_Pos>;
template<size_t COLOR_COUNT>						using Vertex_PosColor	= VertexT_PosColor<DefaultColorType, COLOR_COUNT>;

template<class UV_TYPE, size_t UV_COUNT>			using VertexT_PosUv		= VertexT_Uv<UV_TYPE, UV_COUNT, Vertex_Pos>;
template<size_t UV_COUNT>							using Vertex_PosUv		= VertexT_PosUv<DefaultUvType, UV_COUNT>;

template<class NORMAL_TYPE, size_t NORMAL_COUNT>	using VertexT_PosNormal	= VertexT_Normal<NORMAL_TYPE, NORMAL_COUNT, Vertex_Pos>;
template<size_t NORMAL_COUNT>						using Vertex_PosNormal	= VertexT_PosNormal<DefaultNormalType, NORMAL_COUNT>;

template<class UV_TYPE, size_t UV_COUNT, size_t COLOR_COUNT = 1>	using VertexT_PosColorUv	= VertexT_Uv<UV_TYPE, UV_COUNT, Vertex_PosColor<COLOR_COUNT> >;
template<size_t UV_COUNT, size_t COLOR_COUNT = 1>					using Vertex_PosColorUv		= VertexT_Uv<DefaultUvType, UV_COUNT, Vertex_PosColor<COLOR_COUNT> >;

template<class NORMAL_TYPE, size_t NORMAL_COUNT, size_t UV_COUNT, size_t COLOR_COUNT>	using VertexT_PosColorUvNormal	= VertexT_Normal<NORMAL_TYPE,		NORMAL_COUNT, Vertex_PosColorUv<UV_COUNT, COLOR_COUNT> >;
template<size_t UV_COUNT, size_t NORMAL_COUNT = 1, size_t COLOR_COUNT = 1>				using Vertex_PosColorUvNormal	= VertexT_Normal<DefaultNormalType, NORMAL_COUNT, Vertex_PosColorUv<UV_COUNT, COLOR_COUNT> >;

template<class NORMAL_TYPE, size_t TANGENT_COUNT, size_t NORMAL_COUNT, size_t UV_COUNT, size_t COLOR_COUNT>	using VertexT_PosColorUvNT	= VertexT_Tangent<NORMAL_TYPE,		  TANGENT_COUNT, Vertex_PosColorUvNormal<UV_COUNT, NORMAL_COUNT, COLOR_COUNT> >;
template<size_t UV_COUNT, size_t TANGENT_COUNT = 1, size_t NORMAL_COUNT = 1, size_t COLOR_COUNT = 1>		using Vertex_PosColorUvNT	= VertexT_Tangent<DefaultTangentType, TANGENT_COUNT, Vertex_PosColorUvNormal<UV_COUNT, NORMAL_COUNT, COLOR_COUNT> >;

template<class NORMAL_TYPE, size_t BINORMAL_COUNT, size_t TANGENT_COUNT, size_t NORMAL_COUNT, size_t UV_COUNT, size_t COLOR_COUNT>	using VertexT_PosColorUvNTB	= VertexT_Binormal<NORMAL_TYPE,			BINORMAL_COUNT, Vertex_PosColorUvNT<UV_COUNT, TANGENT_COUNT, NORMAL_COUNT, COLOR_COUNT> >;
template<size_t UV_COUNT, size_t BINORMAL_COUNT = 1, size_t TANGENT_COUNT = 1, size_t NORMAL_COUNT = 1, size_t COLOR_COUNT = 1>		using Vertex_PosColorUvNTB	= VertexT_Binormal<DefaultBinormalType,	BINORMAL_COUNT, Vertex_PosColorUvNT<UV_COUNT, TANGENT_COUNT, NORMAL_COUNT, COLOR_COUNT> >;

template<class NORMAL_TYPE, size_t NORMAL_COUNT, size_t UV_COUNT>	using VertexT_PosUvNormal	= VertexT_Normal<NORMAL_TYPE,		NORMAL_COUNT, Vertex_PosUv<UV_COUNT> >;
template<size_t UV_COUNT, size_t NORMAL_COUNT = 1>					using Vertex_PosUvNormal	= VertexT_Normal<DefaultNormalType, NORMAL_COUNT, Vertex_PosUv<UV_COUNT> >;



#endif


}