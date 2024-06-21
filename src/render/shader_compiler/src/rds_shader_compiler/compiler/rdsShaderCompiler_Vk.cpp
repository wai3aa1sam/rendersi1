#include "rds_shader_compiler-pch.h"
#include "rdsShaderCompiler_Vk.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

//#if RDS_RENDER_HAS_VULKAN


namespace rds
{

using namespace spirv_cross;

#if 0
#pragma mark --- rdsShaderCompiler_Vk-Decl ---
#endif // 0
#if 1

struct SpirvUtil
{
	using SpirvCompiler	= ShaderCompiler_Vk::SpirvCompiler;

	static StrView			toStr(ShaderStageFlag flag);
	static StrView			toStr(SPIRType::BaseType baseType);
	static RenderDataType	toRenderDataType(SPIRType type);
	static VertexSemantic	toVertexSemantic(const Resource& res, SpirvCompiler& compiler);
};

#endif


#if 0
#pragma mark --- rdsShaderCompiler_Vk-Impl ---
#endif // 0
#if 1

bool 
ShaderCompiler_Vk::onCompile(const CompileDescView& descView)
{
	auto			srcFilepath	= descView.srcFilepath;
	const auto&		opt			= descView.opt;
	ShaderStageFlag stage		= descView.stage;
	auto			entry		= descView.entry;
	const auto&		includes	= descView.compileDesc->includes;		RDS_UNUSED(includes);

	TempString binFilepath;
	descView.getBinFilepathTo(binFilepath);

	RDS_TODO("check vulkan extension whether existed then add the compile option, eg. -fhlsl-functionality1");
	RDS_TODO("cbuffer only for hlsl, use ubo for glsl, same for uav, use ssbo");

	if (opt.isCompileBinary)
	{
		u32 stageOffset = 0;
		if (BitUtil::has(stage, ShaderStageFlag::Pixel)) stageOffset = 0;

		//u32 uboStartIdx = 0; u32 texStartIdx = 4; u32 ssboStartIdx = 10; u32 imageStartIdx = 13; u32 samplerStartIdx = 16;
		// 16 is minimum spec in vulkan
		u32 cbufferOffset	= stageOffset + 0;
		u32 textureOffset	= stageOffset + 4;
		u32 samplerOffset	= stageOffset + 8;
		u32 uavOffset		= stageOffset + 12;
		u32 imageOffset		= stageOffset + 14;

		TempString args;
		fmtTo(args, "glslc -x hlsl -fshader-stage={} -fentry-point={} -c \"{}\" -o \"{}\" -fhlsl-functionality1 -fhlsl-iomap", SpirvUtil::toStr(stage), entry, srcFilepath, binFilepath);
		fmtTo(args, " --target-env=vulkan1.2 --target-spv=spv1.5");	//  --target-spv=spv1.5
		fmtTo(args, " -fauto-bind-uniforms");	// auto bind all uniform variable
		if (!opt.isNoOffset)
		{
			fmtTo(args, " -fcbuffer-binding-base {} -ftexture-binding-base {} -fsampler-binding-base {} -fuav-binding-base {} -fimage-binding-base {}"
				, cbufferOffset, textureOffset, samplerOffset, uavOffset, imageOffset);
		}

		//fmtTo(args, " -I\"dir"");

		//fmtTo(args, " -fauto-bind-uniforms -fubo-binding-base 100 -fresource-set-binding b1 1 2");
		//fmtTo(args, " -fresource-set-binding b1 1 2");

		{
			CmdLine cmdl;
			cmdl.execute(args);
		}
	}

	if (!Path::isExist(binFilepath))
		return false;

	if (opt.isReflect)
	{
		Vector<u8> spvBin;
		File::readFile(binFilepath, spvBin);

		reflect(binFilepath, spvBin, stage);
	}

	log("outputed to {}", binFilepath);

	return true;
}

void 
ShaderCompiler_Vk::reflect(StrView outpath, ByteSpan spvBytes, ShaderStageFlag stage)
{
	Span<const u32>	u32Spv		= spanCast<const u32>(spvBytes);
	SpirvCompiler	compiler	= { u32Spv.data(), u32Spv.size() };

	auto active			= compiler.get_active_interface_variables();
	ShaderResources rsc = compiler.get_shader_resources(active);
	compiler.set_enabled_interface_variables(rds::move(active));

	ShaderStageInfo outInfo;
	outInfo.stageFlag = stage;

	_reflect_inputs			(outInfo, compiler, rsc);
	_reflect_outputs		(outInfo, compiler, rsc);
	_reflect_constBufs		(outInfo, compiler, rsc);
	//_reflect_pushConstants	(outInfo, compiler, rsc);
	_reflect_textures		(outInfo, compiler, rsc);
	_reflect_samplers		(outInfo, compiler, rsc);
	_reflect_storageBufs	(outInfo, compiler, rsc);
	_reflect_storageImages	(outInfo, compiler, rsc);

	_reflect_threadGroups	(outInfo, compiler);

	writeStageInfo(outpath, outInfo);
}

void 
ShaderCompiler_Vk::_reflect_inputs(ShaderStageInfo& outInfo, SpirvCompiler& compiler, const ShaderResources& res)
{
	using Input = ShaderStageInfo::Input;

	log("--- Input");

	outInfo.inputs.reserve(res.stage_inputs.size());
	for (const Resource& e : res.stage_inputs)
	{
		Input& dst = outInfo.inputs.emplace_back();

		const SPIRType& type = compiler.get_type(e.base_type_id);
		const std::string& semanticName = compiler.get_decoration_string(e.id, spv::DecorationHlslSemanticGOOGLE);
		
		dst.name		= e.name.c_str();
		dst.semantic	= VertexSemanticUtil::parse(semanticName.c_str());
		dst.dataType	= SpirvUtil::toRenderDataType(type);

		log("\tname: {}, semantic: {}, dataType: {}", dst.name, enumStr(dst.semantic), enumStr(dst.dataType));
	}

	log("");
}

void 
ShaderCompiler_Vk::_reflect_outputs(ShaderStageInfo& outInfo, SpirvCompiler& compiler, const ShaderResources& res)
{
	log("--- Output");
	
	outInfo.outputs.reserve(res.stage_outputs.size());
	for (const Resource& e : res.stage_outputs)
	{
		auto& dst	= outInfo.outputs.emplace_back();

		const SPIRType&		type			= compiler.get_type(e.base_type_id);
		const std::string&	semanticName	= compiler.get_decoration_string(e.id, spv::DecorationHlslSemanticGOOGLE);
		
		dst.name		= e.name.c_str();
		dst.semantic	= VertexSemanticUtil::parse(semanticName.c_str());
		dst.dataType	= SpirvUtil::toRenderDataType(type);

		log("\tname: {}, semantic: {}, dataType: {}", dst.name, enumStr(dst.semantic), enumStr(dst.dataType));
	}

	log("");
}

void 
ShaderCompiler_Vk::_reflect_constBufs(ShaderStageInfo& outInfo, SpirvCompiler& compiler, const ShaderResources& res)
{
	using ConstBuffer	= ShaderStageInfo::ConstBuffer;
	using Variable		= ShaderStageInfo::Variable;

	log("--- ConstBuffer");

	outInfo.constBufs.reserve(res.uniform_buffers.size());
	for (const Resource& e : res.uniform_buffers)
	{
		ConstBuffer& dst = outInfo.constBufs.emplace_back();

		const SPIRType& type = compiler.get_type(e.base_type_id);

		size_t size = compiler.get_declared_struct_size(type);

		u32 set		= compiler.get_decoration(e.id, spv::DecorationDescriptorSet);	RDS_UNUSED(set);
		u32 binding	= compiler.get_decoration(e.id, spv::DecorationBinding);		RDS_UNUSED(binding);

		const std::string& name = compiler.get_name(e.id);

		dst.name		= name.c_str();
		dst.size		= sCast<u32>(size);
		dst.bindSet		= sCast<u16>(set);
		dst.bindPoint	= sCast<u16>(binding);
		dst.bindCount	= sCast<u16>(math::clamp(type.array.size(), sCast<size_t>(1), type.array.size()));

		log("\tname: {}, size: {}, set: {}, binding: {}", name, size, set, binding);

		bool isRowMajor = compiler.get_decoration(e.id, spv::DecorationRowMajor);
		_reflect_struct(outInfo, &dst.variables, compiler, type, isRowMajor);
		_appendStageUnionInfo_constBufs(_allStageUnionInfo, dst);
	}
	log("");
}

void 
ShaderCompiler_Vk::_reflect_pushConstants(ShaderStageInfo& outInfo, SpirvCompiler& compiler, const ShaderResources& res)
{
	using PushConstant	= ShaderStageInfo::PushConstant;
	using Variable		= ShaderStageInfo::Variable;

	log("--- Push Constant");

	outInfo.constBufs.reserve(res.push_constant_buffers.size());
	for (u32 i = 0; i < res.push_constant_buffers.size(); ++i)
	{
		const Resource& e = res.push_constant_buffers[i];

		PushConstant&	dst		= outInfo.pushConstants.emplace_back();
		const SPIRType& type	= compiler.get_type(e.base_type_id);

		const std::string& name = compiler.get_name(e.id);
		size_t size				= compiler.get_declared_struct_size(type);

		/*auto ranges = compiler.get_active_buffer_ranges(res.push_constant_buffers[i].id);
		for (auto& range : ranges)
		{
			range.index;
			range.offset;
			range.range;
		}*/

		dst.name	= name.c_str();
		dst.size	= sCast<u32>(size);
		dst.offset	= sCast<u32>(compiler.type_struct_member_offset(type, i));

		log("\tname: {}, size: {}", name, size);

		bool isRowMajor = compiler.get_decoration(e.id, spv::DecorationRowMajor);
		_reflect_struct(outInfo, &dst.variables, compiler, type, isRowMajor);
		_appendStageUnionInfo_pushConstants(_allStageUnionInfo, dst);
	}

	log("");
}

void 
ShaderCompiler_Vk::_reflect_textures(ShaderStageInfo& outInfo, SpirvCompiler& compiler, const ShaderResources& res)
{
	using Texture = ShaderStageInfo::Texture;
	outInfo.textures.reserve(res.sampled_images.size());

	log("--- Texture");
	
	for (const Resource& resource : res.separate_images)
	{
		Texture& dst = outInfo.textures.emplace_back();
		const SPIRType& type = compiler.get_type(resource.base_type_id);
		//size_t size = compiler.get_declared_struct_size(type);

		const std::string& name = compiler.get_name(resource.id);

		u32 set		= compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);	RDS_UNUSED(set);
		u32 binding	= compiler.get_decoration(resource.id, spv::DecorationBinding);			RDS_UNUSED(binding);
		
		dst.name		= name.c_str();
		dst.bindSet		= sCast<u16>(set);
		dst.bindPoint	= sCast<u16>(binding);
		dst.bindCount	= sCast<u16>(math::clamp(type.array.size(), sCast<size_t>(1), type.array.size()));

		switch (type.image.dim)
		{
			using SRC = spv::Dim;
			case SRC::Dim1D:	{ dst.dataType = RenderDataType::Texture1D;		} break;
			case SRC::Dim2D:	{ dst.dataType = RenderDataType::Texture2D;		} break;
			case SRC::Dim3D:	{ dst.dataType = RenderDataType::Texture3D;		} break;
			case SRC::DimCube:	{ dst.dataType = RenderDataType::TextureCube;	} break;

			default: { RDS_THROW("invalid texture dimension"); } break;
		}
		_appendStageUnionInfo_textures(_allStageUnionInfo, dst);
	}

