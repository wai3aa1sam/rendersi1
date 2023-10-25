#include "rds_shader_compiler-pch.h"
#include "rdsShaderCompiler_Dx12.h"

#if RDS_RENDER_HAS_DX12


// DirectX 12 specific headers.
#include <d3d12.h>
#include <dxgi1_6.h>

// D3D12 extension library.
//#include <directx/d3dx12.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

namespace rds
{

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

	static const wchar_t* toShaderProfile(ShaderStageFlag stage)
	{
		using SRC = ShaderStageFlag;
		switch (stage)
		{
			case SRC::Vertex:	{ return L"vs_6_0"; } break;
			case SRC::Pixel:	{ return L"ps_6_0"; } break;
			case SRC::Compute:	{ return L"cs_6_0"; } break;
		}
		RDS_CORE_ASSERT(false);
		return L"";
	}
};

#if 0
#pragma mark --- rdsShaderCompiler_Dx12-Impl ---
#endif // 0
#if 1

void 
ShaderCompiler_Dx12::onCompile(StrView outpath, StrView filename, ShaderStageFlag stage, StrView entry, const Option& opt)
{
	auto srcPath = Path::realpath(filename);

	ComPtr<Dxc_Compiler>		compiler;
	ComPtr<IDxcUtils>			utils;
	ComPtr<IDxcIncludeHandler>	includeHandler;

	auto ret = ::DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils));
	Util::throwIfError(ret);
	
	ret = ::DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
	Util::throwIfError(ret);

	ret = utils->CreateDefaultIncludeHandler(&includeHandler);
	Util::throwIfError(ret);

	auto entryW = UtfUtil::toTempStringW(entry);

	Vector<const wchar_t*, 32> compileArgs =
	{
		{
			L"-E", entryW.c_str(),
			L"-T", Util::toShaderProfile(stage),
			L"-HV", L"2018",		// 2021 has change vector ternary op to select/any/...
			DXC_ARG_WARNINGS_ARE_ERRORS,
			DXC_ARG_ALL_RESOURCES_BOUND,
		}
	};

	bool isBypassReflection = false;
	if (opt.isToSpirv)
	{
		compileArgs.emplace_back(DXC_ARG_PACK_MATRIX_COLUMN_MAJOR);
		compileArgs.emplace_back(L"-spirv");
		compileArgs.emplace_back(L"-fspv-target-env=vulkan1.2");	// 1.3 cannot read by spirv-cross
		compileArgs.emplace_back(L"-fspv-reflect");

		isBypassReflection = true;	// dxc does not support spirv reflection, use spirv-cross instead
	}
	else		// only available in dx12
	{
		compileArgs.emplace_back(L"DXC_ARG_PACK_MATRIX_ROW_MAJOR");
		compileArgs.emplace_back(L"-Qstrip_reflect");
	}

	if (opt.isDebug)
	{
		compileArgs.emplace_back(L"-Qstrip_debug");
		compileArgs.emplace_back(DXC_ARG_DEBUG);
	}
	else
	{
		compileArgs.push_back(DXC_ARG_OPTIMIZATION_LEVEL3);
	}

	// Load the shader source file to a blob.
	ComPtr<IDxcBlobEncoding> sourceBlob = {};
	auto temp = UtfUtil::toTempStringW(filename);
	ret = utils->LoadFile(temp.data(), nullptr, &sourceBlob);
	Util::throwIfError(ret);

	DxcBuffer sourceBuffer;
	sourceBuffer.Ptr		= sourceBlob->GetBufferPointer();
	sourceBuffer.Size		= sourceBlob->GetBufferSize();
	sourceBuffer.Encoding	= 0;

	// Compile the shader.
	ComPtr<IDxcResult> compiledShaderBuffer = {};
	ret = compiler->Compile(&sourceBuffer,
							compileArgs.data(),
							sCast<u32>(compileArgs.size()),
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
	
	File::writeFile(outpath, bin.byteSpan(), true);

	if (!isBypassReflection)
	{
		reflect(outpath, compiledShaderBuffer.Get(), utils.Get(), stage);
	}
}

void 
ShaderCompiler_Dx12::reflect(StrView outpath, IDxcResult* compiledShaderBuffer, IDxcUtils* utils, ShaderStageFlag stage)
{
	ShaderStageInfo outInfo;
	outInfo.stageFlag = stage;

	// Get shader reflection data.
	ComPtr<IDxcBlob> reflectionBlob = {};
	auto ret = compiledShaderBuffer->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&reflectionBlob), nullptr);
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