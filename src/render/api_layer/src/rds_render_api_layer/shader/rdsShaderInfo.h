#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rds_render_api_layer/vertex/rdsVertex.h"
#include "rds_render_api_layer/shader/rdsRenderState.h"
#include "rds_render_api_layer/shader/rdsShaderInterop.h"
#include "rdsShaderPermutations.h"

namespace rds
{
using ShaderParamId = int;

class Texture2D;

#define ShaderPropType_ENUM_LIST(E) \
	E(None, = 0) \
	E(Bool,) \
	E(Int,) \
	E(UInt64,) \
	E(Float,) \
	E(Vec2f,) \
	E(Vec3f,) \
	E(Vec4f,) \
	E(Color4f,) \
	E(Texture2D,) \
	E(_kCount,) \
//---
RDS_ENUM_CLASS(ShaderPropType, u8);


struct ShaderPropTypeUtil
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	ShaderPropTypeUtil() = delete;

public:
	using Type = ShaderPropType;

public:
	template<class T> static constexpr ShaderPropType get();

	static constexpr RenderDataType toRenderDataType(Type v)
	{
		using SRC = Type;
		switch (v)
		{
			case SRC::Int:			{ return RenderDataType::Int32; }		break;
			case SRC::Float:		{ return RenderDataType::Float32; }		break;
			case SRC::Vec2f:		{ return RenderDataType::Float32x2; }	break;
			case SRC::Vec3f:		{ return RenderDataType::Float32x3; }	break;
			case SRC::Vec4f:		{ return RenderDataType::Float32x4; }	break;
			case SRC::Color4f:		{ return RenderDataType::Float32x4; }	break;
			case SRC::Texture2D:	{ return RenderDataType::Texture2D; }	break;
			default: { RDS_THROW(); break; }
		}
	}
};

template<> inline constexpr ShaderPropType ShaderPropTypeUtil::get<bool>()		{ return Type::Bool; }
template<> inline constexpr ShaderPropType ShaderPropTypeUtil::get<i32>()		{ return Type::Int; }
template<> inline constexpr ShaderPropType ShaderPropTypeUtil::get<u64>()		{ return Type::UInt64; }
template<> inline constexpr ShaderPropType ShaderPropTypeUtil::get<float>()		{ return Type::Float; }
template<> inline constexpr ShaderPropType ShaderPropTypeUtil::get<Vec2f>()		{ return Type::Vec2f; }
template<> inline constexpr ShaderPropType ShaderPropTypeUtil::get<Vec3f>()		{ return Type::Vec3f; }
template<> inline constexpr ShaderPropType ShaderPropTypeUtil::get<Vec4f>()		{ return Type::Vec4f; }
template<> inline constexpr ShaderPropType ShaderPropTypeUtil::get<Tuple2f>()	{ return Type::Vec2f; }
template<> inline constexpr ShaderPropType ShaderPropTypeUtil::get<Tuple3f>()	{ return Type::Vec3f; }
template<> inline constexpr ShaderPropType ShaderPropTypeUtil::get<Tuple4f>()	{ return Type::Vec4f; }
template<> inline constexpr ShaderPropType ShaderPropTypeUtil::get<Color4f>()	{ return Type::Color4f; }
template<> inline constexpr ShaderPropType ShaderPropTypeUtil::get<Texture2D>()	{ return Type::Texture2D; }

#define ShaderResourceType_ENUM_LIST(E) \
	E(None, = 0) \
	E(ConstantBuffer,) \
	E(Texture,) \
	E(Sampler,) \
	E(StorageBuffer,) \
	E(StorageImage,) \
	E(_kCount,) \
//---
RDS_ENUM_CLASS(ShaderResourceType, u8);

struct ShaderPropValueConstPtr
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	ShaderPropValueConstPtr() = default;

	template<class T>
	ShaderPropValueConstPtr(const T& v)
	{
		type = ShaderPropTypeUtil::get<T>();
		data = &v;
		size = sizeof(T);
	}