	log("");
}

void 
ShaderCompiler_Vk::_reflect_samplers(ShaderStageInfo& outInfo, SpirvCompiler& compiler, const ShaderResources& res)
{
	using Sampler = ShaderStageInfo::Sampler;
	outInfo.samplers.reserve(res.separate_samplers.size());

	log("--- Sampler");

	for (const Resource& resource : res.separate_samplers)
	{
		Sampler& dst = outInfo.samplers.emplace_back();
		const SPIRType& type = compiler.get_type(resource.base_type_id);
		//size_t size = compiler.get_declared_struct_size(type);

		const std::string& name = compiler.get_name(resource.id);

		u32 set		= compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);	RDS_UNUSED(set);
		u32 binding	= compiler.get_decoration(resource.id, spv::DecorationBinding);			RDS_UNUSED(binding);

		dst.name		= name.c_str();
		dst.bindSet		= sCast<u16>(set);
		dst.bindPoint	= sCast<u16>(binding);
		dst.bindCount	= sCast<u16>(math::clamp(type.array.size(), sCast<size_t>(1), type.array.size()));

		log("Sampler name: {}, set: {}, binding: {}", name, set, binding);
		_appendStageUnionInfo_samplers(_allStageUnionInfo, dst);
	}

	log("");
}

void 
ShaderCompiler_Vk::_reflect_storageBufs	(ShaderStageInfo& outInfo, SpirvCompiler& compiler, const ShaderResources& res)
{
	using StorageBuffer = ShaderStageInfo::StorageBuffer;
	outInfo.storageBufs.reserve(res.storage_buffers.size());

	log("--- StorageBuffer");

	for (const Resource& resource : res.storage_buffers)
	{
				StorageBuffer&	dst		= outInfo.storageBufs.emplace_back();
		const	SPIRType&		type	= compiler.get_type(resource.base_type_id);

		#if 0
		size_t memberCount = type.member_types.size();
		for (u32 i = 0; i < memberCount; i++)
		{
			auto &member_type = compiler.get_type(type.member_types[i]);
			size_t member_size = compiler.get_declared_struct_member_size(type, i);

			// Get member offset within this struct.
			size_t offset = compiler.type_struct_member_offset(type, i);

			if (!member_type.array.empty())
			{
				// Get array stride, e.g. float4 foo[]; Will have array stride of 16 bytes.
				size_t array_stride = compiler.type_struct_member_array_stride(type, i);
				RDS_DUMP_VAR(array_stride);
			}

			if (member_type.columns > 1)
			{
				// Get bytes stride between columns (if column major), for float4x4 -> 16 bytes.
				size_t matrix_stride = compiler.type_struct_member_matrix_stride(type, i);
				RDS_DUMP_VAR(matrix_stride);
			}

			const std::string &name = compiler.get_member_name(type.self, i);
			RDS_DUMP_VAR(name, member_size, offset);
		}
		#endif // 0

		//size_t size = compiler.get_declared_struct_size(type);

		const std::string& name = compiler.get_name(resource.id);

		u32 set		= compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);	RDS_UNUSED(set);
		u32 binding	= compiler.get_decoration(resource.id, spv::DecorationBinding);			RDS_UNUSED(binding);

		dst.name		= name.c_str();
		dst.bindSet		= sCast<u16>(set);
		dst.bindPoint	= sCast<u16>(binding);
		dst.bindCount	= sCast<u16>(math::clamp(type.array.size(), sCast<size_t>(1), type.array.size()));

		log("StorageBuffer name: {}, set: {}, binding: {}", name, set, binding);
		_appendStageUnionInfo_storageBufs(_allStageUnionInfo, dst);
	}

	log("");
}

