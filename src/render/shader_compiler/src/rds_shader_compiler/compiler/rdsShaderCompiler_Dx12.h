#pragma once

#include "rds_shader_compiler/common/rds_shader_compiler_common.h"
#include "rdsShaderCompiler.h"

#include <dxcapi.h>

#if RDS_RENDER_HAS_DX12

#include <wrl/client.h>
#include <d3d12shader.h>

namespace rds
{

#if 0
#pragma mark --- rdsShaderCompiler_Dx12-Decl ---
#endif // 0
#if 1

struct Dx12Util;
class ShaderCompiler_Dx12;

class CompileArgs : public Vector<TempStringW, 64>
{
public:
	void appendIncludes(const ShaderIncludes& incs)
	{
		for (auto& e : incs.dirs())
		{
			emplace_back(UtfUtil::toTempStringW(fmtAs_T<TempString>("-I{}", e)));
		}
	}

	void appendMarcos(Span<const ShaderMarco> marcos)
	{
		for (const auto& e : marcos)
		{
			emplace_back(L"-D");
			emplace_back(UtfUtil::toTempStringW(fmtAs_T<TempString>("{}={}", e.name, e.value)));
		}
	}
};

struct DxcCompileDesc
{
	friend class ShaderCompiler_Dx12;
public:
	using Dxc_Compiler	= IDxcCompiler3;
	//using CompileArgs	= Vector<TempStringW, 64>;

public:
	Dxc_Compiler*		dxcCompiler		= nullptr;
	IDxcUtils*			dxcUtils		= nullptr;
	IDxcIncludeHandler* dxcIncHandler	= nullptr;
	DxcBuffer*			dxcSrcBuf		= nullptr;

	StrView		outputFilePath;
	CompileArgs	compileArgs;
};

class ShaderCompiler_Dx12: public ShaderCompiler
{
public:
	using Util = Dx12Util;

public:
	using Dxc_Compiler = IDxcCompiler3;
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

protected:
	/* .hlsl -> .spv */ 
	virtual bool onCompile(const CompileDescView& descView);

private:
	bool _compile(ComPtr<IDxcResult>& oRes, DxcCompileDesc& dxcCmpReq, const CompileDescView& descView, bool isStripLastByte);

protected:
	// reflect .spv by spirv_cross
	void reflect(StrView outpath, IDxcResult* compiledShaderBuffer, IDxcUtils* utils, ShaderStageFlag stage);
	/*void _reflect_inputs		(ShaderStageInfo& outInfo, SpirvCompiler& compiler, const ShaderResources& res);
	void _reflect_outputs		(ShaderStageInfo& outInfo, SpirvCompiler& compiler, const ShaderResources& res);
	void _reflect_constBufs		(ShaderStageInfo& outInfo, SpirvCompiler& compiler, const ShaderResources& res);
	void _reflect_textures		(ShaderStageInfo& outInfo, SpirvCompiler& compiler, const ShaderResources& res);
	void _reflect_samplers		(ShaderStageInfo& outInfo, SpirvCompiler& compiler, const ShaderResources& res);
	void _reflect_storageBufs	(ShaderStageInfo& outInfo, SpirvCompiler& compiler, const ShaderResources& res);
	void _reflect_storageImages	(ShaderStageInfo& outInfo, SpirvCompiler& compiler, const ShaderResources& res);*/

protected:
};

#endif

}

#endif