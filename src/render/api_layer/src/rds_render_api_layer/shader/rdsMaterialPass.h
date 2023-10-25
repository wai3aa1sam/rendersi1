#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsShaderInfo.h"
#include "rdsShaderPass.h"
#include "rds_render_api_layer/buffer/rdsRenderGpuMultiBuffer.h"
#include "rds_render_api_layer/texture/rdsTexture.h"

namespace rds
{

class ShaderPass;
struct VertexShaderStage;
struct PixelShaderStage;

class Material;
class MaterialPass;

class RenderContext;

struct ShaderParams;
using FramedShaderParams	= Vector<ShaderParams, RenderApiLayerTraits::s_kFrameInFlightCount>;

#if 0
#pragma mark --- rdsShaderParams-Decl ---
#endif // 0
#if 1

struct ShaderParams
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	static constexpr SizeType s_kLocalConstBufSize = 2;

public:
	template<class T> void setParam(StrView name, const T& v);

	void create(ShaderStage* shaderStage);
	void destroy();

	void uploadToGpu();

	void clear();

	struct ConstBuffer
	{
	public:
		using Info		= ShaderStageInfo::ConstBuffer;
		using VarInfo	= ShaderVariableInfo;

	public:
		static constexpr SizeType s_kLocalDataSize = 2;

	public:
		void create(const Info* info);
		void destroy();

		template<class T> void setParam(StrView name, const T& v)
		{
			const ShaderVariableInfo* var = info()->findVariable(name);
			if (!var)
				return;
			_setParam(*var, v);
		}

		void uploadToGpu();

				u8* data();
		const	u8* data() const;

		const Info* info() const;

	protected:
		void _setParam(const VarInfo& varInfo, const i32&		v)	{ return _setParamCheckType(varInfo, v); }
		void _setParam(const VarInfo& varInfo, const f32&		v)	{ return _setParamCheckType(varInfo, v); }
		void _setParam(const VarInfo& varInfo, const Color4b&	v)	{ return _setParamCheckType(varInfo, v); }
		void _setParam(const VarInfo& varInfo, const Tuple2f&	v)	{ return _setParamCheckType(varInfo, v); }
		void _setParam(const VarInfo& varInfo, const Tuple3f&	v)	{ return _setParamCheckType(varInfo, v); }
		void _setParam(const VarInfo& varInfo, const Tuple4f&	v)	{ return _setParamCheckType(varInfo, v); }
		void _setParam(const VarInfo& varInfo, const Mat4f&		v)	{ return _setParamCheckType(varInfo, v); }

		template<class T> void _setParamCheckType	(const VarInfo& varInfo, const T& v);
		template<class T> bool _checkType			(const VarInfo& varInfo, const T& v);
		template<class T> void _setValue			(const VarInfo& varInfo, const T& v);

	protected:

	public:
		const Info* _info = nullptr;
		Vector<u8, s_kLocalDataSize>	_cpuBuf;
		SPtr<RenderGpuBuffer>			_gpuBuffer;

		bool _isDirty = false;
	};

	const ShaderStageInfo& info() const;

	ConstBuffer&			constBufs(SizeType i);
	Span<ConstBuffer>		constBufs();
	Span<const ConstBuffer>	constBufs() const;

protected:
	const ShaderStageInfo* _info = nullptr;
	Vector<ConstBuffer, s_kLocalConstBufSize> _constBufs;
};

template<class T> inline
void 
ShaderParams::setParam(StrView name, const T& v)
{
	for (auto& e : constBufs())
	{
		e.setParam(name, v);
	}
}

inline
void 
ShaderParams::uploadToGpu()
{
	for (auto& e : constBufs())
	{
		e.uploadToGpu();
	}
}

inline
void 
ShaderParams::clear()
{
	this->_constBufs.clear();
}

inline ShaderParams::ConstBuffer&				ShaderParams::constBufs(SizeType i)	{ return _constBufs[i]; }
inline Span<ShaderParams::ConstBuffer>			ShaderParams::constBufs()			{ return _constBufs; }
inline Span<const ShaderParams::ConstBuffer>	ShaderParams::constBufs() const		{ return spanCast<const ConstBuffer>(_constBufs.span()); }

#if 0
#pragma mark --- rdsShaderParams::ConstBuffer-Impl ---
#endif // 0
#if 1

template<class T> inline
void 
ShaderParams::ConstBuffer::_setParamCheckType(const VarInfo& varInfo, const T& v)
{
	throwIf(!_checkType(varInfo, v), "material set param failed, in valid type");
	_setValue(varInfo, v);
}

template<class T> inline
bool 
ShaderParams::ConstBuffer::_checkType(const VarInfo& varInfo, const T& v)
{
	return varInfo.dataType == RenderDataTypeUtil::get<T>();
}