void 
ShaderCompiler_Vk::_reflect_storageImages	(ShaderStageInfo& outInfo, SpirvCompiler& compiler, const ShaderResources& res)
{
	using StorageImage = ShaderStageInfo::StorageImage;
	outInfo.storageBufs.reserve(res.storage_images.size());

	log("--- StorageImage");

	for (const Resource& resource : res.storage_images)
	{
		StorageImage& dst		= outInfo.storageImages.emplace_back();
		const SPIRType& type	= compiler.get_type(resource.base_type_id);
		//size_t size = compiler.get_declared_struct_size(type);

		const std::string& name = compiler.get_name(resource.id);

		u32 set		= compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);	RDS_UNUSED(set);
		u32 binding	= compiler.get_decoration(resource.id, spv::DecorationBinding);			RDS_UNUSED(binding);

		dst.name		= name.c_str();
		dst.bindSet		= sCast<u16>(set);
		dst.bindPoint	= sCast<u16>(binding);
		dst.bindCount	= sCast<u16>(math::clamp(type.array.size(), sCast<size_t>(1), type.array.size()));

		log("StorageImage name: {}, set: {}, binding: {}", name, set, binding);
		_appendStageUnionInfo_storageImages(_allStageUnionInfo, dst);
	}

	log("");
}

