/*
reference:
~ https://simoncoenen.com/blog/programming/graphics/DxcCompiling#all-compiler-arguments
*/

#include "rds_shader_compiler-pch.h"
#include "rdsShaderCompiler_Dx12.h"

#if RDS_RENDER_HAS_DX12 && 0

// DirectX 12 specific headers.
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

// D3D12 extension library.
#include <directx/d3dx12.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#endif

#if RDS_RENDER_HAS_VULKAN

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"


#else


#endif // RDS_RENDER_HAS_VULKAN

#if RDS_RENDER_HAS_DX12 || RDS_OS_WINDOWS

namespace rds
{

template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

struct Dx12Util
{
	static void reportError(HRESULT hr) 
	{
		if (!SUCCEEDED(hr)) 
		{
			auto str = getStrFromHRESULT(hr);
			RDS_ERROR("HRESULT(0x{:0X}) {}", sCast<u32>(hr), str);
		}
	}

	static void throwIfError(HRESULT hr)
	{
		if (!_checkError(hr))
		{
			reportError(hr); 
			RDS_THROW("HRESULT = {}", hr);
		}
	}

	static bool _checkError(HRESULT hr)
	{
		return SUCCEEDED(hr);
	}

	static String getStrFromHRESULT(HRESULT hr)
	{
		const int bufSize = 4096;
		wchar_t buf[bufSize + 1];

		DWORD langId = 0; // MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, hr, langId, buf, bufSize, nullptr);
		buf[bufSize] = 0; // ensure terminate with 0

		auto str = UtfUtil::toString(buf);
		return str;
	}
};

struct VkUtil : Vk_RenderApiUtil
{

};

#if 0
#pragma mark --- rdsShaderCompiler_Dx12-Impl ---
#endif // 0
#if 1

void 
ShaderCompiler_Dx12::onCompile(const CompileDesc& desc)
{
	auto srcPath = Path::realpath(desc.filename);

	const auto& opt			= desc.opt;
	const auto& filename	= desc.filename;
	const auto& outpath		= desc.outpath;
	ShaderStageFlag stage	= desc.stage;

	ComPtr<Dxc_Compiler>		compiler;
	ComPtr<IDxcUtils>			utils;
	ComPtr<IDxcIncludeHandler>	includeHandler;

	auto ret = ::DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils));
	Util::throwIfError(ret);
	
	ret = ::DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
	Util::throwIfError(ret);

	ret = utils->CreateDefaultIncludeHandler(&includeHandler);
	Util::throwIfError(ret);

	auto entryW = UtfUtil::toTempStringW(desc.entry);

	Vector<TempStringW, 64> compileArgs;

	compileArgs.emplace_back(L"-E");	compileArgs.emplace_back(entryW);
	compileArgs.emplace_back(L"-T");	compileArgs.emplace_back(UtfUtil::toTempStringW(toShaderStageProfile(desc.stage)));
	compileArgs.emplace_back(L"-HV");	compileArgs.emplace_back(L"2021");							// 2021 has change vector ternary op to select/any/...
	
	compileArgs.emplace_back(DXC_ARG_WARNINGS_ARE_ERRORS);
	compileArgs.emplace_back(DXC_ARG_ALL_RESOURCES_BOUND);
	compileArgs.emplace_back(DXC_ARG_PACK_MATRIX_COLUMN_MAJOR);

	compileArgs.emplace_back(L"-Qstrip_debug");			// adding this then can extract DXC_OUT_PDB

	bool isBypassReflection = false;
	if (opt->isToSpirv)
	{
		//compileArgs.emplace_back(DXC_ARG_PACK_MATRIX_COLUMN_MAJOR);
		compileArgs.emplace_back(L"-spirv");
		compileArgs.emplace_back(L"-fspv-target-env=vulkan1.2");	// 1.3 cannot read by spirv-cross
		compileArgs.emplace_back(L"-fspv-reflect");
		compileArgs.emplace_back(L"-fvk-auto-shift-bindings");

		if (!opt->isNoOffset)
		{
			u32 stageOffset = 0;
			// 16 is minimum spec in vulkan
			u32 cbufferOffset	= stageOffset + 0;		RDS_UNUSED(cbufferOffset);
			u32 textureOffset	= stageOffset + 4;		RDS_UNUSED(textureOffset);
			u32 samplerOffset	= stageOffset + 8;		RDS_UNUSED(samplerOffset);
			u32 uavOffset		= stageOffset + 12;		RDS_UNUSED(uavOffset	);
			u32 imageOffset		= stageOffset + 14;		RDS_UNUSED(imageOffset	);
			// <shift> <space>
			compileArgs.emplace_back(L"-fvk-b-shift"); compileArgs.emplace_back(StrUtil::toStrW(cbufferOffset));	compileArgs.emplace_back(L"0");
			compileArgs.emplace_back(L"-fvk-t-shift"); compileArgs.emplace_back(StrUtil::toStrW(textureOffset));	compileArgs.emplace_back(L"1");	// temporary
			compileArgs.emplace_back(L"-fvk-s-shift"); compileArgs.emplace_back(StrUtil::toStrW(textureOffset));	compileArgs.emplace_back(L"1");
			compileArgs.emplace_back(L"-fvk-u-shift"); compileArgs.emplace_back(StrUtil::toStrW(imageOffset));		compileArgs.emplace_back(L"0");
		}

		// rds_define
		if (true)
		{
			auto& bindlessRsc = Renderer::rdDev()->bindlessResource();

			compileArgs.emplace_back(L"-fvk-bind-globals"); compileArgs.emplace_back(StrUtil::toStrW(14));	compileArgs.emplace_back(StrUtil::toStrW(bindlessRsc.bindlessTypeCount()));
			
			compileArgs.emplace_back(L"-D"); compileArgs.emplace_back(UtfUtil::toTempStringW(fmtAs_T<TempString>("RDS_K_SAMPLER_COUNT={}",				bindlessRsc.samplerCount())));
			compileArgs.emplace_back(L"-D"); compileArgs.emplace_back(UtfUtil::toTempStringW(fmtAs_T<TempString>("RDS_CONSTANT_BUFFER_SPACE=space{}",	bindlessRsc.bindlessTypeCount())));
		}

		isBypassReflection = true;	// dxc does not support spirv reflection (Qstrip_reflect), use spirv-cross instead
	}
	else		// only available in dx12
	{
		//compileArgs.emplace_back(L"DXC_ARG_PACK_MATRIX_ROW_MAJOR");
		compileArgs.emplace_back(L"-Qstrip_reflect");		// adding this then can extract DXC_OUT_REFLECTION, spv do not support it
	}

	if (opt->isDebug)
	{
		compileArgs.emplace_back(DXC_ARG_DEBUG);
	}
	else
	{
		compileArgs.emplace_back(DXC_ARG_OPTIMIZATION_LEVEL3);
	}

	auto curDir = Path::getCurrentDir();
	compileArgs.emplace_back(UtfUtil::toTempStringW(fmtAs_T<TempString>("-I{}/{}", curDir, "asset/shader")));
	compileArgs.emplace_back(UtfUtil::toTempStringW(fmtAs_T<TempString>("-I{}/{}", curDir, "asset/shader/common")));
	compileArgs.emplace_back(UtfUtil::toTempStringW(fmtAs_T<TempString>("-I{}/{}", curDir, "asset/shader/pbr")));
	compileArgs.emplace_back(UtfUtil::toTempStringW(fmtAs_T<TempString>("-I{}/{}", curDir, "asset/shader/terrain")));
	compileArgs.emplace_back(UtfUtil::toTempStringW(fmtAs_T<TempString>("-I{}/{}", curDir, "asset/shader/test")));
	compileArgs.emplace_back(UtfUtil::toTempStringW(fmtAs_T<TempString>("-I{}/{}", curDir, "asset/shader/ui")));

	// Load the shader source file to a blob.
	ComPtr<IDxcBlobEncoding> sourceBlob = {};
	auto temp = UtfUtil::toTempStringW(filename);
	ret = utils->LoadFile(temp.data(), nullptr, &sourceBlob);
	Util::throwIfError(ret);

	DxcBuffer sourceBuffer;
	sourceBuffer.Ptr		= sourceBlob->GetBufferPointer();
	sourceBuffer.Size		= sourceBlob->GetBufferSize();
	sourceBuffer.Encoding	= 0;

	Vector<const wchar_t*, 64> compileArgs_wc;
	compileArgs_wc.reserve(compileArgs.size());
	for (const auto& e : compileArgs)
	{
		compileArgs_wc.emplace_back(e.c_str());
	}

	// Compile the shader.
	ComPtr<IDxcResult> compiledShaderBuffer = {};
	ret = compiler->Compile(&sourceBuffer,
							compileArgs_wc.data(),
							sCast<u32>(compileArgs_wc.size()),
							includeHandler.Get(),   
							IID_PPV_ARGS(&compiledShaderBuffer));
	//Util::throwIfError(ret);

	ComPtr<IDxcBlobUtf8> dxcError = {};
	ret = compiledShaderBuffer->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&dxcError), nullptr);
	//Util::throwIfError(ret);
	bool isFailed = dxcError && dxcError->GetStringLength() > 0;
	if (isFailed)
	{
		RDS_LOG_ERROR("Dxc Compile shader failed \n path: \t{} \n msg: \t{}", filename, (const char*)dxcError->GetBufferPointer());
		return;
	}

	ComPtr<IDxcBlob> pResult;
	compiledShaderBuffer->GetResult(&pResult);

	Vector<u8, 1024> bin;
	const size_t size = pResult->GetBufferSize();
	bin.resize(size);
	memory_copy(bin.data(), sCast<u8*>(pResult->GetBufferPointer()), bin.size());
	
	TempString binFilepath;
	switch (opt->apiType)
	{
		case RenderApiType::Dx12:	{ toBinFilepath(binFilepath, filename, outpath, stage); }						break;
		case RenderApiType::Vulkan: { toBinFilepath(binFilepath, outpath, VkUtil::toShaderStageProfile(stage)); }	break;
	}
	File::writeFile(binFilepath, bin.byteSpan(), true);

	if (_opt->isReflect)
	{
		if (!isBypassReflection)
		{
			reflect(outpath, compiledShaderBuffer.Get(), utils.Get(), stage);
		}
	}
}

