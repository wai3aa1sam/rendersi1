#pragma once

#include "rds_core/common/rds_core_common.h"

namespace rds
{

#if 0
#pragma mark --- rdsProjectSetting-Decl ---
#endif // 0
#if 1

class ProjectSetting : public Singleton<ProjectSetting, ProjectSetting_T>
{
	using Base = ProjectSetting_T;

public:
	void setRdsRoot(	StrView path);
	void setProjectRoot(StrView path);

public:
	StrView importedPath()				const;
	StrView importedShaderPath()		const;
	StrView buildInPath()				const;
	StrView buildInShaderPath()			const;
	StrView shaderPermutationPath()		const;
	StrView shaderCompilerPath()		const;
	
	StrView spirvPath()		const;

	StrView shaderRecompileListPath()	const;

public:
	const String& rdsRoot()				const;
	const String& shaderCompilerRoot()	const;
	const String& buildInRoot()			const;
	const String& buildInShaderRoot()	const;

private:
	static void toRoot(String& dst, const StrView root, const StrView path);

private:
	String _rdsRoot;

	String _shaderCompilerRoot;
	String _buildInRoot;
	String _buildInShaderRoot;
};

inline const String&	ProjectSetting::rdsRoot()				const { return _rdsRoot; }
inline const String&	ProjectSetting::shaderCompilerRoot()	const { return _shaderCompilerRoot; }
inline const String&	ProjectSetting::buildInRoot()			const { return _buildInRoot; }
inline const String&	ProjectSetting::buildInShaderRoot()		const { return _buildInShaderRoot; }

#endif
}