void 
ShaderCompiler_Vk::_reflect_threadGroups(ShaderStageInfo& outInfo, SpirvCompiler& compiler)
{
	if (!BitUtil::has(outInfo.stageFlag, ShaderStageFlag::Compute))
		return;

	u32 x = compiler.get_execution_mode_argument(spv::ExecutionMode::ExecutionModeLocalSize, 0);
	u32 y = compiler.get_execution_mode_argument(spv::ExecutionMode::ExecutionModeLocalSize, 1);
	u32 z = compiler.get_execution_mode_argument(spv::ExecutionMode::ExecutionModeLocalSize, 2);

	outInfo._specialConstant = Vec4i{Vec3i::s_cast(Vec3u{ x, y, z }), 0};
}

void 
ShaderCompiler_Vk::_reflect_struct(ShaderStageInfo& outInfo, ShaderStageInfo::Variables* outVariables, SpirvCompiler& compiler, const SPIRType& type, bool isRowMajor)
{
	using Variable		= ShaderStageInfo::Variable;

	size_t member_count = type.member_types.size();
	outVariables->reserve(outVariables->size() + member_count);
	for (u32 i = 0; i < member_count; i++)
	{
		Variable& dstVar = outVariables->emplace_back();

		const SPIRType&		memberType = compiler.get_type(type.member_types[i]);
		size_t				memberSize = compiler.get_declared_struct_member_size(type, i);
		const std::string&	memberName = compiler.get_member_name(type.self, i);

		// Get member offset within this struct.
		size_t offset = compiler.type_struct_member_offset(type, i);

		if (!memberType.array.empty())
		{
			// Get array stride, e.g. float4 foo[]; Will have array stride of 16 bytes.
			size_t arrayStride = compiler.type_struct_member_array_stride(type, i);
			bool hasPaddingWarning = memberSize % arrayStride != 0;
			if (hasPaddingWarning)
			{
				_log("array padding warning: variable: {}, dataType : {}, array stride: {}", memberName, SpirvUtil::toStr(memberType.basetype), arrayStride);
			}
		}

		if (memberType.columns > 1)
		{
			// Get bytes stride between columns (if column major), for float4x4 -> 16 bytes.
			//size_t matrixStride = compiler.type_struct_member_matrix_stride(type, i);
			dstVar.isRowMajor = isRowMajor;
		}

		if (memberType.basetype != SPIRType::BaseType::Struct)
		{
			dstVar.name		= memberName.c_str();
			dstVar.dataType = SpirvUtil::toRenderDataType(memberType);
			dstVar.size		= sCast<u32>(memberSize);
			dstVar.offset	= sCast<u32>(offset);
			log("\t\tmemberName: {}, offset: {}, membSize: {}", memberName, offset, memberSize);
		}
		else
		{
			//_reflect_struct(outInfo, outVariables, compiler, type, isRowMajor);
		}
	}
}


