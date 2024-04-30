#include "rds_render_api_layer-pch.h"
#include "rdsVertex.h"

#include "rdsVertexLayoutManager.h"

namespace rds
{

VertexSemantic 
VertexSemanticUtil::parse(StrView v)
{
	if (StrUtil::ignoreCaseCompare(v, "POSITION")		== 0)	{ return VertexSemantic::POSITION; }
	if (StrUtil::ignoreCaseCompare(v, "COLOR")			== 0)	{ return VertexSemantic::COLOR0; }
	if (StrUtil::ignoreCaseCompare(v, "TEXTURE")		== 0)	{ return VertexSemantic::TEXCOORD0; }
	if (StrUtil::ignoreCaseCompare(v, "NORMAL")			== 0)	{ return VertexSemantic::NORMAL0; }
	if (StrUtil::ignoreCaseCompare(v, "TANGENT")		== 0)	{ return VertexSemantic::TANGENT0; }
	if (StrUtil::ignoreCaseCompare(v, "BINORMAL")		== 0)	{ return VertexSemantic::BINORMAL0; }

	if (StrUtil::ignoreCaseCompare(v, "SV_Position")	== 0)	{ return VertexSemantic::SV_Position; }
	if (StrUtil::ignoreCaseCompare(v, "SV_Target")		== 0)	{ return VertexSemantic::SV_TARGET0; }
	if (StrUtil::ignoreCaseCompare(v, "SV_VertexID")	== 0)	{ return VertexSemantic::SV_VertexID; }

	TempString buf;
	StrUtil::toUpperCase(buf, v);

	VertexSemantic o;
	throwIf(!enumTryParse(o, buf), "VertexSemantic parse failed");
	return o;
}


}