template<class T> inline
void 
ShaderParams::ConstBuffer::_setValue(const VarInfo& varInfo, const T& v)
{
	auto end = varInfo.offset + sizeof(v);
	throwIf(end > _cpuBuf.size(), "material set param failed, cpuBuffer overflow");
	auto* dst = data() + varInfo.offset;
	*reinCast<T*>(dst) = v;
	_isDirty = true;
}

inline 			u8*									ShaderParams::ConstBuffer::data()		{ return _cpuBuf.data(); }
inline const	u8*									ShaderParams::ConstBuffer::data() const	{ return _cpuBuf.data(); }
inline const	ShaderParams::ConstBuffer::Info*	ShaderParams::ConstBuffer::info() const	{ return _info; }


#endif


#endif

#if 0
#pragma mark --- rdsMaterialPass_Stage-Decl ---
#endif // 0
#if 1

struct MaterialPass_Stage
{
	friend class Material;
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Info					= ShaderStageInfo;
	using ConstBuffer			= ShaderParams::ConstBuffer;

public:
	static constexpr SizeType s_kLocalConstBufSize = 2;

public:
	void create(MaterialPass* pass, ShaderStage* shaderStage);
	void destroy();

	template<class T> void setParam(Material* mtl, StrView name, const T& v);

	const ShaderStageInfo&	info() const;
	ShaderParams&			shaderParams(Material* mtl);

protected:
	ShaderStage* _shaderStage = nullptr;
	FramedShaderParams _framedShaderParams;
};

#if 0
#pragma mark --- rdsMaterialPass_Stage-Impl ---
#endif // 0
#if 1

template<class T> inline 
void 
MaterialPass_Stage::setParam(Material* mtl, StrView name, const T& v)
{
	if (!_shaderStage)
		return;

	shaderParams(mtl).setParam(name, v);
}

inline const ShaderStageInfo& MaterialPass_Stage::info() const { return _shaderStage->info(); }

#endif // 1

struct MaterialPass_VertexStage : public MaterialPass_Stage
{
public:
			VertexShaderStage* shaderStage();
	const	VertexShaderStage* shaderStage() const;

protected:
};

inline			VertexShaderStage* MaterialPass_VertexStage::shaderStage()			{ return sCast<VertexShaderStage*>(_shaderStage); }
inline const	VertexShaderStage* MaterialPass_VertexStage::shaderStage() const	{ return sCast<VertexShaderStage*>(_shaderStage); }

struct MaterialPass_PixelStage : public MaterialPass_Stage
{
public:
			PixelShaderStage* shaderStage();
	const	PixelShaderStage* shaderStage() const;

protected:
};

inline			PixelShaderStage* MaterialPass_PixelStage::shaderStage()		{ return sCast<PixelShaderStage*>(_shaderStage); }
inline const	PixelShaderStage* MaterialPass_PixelStage::shaderStage() const	{ return sCast<PixelShaderStage*>(_shaderStage); }

#endif

#if 0
#pragma mark --- rdsMaterialPass-Decl ---
#endif // 0
#if 1

class MaterialPass
{
	friend class Material;
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using Stage			= MaterialPass_Stage;
	using VertexStage	= MaterialPass_VertexStage;
	using PixelStage	= MaterialPass_PixelStage;

	using Info = ShaderInfo::Pass;

public:
	MaterialPass() {}
	virtual ~MaterialPass() {}

	void create(Material* material, ShaderPass* shaderPass) { onCreate(material, shaderPass); };
	void destroy();

	void bind(RenderContext* ctx, const VertexLayout* vtxLayout) { onBind(ctx, vtxLayout); }

	template<class T> void setParam(StrView name, const T& v);

	const Info& info() const;

	VertexStage*	vertexStage();
	PixelStage*		pixelStage();

protected:
	virtual void onCreate(Material* material, ShaderPass* shaderPass);
	virtual void onDestroy() {};

	virtual void onBind(RenderContext* ctx, const VertexLayout* vtxLayout) = 0;

protected:
	Material*		_material		= nullptr;
	ShaderPass*		_shaderPass		= nullptr;
	VertexStage*	_vertexStage	= nullptr;
	PixelStage*		_pixelStage		= nullptr;
};

inline
void 
MaterialPass::onCreate(Material* material, ShaderPass* shaderPass)
{
	_material	= material;
	_shaderPass = shaderPass;
}

template<class T> inline 
void 
MaterialPass::setParam(StrView name, const T& v)
{
	auto* mtl = _material;
	if (_vertexStage)	_vertexStage->setParam(mtl, name, v);
	if (_pixelStage)	 _pixelStage->setParam(mtl, name, v);
}

inline const MaterialPass::Info& MaterialPass::info() const { return _shaderPass->info(); }

inline MaterialPass::VertexStage*	MaterialPass::vertexStage() { return _vertexStage; }
inline MaterialPass::PixelStage*	MaterialPass::pixelStage()	{ return _pixelStage; }

#endif

}