#endif


#if 0
#pragma mark --- rdsShaderCompiler_Vk-Impl ---
#endif // 0
#if 1

inline
StrView 
SpirvUtil::toStr(ShaderStageFlag flag)
{
	using SRC = ShaderStageFlag;
	switch (flag)
	{
		case SRC::Vertex:	{ return "vertex";		} break;
		case SRC::Pixel:	{ return "fragment";	} break;
		case SRC::Compute:	{ return "compute";		} break;
	}

	RDS_CORE_ASSERT(false, "{}", RDS_SRCLOC);
	return "";
}

inline
StrView
SpirvUtil::toStr(SPIRType::BaseType baseType)
{
	switch (baseType)
	{
		using SRC = SPIRType::BaseType;
		case SRC::Boolean:	{ return "Bool";		} break;
		case SRC::SByte:	{ return "SNorm8";		} break;
		case SRC::UByte:	{ return "UNorm8";		} break;
		case SRC::Short:	{ return "Int16";		} break;
		case SRC::UShort:	{ return "UInt16";		} break;
		case SRC::Int:		{ return "Int32";		} break;
		case SRC::Int64:	{ return "Int64";		} break;
		case SRC::UInt:		{ return "UInt32";		} break;
		case SRC::UInt64:	{ return "UInt64";		} break;
		case SRC::Half:		{ return "Float16";		} break;
		case SRC::Float:	{ return "Float32";		} break;
		case SRC::Double:	{ return "Float64";		} break;

		//case SRC::Struct:	{ RDS_THROW("not support struct"); } break;
		case SRC::Struct:	{ return "Struct"; } break;

		default:			{ RDS_THROW("unknow type"); }	break;
	}
}

inline 
RenderDataType 
SpirvUtil::toRenderDataType(SPIRType type)
{
	RenderDataType o;

	TempString dataType;
	dataType = toStr(type.basetype);

	bool isVec = type.columns == 1  && type.vecsize > 1;
	bool isMat = type.columns > 3 && type.vecsize > 1;
	if (isMat)
	{
		fmtTo(dataType, "_{}x{}", type.vecsize, type.vecsize);
	}
	else if (isVec)
	{
		fmtTo(dataType, "x{}", type.vecsize);
	}
	throwIf(!enumTryParse(o, dataType), "dataType {} parse failed", dataType);

	return o;
}

#endif

}

//#endif