#include "rds_shader_compiler-pch.h"
#include "rdsShaderCompiler_Vk.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#if RDS_RENDER_HAS_VULKAN


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

void 
ShaderCompiler_Vk::onCompile(const CompileDesc& desc)
{
	if (desc.entry.is_empty())
		return;

	_opt = desc.opt;

	String			srcPath = desc.filename;		// shoulde be realpath
	ShaderStageFlag stage	= desc.stage; 
	TempString		dstpath;
	fmtTo(dstpath, "{}/{}.bin", desc.outpath, Util::toShaderStageProfile(stage));

	//u32 uboStartIdx = 0; u32 texStartIdx = 4; u32 ssboStartIdx = 10; u32 imageStartIdx = 13; u32 samplerStartIdx = 16;

	RDS_TODO("check vulkan extension whether existed then add the compile option, eg. -fhlsl-functionality1");
	RDS_TODO("cbuffer only for hlsl, use ubo for glsl, same for uav, use ssbo");

	TempString args;
	fmtTo(args, "glslc -x hlsl -fshader-stage={} -fentry-point={} -c \"{}\" -o \"{}\" -fhlsl-functionality1 -fhlsl-iomap", SpirvUtil::toStr(stage), desc.entry, srcPath, dstpath);
	fmtTo(args, " -fauto-bind-uniforms");	// auto bind all uniform variable
	fmtTo(args, " -fcbuffer-binding-base {} -ftexture-binding-base {} -fsampler-binding-base {} -fuav-binding-base {} -fimage-binding-base {}", 0, 4, 8, 12 ,14); // 16 is minimum spec in vulkan
	//fmtTo(args, " -I\"dir"");
	
	//fmtTo(args, " -fauto-bind-uniforms -fubo-binding-base 100 -fresource-set-binding b1 1 2");
	//fmtTo(args, " -fresource-set-binding b1 1 2");

	{
		CmdLine cmdl;
		cmdl.execute(args);
	}

	Vector<u8> spvBin;
	File::readFile(dstpath, spvBin);

	reflect(dstpath, spvBin, stage);

	_log("outputed to {}", dstpath);

	_opt = nullptr;
}

void 
ShaderCompiler_Vk::reflect(StrView outpath, ByteSpan spvBytes, ShaderStageFlag stage)
{
	Span<const u32>	u32Spv		= spanCast<const u32>(spvBytes);
	SpirvCompiler	compiler	= { u32Spv.data(), u32Spv.size() };

	auto active			= compiler.get_active_interface_variables();
	ShaderResources res = compiler.get_shader_resources(active);
	compiler.set_enabled_interface_variables(rds::move(active));

	ShaderStageInfo outInfo;
	outInfo.stageFlag = stage;

	_reflect_inputs			(outInfo, compiler, res);
	_reflect_outputs		(outInfo, compiler, res);
	_reflect_constBufs		(outInfo, compiler, res);
	_reflect_textures		(outInfo, compiler, res);
	_reflect_samplers		(outInfo, compiler, res);
	_reflect_storageBufs	(outInfo, compiler, res);
	_reflect_storageImages	(outInfo, compiler, res);

	_reflect_threadGroups	(outInfo, compiler);

	TempString dstpath;
	fmtTo(dstpath, "{}.json", outpath);
	JsonUtil::writeFileIfChanged(dstpath, outInfo, true);

	if (true)
	{

	}
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
		dst.bindPoint	= sCast<u16>(binding);
		dst.bindCount	= sCast<u16>(math::clamp(type.array.size(), sCast<size_t>(1), type.array.size()));

		log("\tname: {}, size: {}, set: {}, binding: {}", name, size, set, binding);

		size_t member_count = type.member_types.size();
		dst.variables.reserve(member_count);

		for (u32 i = 0; i < member_count; i++)
		{
			Variable& dstVar = dst.variables.emplace_back();

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
				dstVar.isRowMajor = compiler.get_decoration(e.id, spv::DecorationRowMajor);
			}

			dstVar.name		= memberName.c_str();
			dstVar.dataType = SpirvUtil::toRenderDataType(memberType);
			dstVar.size		= sCast<u32>(memberSize);
			dstVar.offset	= sCast<u32>(offset);
			log("\t\tmemberName: {}, offset: {}, membSize: {}", memberName, offset, memberSize);
		}
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
		dst.bindPoint	= sCast<u16>(binding);
		dst.bindCount	= sCast<u16>(math::clamp(type.array.size(), sCast<size_t>(1), type.array.size()));

		log("Texture name: {}, set: {}, binding: {}", name, set, binding);
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
		dst.bindPoint	= sCast<u16>(binding);
		dst.bindCount	= sCast<u16>(math::clamp(type.array.size(), sCast<size_t>(1), type.array.size()));

		log("Sampler name: {}, set: {}, binding: {}", name, set, binding);
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
		StorageBuffer& dst		= outInfo.storageBufs.emplace_back();
		const SPIRType& type	= compiler.get_type(resource.base_type_id);
		//size_t size = compiler.get_declared_struct_size(type);

		const std::string& name = compiler.get_name(resource.id);

		u32 set		= compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);	RDS_UNUSED(set);
		u32 binding	= compiler.get_decoration(resource.id, spv::DecorationBinding);			RDS_UNUSED(binding);

		dst.name		= name.c_str();
		dst.bindPoint	= sCast<u16>(binding);
		dst.bindCount	= sCast<u16>(math::clamp(type.array.size(), sCast<size_t>(1), type.array.size()));

		log("StorageBuffer name: {}, set: {}, binding: {}", name, set, binding);
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
		dst.bindPoint	= sCast<u16>(binding);
		dst.bindCount	= sCast<u16>(math::clamp(type.array.size(), sCast<size_t>(1), type.array.size()));

		log("StorageImage name: {}, set: {}, binding: {}", name, set, binding);
	}

	log("");
}

void 
ShaderCompiler_Vk::_reflect_threadGroups(ShaderStageInfo& outInfo, SpirvCompiler& compiler)
{
	if (!BitUtil::has(outInfo.stageFlag, ShaderStageFlag::Compute))
		return;

	auto ex = compiler.get_execution_model();
	ex = ex;

	//compiler();

	auto constants = compiler.get_specialization_constants();
	for (auto& e : constants)
	{
		e.constant_id;
	}
	SpecializationConstant a[3];
	compiler.get_work_group_size_specialization_constants(a[0], a[1], a[2]);		// ??????????????
	a[0];
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

		case SRC::Struct:	{ RDS_THROW("not support struct"); } break;

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

#endif