public:
	ShaderPropType	type = ShaderPropType::None;
	const void*		data = nullptr;
	u32				size = 0;
};

#if 0
#pragma mark --- rdsShaderInfo-Decl ---
#endif // 0
#if 1

struct ShaderPropInfo
{
	ShaderPropType	type	= ShaderPropType::None;
	String			name;
	String			displayName;
	String			defaultValue;

public:
	template<class JSON_SE>
	void onJsonIo(JSON_SE& se)
	{
		RDS_NAMED_FIXED_IO(se, type);
		RDS_NAMED_FIXED_IO(se, name);
		RDS_NAMED_FIXED_IO(se, displayName);		
		RDS_NAMED_FIXED_IO(se, defaultValue);
	}
};

struct ShaderVariableInfo
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	RenderDataType	dataType = RenderDataType::None;
	u32				offset;
	u32				size;
	String			name;
	bool			isRowMajor = false;

public:
	u32 count() const { return size / sCast<u32>(RenderDataTypeUtil::getByteSize(dataType)); }

public:
	template<class JSON_SE>
	void onJsonIo(JSON_SE& se)
	{
		RDS_NAMED_FIXED_IO(se, dataType);
		RDS_NAMED_FIXED_IO(se, offset);
		RDS_NAMED_FIXED_IO(se, name);
		RDS_NAMED_FIXED_IO(se, isRowMajor);
	}
};

struct ShaderStageInfo
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Variable = ShaderVariableInfo;
	using DataType = RenderDataType;
	using Variables = Vector<Variable, 8>;

public:
	static constexpr SizeType s_kInputLocalSize			= 8;
	static constexpr SizeType s_kOutputLocalSize		= 4;
	static constexpr SizeType s_kConstBufLocalSize		= 3;
	static constexpr SizeType s_kTextureLocalSize		= 3;
	static constexpr SizeType s_kSamplerLocalSize		= 3;
	static constexpr SizeType s_kStorageBufLocalSize	= 3;
	static constexpr SizeType s_kStorageImageLocalSize	= 3;

public:

	void create(StrView filename_, bool isLoadDefaultPushConst)
	{
		destroy();

		filename = filename_;
		JsonUtil::readFile(filename, *this);

		if (isLoadDefaultPushConst)
		{
			createDefaultPushConstant();
		}
	}

	void createDefaultPushConstant()
	{
		// hardcode currently
		auto& pushConst = pushConstants.emplace_back();
		pushConst.name		= "rds_perObjectParam";		
		pushConst.offset	= 0;
		pushConst.size		= sizeof(PerObjectParam);
	}

	void destroy()
	{
		filename.clear();
		stageFlag = ShaderStageFlag::None;

		inputs.clear();
		outputs.clear();
		pushConstants.clear();
		constBufs.clear();
		textures.clear();
		samplers.clear();
		storageBufs.clear();
		storageImages.clear();
	}

	SizeType bindingCount() const
	{
		SizeType out = constBufs.size() + textures.size() + samplers.size() 
			+ storageBufs.size() + storageImages.size();
		return out;
	}

	SizeType bindingCountCombinedImage() const
	{
		SizeType out = textures.size();
		return out;
	}

	SizeType renderTargetCount() const
	{
		RDS_CORE_ASSERT(BitUtil::hasOnly(stageFlag, ShaderStageFlag::Pixel), "only pixel shader has render targets");
		return outputs.size();
	}

