#pragma once
#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

namespace rds
{

struct	Shader_CreateDesc;
class	Shader;
class	Material;

#if 0
#pragma mark --- rdsShaderStock-Decl ---
#endif // 0
#if 1

class ShaderStock
{
public:
	/*
	using a HashedPermutationVectorMap for faster search
	, hash with its index and value

	Shaders contains all original and permutated shader
	*/
	using Shaders	= Vector<SPtr<Shader>,	2>;
	using Materials = Vector<Material*,		2>;		// owning material will cause it persistent

	using PermutRequest = VectorMap<SPtr<Material>, SPtr<Shader> >;
	//using PermutRequest = VectorMap<Material*, Shader*>;

	/*
	* _shaders should use a StrHashVectorMap<Shaders>;		// using Shaders = HashedPermutationVectorMap
	*/

public:
	ShaderStock();
	~ShaderStock();

	void create(RenderDevice* rdDev);
	void destroy();

public:
	SPtr<Shader>	createShader(const Shader_CreateDesc& cDesc);
	//SPtr<Shader>	createShader(StrView filename);
	//SPtr<Shader>	createShader(Shader* shader, const ShaderPermutations& permuts);
	//SPtr<Material>	createMaterial();

	void removeShader(Shader* shader);

public:
	void appendUniqueMaterial(Material* mtl, Shader* shader);
	void removeMaterial(Material* mtl);

	void sendPermutationRequest(Material* mtl);
	void clearPermutationRequest();

public:
	Shader*		findShader(StrView filename);
	//Shader*		findShader(const Shader_CreateDesc& cDesc);
	Shaders*	getShaders(StrView filename);
	Materials*	getMaterials(Shader* shader);

public:
	PermutRequest& permutationRequest();
	const	PermutRequest& permutationRequest() const;

protected:
	RenderDevice& renderDevice();

private:
	RenderDevice*					_rdDev = nullptr;
	Map<String,			Shaders>	_shaders;
	Map<Shader*,		Materials>	_mtlTable;
	PermutRequest					_permutReq;
};

inline			ShaderStock::PermutRequest& ShaderStock::permutationRequest()		{ return _permutReq; }
inline const	ShaderStock::PermutRequest&	ShaderStock::permutationRequest() const { return _permutReq; }

inline RenderDevice&						ShaderStock::renderDevice()				{ return *_rdDev; }

#endif // 0


}