#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rds_render_api_layer/vertex/rdsVertex.h"

namespace rds
{

#define ShaderStageFlag_ENUM_LIST(E) \
	E(None, = 0) \
	E(Vertex,	= BitUtil::bit(0)) \
	E(Pxiel,	= BitUtil::bit(1)) \
	E(Compute,	= BitUtil::bit(2)) \
	E(_kCount,	= 3) \
//---
RDS_ENUM_CLASS(ShaderStageFlag, u8);
RDS_ENUM_ALL_OPERATOR(ShaderStageFlag);

#define ShaderPropType_ENUM_LIST(E) \
	E(None, = 0) \
	E(Int,) \
	E(Float,) \
	E(Vec2f,) \
	E(Vec3f,) \
	E(Vec4f,) \
	E(Color4f,) \
	E(Texture2D,) \
	E(_kCount,) \
//---
RDS_ENUM_CLASS(ShaderPropType, u8);

#if 0
#pragma mark --- rdsShaderInfo-Decl ---
#endif // 0
#if 1

struct ShaderPropInfo
{
	ShaderPropType	type	= ShaderPropType::None;;
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

public:
	static constexpr SizeType s_kInputLocalSize			= 8;
	static constexpr SizeType s_kOutputLocalSize		= 4;
	static constexpr SizeType s_kConstBufLocalSize		= 3;
	static constexpr SizeType s_kTextureLocalSize		= 3;
	static constexpr SizeType s_kSamplerLocalSize		= 3;
	static constexpr SizeType s_kStorageBufLocalSize	= 3;
	static constexpr SizeType s_kStorageImageLocalSize	= 3;

public:
	void load(StrView filename_)
	{
		filename = filename_;
		JsonUtil::readFile(filename, *this);
	}

public:
	// Info Type
	#if 1

	struct Input
	{
		String			name;
		RenderDataType	dataType;
		VertexSemantic	semantic;

	public:
		template<class JSON_SE>
		void onJsonIo(JSON_SE& se)
		{
			RDS_NAMED_FIXED_IO(se, name);
			RDS_NAMED_FIXED_IO(se, dataType);
			RDS_NAMED_FIXED_IO(se, semantic);
		}
	};

	struct Output
	{
		String			name;
		RenderDataType	dataType;
		VertexSemantic	semantic;

	public:
		template<class JSON_SE>
		void onJsonIo(JSON_SE& se)
		{
			RDS_NAMED_FIXED_IO(se, name);
			RDS_NAMED_FIXED_IO(se, dataType);
			RDS_NAMED_FIXED_IO(se, semantic);
		}
	};

	struct ConstBuffer
	{
		String				name;
		u16					bindPoint	= 0;
		u32					size		= 0;
		Vector<Variable, 8> variables;

	public:
		template<class JSON_SE>
		void onJsonIo(JSON_SE& se)
		{
			RDS_NAMED_FIXED_IO(se, name);
			RDS_NAMED_FIXED_IO(se, bindPoint);
			RDS_NAMED_FIXED_IO(se, size);
			RDS_NAMED_FIXED_IO(se, variables);
		}
	};

	struct Texture
	{
		String	name;
		u16		bindPoint = 0;
		u16		arraySize = 0;

	public:
		template<class JSON_SE>
		void onJsonIo(JSON_SE& se)
		{
			RDS_NAMED_FIXED_IO(se, name);
			RDS_NAMED_FIXED_IO(se, bindPoint);
		}
	};

	struct Sampler
	{
		String	name;
		u16		bindPoint = 0;
		u16		arraySize = 0;

	public:
		template<class JSON_SE>
		void onJsonIo(JSON_SE& se)
		{
			RDS_NAMED_FIXED_IO(se, name);
			RDS_NAMED_FIXED_IO(se, bindPoint);
		}
	};

	struct StorageBuffer
	{
	public:
		template<class JSON_SE>
		void onJsonIo(JSON_SE& se)
		{
			
		}
	};

	struct StorageImage
	{
	public:
		template<class JSON_SE>
		void onJsonIo(JSON_SE& se)
		{

		}
	};

	#endif // 1

public:
	String			filename;
	ShaderStageFlag stageFlag;

	Vector<Input,			s_kInputLocalSize>			inputs;
	Vector<Output,			s_kOutputLocalSize>			outputs;
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
	// RenderState

	String name;

	String queue;

	String vsFunc;
	String psFunc;

public:
	template<class JSON_SE>
	void onJsonIo(JSON_SE& se)
	{
		RDS_NAMED_FIXED_IO(se, name);
		RDS_NAMED_FIXED_IO(se, queue);
		RDS_NAMED_FIXED_IO(se, vsFunc);
		RDS_NAMED_FIXED_IO(se, psFunc);
	}
};


struct ShaderInfo : public NonCopyable
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Prop		= ShaderPropInfo;
	using Pass		= ShaderPassInfo;
	using Stage		= ShaderStageInfo;

public:
	static constexpr SizeType s_kShaderStageCount = enumInt(ShaderStageFlag::_kCount);
	static constexpr SizeType s_kLocalPassSize		= 2;
	static constexpr SizeType s_kLocalPropSize	= 12;

public:
	Vector<Prop, s_kLocalPropSize> props;
	Vector<Pass, s_kLocalPassSize> passes;

public:
	void clear()
	{
		props.clear();
		passes.clear();
	}

public:
	template<class JSON_SE>
	void onJsonIo(JSON_SE& se)
	{
		RDS_NAMED_FIXED_IO(se, props);
		RDS_NAMED_FIXED_IO(se, passes);
		//RDS_NAMED_FIXED_IO(se, stages);
	}
};


#endif

}