public:
	// Info Type
	#if 1

	struct ShaderResource
	{
	public:
		String			name;

	public:
		template<class JSON_SE>
		void onJsonIo(JSON_SE& se)
		{
			RDS_NAMED_FIXED_IO(se, name);
		}
	};

	struct Input : public ShaderResource
	{
	public:
		using Base = ShaderResource;

	public:
		RenderDataType	dataType;
		VertexSemantic	semantic;

	public:
		template<class JSON_SE>
		void onJsonIo(JSON_SE& se)
		{
			Base::onJsonIo(se);
			RDS_NAMED_FIXED_IO(se, dataType);
			RDS_NAMED_FIXED_IO(se, semantic);
		}
	};

	struct Output : public ShaderResource
	{
	public:
		using Base = ShaderResource;

	public:
		RenderDataType	dataType;
		VertexSemantic	semantic;

	public:
		template<class JSON_SE>
		void onJsonIo(JSON_SE& se)
		{
			Base::onJsonIo(se);
			RDS_NAMED_FIXED_IO(se, dataType);
			RDS_NAMED_FIXED_IO(se, semantic);
		}
	};

	struct PushConstant : public ShaderResource
	{
	public:
		using Base = ShaderResource;

	public:
		u32			offset		= 0;
		u32			size		= 0;
		Variables	variables;

	public:
		template<class JSON_SE>
		void onJsonIo(JSON_SE& se)
		{
			Base::onJsonIo(se);
			RDS_NAMED_FIXED_IO(se, offset);
			RDS_NAMED_FIXED_IO(se, size);
			RDS_NAMED_FIXED_IO(se, variables);
		}
	};

	struct ShaderResourceParam : public ShaderResource
	{
	public:
		using Base = ShaderResource;

	public:
		u16		bindSet		= 0;
		u16		bindPoint	= 0;
		u16		bindCount	= 0;
		//bool	isBindless	: 1;

	public:
		ShaderResourceParam()
			//: isBindless(false)
		{

		}

		template<class JSON_SE>
		void onJsonIo(JSON_SE& se)
		{
			Base::onJsonIo(se);
			RDS_NAMED_FIXED_IO(se, bindSet);
			RDS_NAMED_FIXED_IO(se, bindPoint);
			RDS_NAMED_FIXED_IO(se, bindCount);
			//RDS_NAMED_FIXED_IO(se, isBindless);
		}

		bool isBindlessResource() const
		{
			return true; // isBindless;
		}
	};

	struct ConstBuffer : public ShaderResourceParam
	{
	public:
		using Base = ShaderResourceParam;

	public:
		u32			size		= 0;
		Variables	variables;

	public:
		static constexpr ShaderResourceType paramType() { return ShaderResourceType::ConstantBuffer; }

	public:
		const Variable* findVariable(StrView name_) const
		{
			for (const auto& var : variables)
			{
				if (var.name == name_)
				{
					return &var;
				}
			}
			return nullptr;
		}

	public:
		template<class JSON_SE>
		void onJsonIo(JSON_SE& se)
		{
			Base::onJsonIo(se);
			
			RDS_NAMED_FIXED_IO(se, size);
			RDS_NAMED_FIXED_IO(se, variables);
		}
	};

	struct Texture : public ShaderResourceParam
	{
	public:
		using Base = ShaderResourceParam;

	public:
		DataType	dataType	= DataType::None;

	public:
		static constexpr ShaderResourceType paramType() { return ShaderResourceType::Texture; }

	public:
		template<class JSON_SE>
		void onJsonIo(JSON_SE& se)
		{
			Base::onJsonIo(se);
			RDS_NAMED_FIXED_IO(se, dataType);
		}
	};

	struct Sampler : public ShaderResourceParam
	{
	public:
		using Base = ShaderResourceParam;

	public:
		static constexpr ShaderResourceType paramType() { return ShaderResourceType::Sampler; }

	public:

	public:
		template<class JSON_SE>
		void onJsonIo(JSON_SE& se)
		{
			Base::onJsonIo(se);
		}
	};

	struct StorageBuffer : public ShaderResourceParam
	{
	public:
		using Base = ShaderResourceParam;

	public:
		static constexpr ShaderResourceType paramType() { return ShaderResourceType::StorageBuffer; }

	public:

	public:
		template<class JSON_SE>
		void onJsonIo(JSON_SE& se)
		{
			Base::onJsonIo(se);
		}

		/*bool isBindlessResource() const
		{
			return true;
		}*/
	};

	struct StorageImage : public ShaderResourceParam
	{
	public:
		using Base = ShaderResourceParam;

	public:
		static constexpr ShaderResourceType paramType() { return ShaderResourceType::StorageImage; }

	public:

	public:
		template<class JSON_SE>
		void onJsonIo(JSON_SE& se)
		{
			Base::onJsonIo(se);
		}
	};

	#endif // 1

