#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

namespace rds
{

struct ShaderInfo;

struct ShaderMarco 
{ 
public:
	String name; 
	String value; 
};

#if 0
#pragma mark --- rdsShaderPermutationInfo-Decl ---
#endif // 0
#if 1

struct ShaderPermutationInfo
{
public:
	static constexpr int s_kInvalid = -1;

public:
	String name;
	Vector<String, 2> values;

public:
	bool	isValueExist(StrView val) const;
	int		findValueIdx(StrView val) const;

	template<class JSON_SE>
	void onJsonIo(JSON_SE& se)
	{
		RDS_NAMED_FIXED_IO(se, name);
		RDS_NAMED_FIXED_IO(se, values);		
	}
};

#endif

#if 0
#pragma mark --- rdsShaderPermutations-Decl ---
#endif // 0
#if 1

class ShaderPermutations
{
public:
	struct PermutationParam;

public:
	using SizeType	= RenderApiLayerTraits::SizeType;
	using Params	= Vector<PermutationParam, 2>;

public:
	static constexpr SizeType s_kLocalPermutSize = 4;

public:
	using Info	= Vector<ShaderPermutationInfo,	s_kLocalPermutSize>;

public:
	template<class STR> static  bool getNameTo(STR& o, Span<ShaderPermutationInfo> permutInfos, Span<ShaderMarco> marcos);

public:
	struct PermutationParam
	{
		using Info = ShaderPermutationInfo;
	public:
		void create(const Info& info) 
		{
			_info = &info; 
			_valueIdx = math::clamp(_valueIdx, 0, sCast<int>(info.values.size()));
		}

		void set(int valueIdx) { _valueIdx = valueIdx; }

	public:
		const Info* info() const { return _info; }

		StrView name()		const { return _info->name; }
		StrView value()		const { return _info->values[_valueIdx]; }
		int		valueIdx()	const { return _valueIdx; }

	protected:
		const Info* _info = nullptr;
		int			_valueIdx = 0;
	};

	void create(const Info& info);
	void clear();

	void set(StrView name, StrView value);

public:
	template<class STR> void getNameTo(STR& o) const;

public:
	bool operator==(const ShaderPermutations& permuts) const;
	bool operator!=(const ShaderPermutations& permuts) const;

	const PermutationParam& operator[](SizeType i) const;

public:
	SizeType	size() const;
	const Info& info() const;

	bool		isEmpty() const;

protected:
	bool findIndex(int& nameIdx, int& valueIdx, StrView name, StrView value) const;
	bool isInBoundary(SizeType i) const;

private:
	const Info*	_info = nullptr;
	Params		_permutParams;
};

template<class STR> inline
void 
ShaderPermutations::getNameTo(STR& o) const
{
	TempString temp;
	if (!_permutParams.is_empty())
	{
		temp.append("_");
	}
	for (SizeType i = 0; i < _permutParams.size(); i++)
	{
		auto& permut = _permutParams[i];
		StrUtil::convertToStr(temp, permut.valueIdx());
		o.append(temp.c_str());
	}
}

template<class STR> inline
bool 
ShaderPermutations::getNameTo(STR& o, Span<ShaderPermutationInfo> permutInfos, Span<ShaderMarco> marcos)
{
	o.clear();

	auto& permuts = permutInfos;

	if (permuts.size() == 0)
		return false;

	bool isFailed = false;
	for (size_t i = 0; i < permuts.size(); i++)
	{
		const auto&			permut		= permuts[i];
		int					valueIdx	= ShaderPermutationInfo::s_kInvalid;
		const ShaderMarco*	targetMar	= nullptr;

		// find same name marcos
		{
			for (auto& mar : marcos)
			{
				if (StrUtil::isSame(mar.name, permut.name))
				{
					targetMar = &mar;
					break;
				}
			}
			if (!targetMar)
			{
				isFailed = true;
				break;
			}
		}

		valueIdx = permut.findValueIdx(targetMar->value);
		if (valueIdx == ShaderPermutationInfo::s_kInvalid)
		{
			isFailed = true;
			break;
		}

		o += StrUtil::toStr(valueIdx);
	}

	if (isFailed)
	{
		o.clear();
		return false;
	}

	return true;
}

inline const ShaderPermutations::PermutationParam&	ShaderPermutations::operator[](SizeType i) const	{ RDS_ASSERT(isInBoundary(i), "outside boundary"); return _permutParams[i]; }

inline ShaderPermutations::SizeType					ShaderPermutations::size() const					{ return _permutParams.size(); }
inline const ShaderPermutations::Info&				ShaderPermutations::info() const					{ return *_info; }

inline bool											ShaderPermutations::isEmpty() const					{ return _permutParams.is_empty(); }

inline bool ShaderPermutations::isInBoundary(size_t i) const { return i >= 0 && i < _permutParams.size(); }

#endif


}