StrView
ShaderCompiler_Dx12::toShaderStageProfile(ShaderStageFlag stage)
{
	using SRC = ShaderStageFlag;
	switch (stage)
	{
		case SRC::Vertex:	{ return "vs_6_0"; } break;
		case SRC::Pixel:	{ return "ps_6_0"; } break;
		case SRC::Compute:	{ return "cs_6_0"; } break;
	}
	RDS_CORE_ASSERT(false);
	return "";
}

void 
ShaderCompiler_Dx12::reflect(StrView outpath, IDxcResult* compiledShaderBuffer, IDxcUtils* utils, ShaderStageFlag stage)
{
	ShaderStageInfo outInfo;
	outInfo.stageFlag = stage;

	//ID3D12ShaderReflection* pReflection = NULL;
	//D3DReflect(compiledShaderBuffer->GetBufferPointer(), pPSBlob->GetBufferSize(), IID_ID3D12ShaderReflection, (void**)&pReflection);

	// Get shader reflection data.
	ComPtr<IDxcBlob> reflectionBlob = {};
	auto ret = compiledShaderBuffer->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&reflectionBlob), nullptr);
	Util::throwIfError(ret);

	DxcBuffer reflectionBuffer;
	reflectionBuffer.Ptr		= reflectionBlob->GetBufferPointer();
	reflectionBuffer.Size		= reflectionBlob->GetBufferSize();
	reflectionBuffer.Encoding	= 0;

	ComPtr<ID3D12ShaderReflection> shaderReflection = {};
	ret = utils->CreateReflection(&reflectionBuffer, IID_PPV_ARGS(&shaderReflection));
	Util::throwIfError(ret);

	D3D12_SHADER_DESC shaderDesc = {};
	shaderReflection->GetDesc(&shaderDesc);

	// input
	#if 0
	Vector<D3D12_INPUT_ELEMENT_DESC, 32> inputDescs;
	//for (const uint32_t parameterIndex : std::views::iota(0u, shaderDesc.InputParameters))
	for (u32 i = 0; i < shaderDesc.InputParameters; ++i)
	{
		D3D12_SIGNATURE_PARAMETER_DESC signatureParameterDesc{};
		shaderReflection->GetInputParameterDesc(i, &signatureParameterDesc);

		// Using the semantic name provided by the signatureParameterDesc directly to the input element desc will cause the SemanticName field to have garbage values.
		// This is because the SemanticName filed is a const wchar_t*. I am using a separate std::vector<std::string> for simplicity.
		auto& dst = inputDescs.emplace_back();
		dst.SemanticName	= signatureParameterDesc.SemanticName;
		dst.SemanticIndex	= signatureParameterDesc.SemanticIndex;

		RDS_LOG("semantic name: {}, idx: {}", dst.SemanticName, dst.SemanticIndex);
	}
	#endif // 0

	//_reflect_inputs			(outInfo, compiler, res);
	//_reflect_outputs		(outInfo, compiler, res);
	//_reflect_constBufs		(outInfo, compiler, res);
	//_reflect_textures		(outInfo, compiler, res);
	//_reflect_samplers		(outInfo, compiler, res);
	//_reflect_storageBufs	(outInfo, compiler, res);
	//_reflect_storageImages	(outInfo, compiler, res);

	//String outDir = Path::dirname(outpath);
	String outInfoPath;
	fmtTo(outInfoPath, "{}.json", outpath);
	JsonUtil::writeFileIfChanged(outInfoPath, outInfo, true);
}

#endif

}

#endif