public:
	String			filename;
	ShaderStageFlag stageFlag;

	Vector<Input,			s_kInputLocalSize>			inputs;
	Vector<Output,			s_kOutputLocalSize>			outputs;
	Vector<PushConstant,	s_kConstBufLocalSize>		pushConstants;
	Vector<ConstBuffer,		s_kConstBufLocalSize>		constBufs;
	Vector<Texture,			s_kTextureLocalSize>		textures;
	Vector<Sampler,			s_kSamplerLocalSize>		samplers;
	Vector<StorageBuffer,	s_kStorageBufLocalSize>		storageBufs;
	Vector<StorageImage,	s_kStorageImageLocalSize>	storageImages;

public:
	template<class JSON_SE>
	void onJsonIo(JSON_SE& se)
	{
		RDS_NAMED_FIXED_IO(se, stageFlag);

		RDS_NAMED_FIXED_IO(se, inputs);
		RDS_NAMED_FIXED_IO(se, outputs);
		RDS_NAMED_FIXED_IO(se, pushConstants);
		RDS_NAMED_FIXED_IO(se, constBufs);
		RDS_NAMED_FIXED_IO(se, textures);
		RDS_NAMED_FIXED_IO(se, samplers);
		RDS_NAMED_FIXED_IO(se, storageBufs);
		RDS_NAMED_FIXED_IO(se, storageImages);
	}
};

struct ShaderPassInfo
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:

	String name;

	String queue;

	String vsFunc;
	String psFunc;
	String csFunc;

	RenderState renderState;

	// --- no need to serialize
	ShaderStageInfo	allStageUnionInfo;

public:
	template<class JSON_SE>
	void onJsonIo(JSON_SE& se)
	{
		RDS_NAMED_FIXED_IO(se, name);
		RDS_NAMED_FIXED_IO(se, queue);
		RDS_NAMED_FIXED_IO(se, vsFunc);
		RDS_NAMED_FIXED_IO(se, psFunc);
		RDS_NAMED_FIXED_IO(se, csFunc);
		RDS_NAMED_FIXED_IO(se, renderState);
	}
};

struct ShaderInfo : public NonCopyable
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Prop		= ShaderPropInfo;
	using Pass		= ShaderPassInfo;
	using Stage		= ShaderStageInfo;
	using Permut	= ShaderPermutationInfo;

public:
	static constexpr SizeType s_kShaderStageCount	= enumInt(ShaderStageFlag::_kCount);
	static constexpr SizeType s_kLocalPassSize		= 2;
	static constexpr SizeType s_kLocalPropSize		= 12;
	static constexpr SizeType s_kLocalPermutSize	= ShaderPermutations::s_kLocalPermutSize;

public:
	using Props		= Vector<Prop, s_kLocalPropSize>;
	using Passes	= Vector<Pass, s_kLocalPassSize>;
	using Permuts	= ShaderPermutations::Info;

public:
	Props	props;
	Passes	passes;
	Permuts	permuts;

public:
	void clear();

	bool isPermutationValid(	StrView name, StrView value)	const;
	int  findPermutationNameIdx(StrView name)					const;

public:
	template<class JSON_SE>
	void onJsonIo(JSON_SE& se)
	{
		RDS_NAMED_FIXED_IO(se, props);
		RDS_NAMED_FIXED_IO(se, passes);
		RDS_NAMED_FIXED_IO(se, permuts);
	}
};


#endif

}
