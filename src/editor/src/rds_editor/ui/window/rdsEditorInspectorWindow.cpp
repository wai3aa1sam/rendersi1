#include "rds_editor-pch.h"
#include "rdsEditorInspectorWindow.h"
#include "../../rdsEditorContext.h"

#include "../property/rdsEditorPropertyDrawer.h"
#include "../property/rdsEditorPropertyDrawRequest.h"

namespace rds
{

#if 0
#pragma mark --- rdsEditorInspectorWindow-Impl ---
#endif // 0
#if 1

void 
EditorInspectorWindow::draw(EditorUiDrawRequest* edtDrawReq, Scene& scene)
{
	auto& edtCtx = edtDrawReq->editorContext();

	auto wnd = window(edtDrawReq, label());

	auto& entSelection = edtCtx.entitySelection();
	for (auto& entId : entSelection.entities())
	{
		auto* ent = scene.findEntity(entId);
		if (!ent)
			continue;

		EditorPropertyDrawRequest propDrawReq;
		propDrawReq.create(edtDrawReq);

		for (auto& c : ent->components())
		{
			drawComponent(&propDrawReq, c);
		}

		break;		// temporary, only support draw one ent, not entities union component
	}
}

void 
EditorInspectorWindow::drawComponent(EditorPropertyDrawRequest* propDrawReq, CComponent* c)
{
	auto& edtCtx		= propDrawReq->editorContext();	
	auto& edtDrawReq	= propDrawReq->editorUiDrawRequest();

	if (c->isTransform)
	{
		auto&			comp	= *sCast<CTransform*>(c);
		const char*		label	= "Transform";
		auto			header	= edtDrawReq.makeCollapsingHeader(label);
		auto			pushId	= edtDrawReq.makePushID(label);

		{
			auto position = comp.localPosition();
			auto* drawer = edtCtx.findPropertyDrawer(position);
			drawer->draw(propDrawReq, "Position", &position);
			comp.setLocalPosition(position);
		}

		{
			auto rotation = math::degrees(comp.localRotation().euler());
			auto* drawer = edtCtx.findPropertyDrawer(rotation);
			drawer->draw(propDrawReq, "Rotation", &rotation);
			comp.setLocalRotation(math::radians(rotation));
		}

		{
			auto scale = comp.localScale();
			auto* drawer = edtCtx.findPropertyDrawer(scale);
			drawer->draw(propDrawReq, "Scale", &scale, Vec3f::s_one());
			comp.setLocalScale(scale);
		}
	}

	if (c->isRenderableMesh)
	{
		auto&			comp	= *sCast<CRenderableMesh*>(c);
		const char*		label	= "RenderableMesh";
		auto			header	= edtDrawReq.makeCollapsingHeader(label);
		auto			pushId	= edtDrawReq.makePushID(label);

		if (comp.material)
		{
			auto& mtl = *comp.material.ptr();

			edtDrawReq.showText(mtl.shader()->shadername());

			for (size_t iPass = 0; iPass < mtl.passes().size(); iPass++)
			{
				auto& pass		= *mtl.getPass(iPass);
				auto& shaderRsc	= pass.shaderResources();

				edtDrawReq.showText(fmtAs_T<TempString>("Pass{}", iPass));

				for (const auto& e : mtl.info().props)
				{
					using SRC = ShaderPropType;

					const auto& name	= !e.displayName.is_empty() ? e.displayName : e.name;

					switch (e.type)
					{
						case SRC::Bool:
						case SRC::Int:
						case SRC::Float:
						case SRC::Vec2f:
						case SRC::Vec3f:
						case SRC::Vec4f:
						case SRC::Color4f:
						{
							auto* drawer	= edtCtx.findPropertyDrawer(e.type);
							auto* value		= shaderRsc.findParam(e.name);
							if (!value)
								continue;
							drawer->draw(propDrawReq, name.c_str(), value);
						} break;
						case SRC::Texture2D:
						{
							// bindless do not have TexParam...
							#if 0
							auto* rsc = shaderRsc.findTexParam(e.name);
							if (!rsc)
								continue;
							RDS_CORE_ASSERT(rsc->info().dataType == RenderDataType::Texture2D, "only support Texture2D");
							auto* tex = sCast<Texture2D*>(rsc->resource());
							#endif // 0

							{
								auto* drawer	= edtCtx.findPropertyDrawer(e.type);
								bool hasChanged = false;

								TempString buf;
								ShaderResources::getTextureNameTo(buf, e.name);
								auto* value = sCast<BindlessResourceHandle::IndexT*>(shaderRsc.findParam(buf));
								if (!value)
								{
									continue;
								}
								auto* texture	= mtl.renderDevice()->textureStock().textures.find(*value);
								RDS_CORE_ASSERT(texture->type() == RenderDataType::Texture2D, "only support Texture2D");
								auto* tex		= sCast<Texture2D*>(texture);

								hasChanged = drawer->draw(propDrawReq, name, tex); RDS_UNUSED(hasChanged);
								if (hasChanged)
									mtl.setParam(e.name, tex);
							}
							{
								auto* drawer = edtCtx.findPropertyDrawer(ShaderPropType::Vec2f);
								bool hasChanged = false;
								
								TempString buf;
								ShaderResources::getTextureStNameTo(buf, e.name);
								auto* texSt	= sCast<Vec4f*>(shaderRsc.findParam(buf));
								if (texSt)
								{
									auto tiling = Vec2f{ texSt->x, texSt->y };
									auto offset = Vec2f{ texSt->z, texSt->w };
									
									hasChanged = drawer->draw(propDrawReq, "tiling", &tiling);
									hasChanged = drawer->draw(propDrawReq, "offset", &offset);
									*texSt = Vec4f{ tiling.x, tiling.y, offset.x, offset.y };
								}
							}

						} break;
					}
				}
			}
		}
	}

	if (c->isLight)
	{
		auto&			comp	= *sCast<CLight*>(c);
		const char*		label	= "Light";
		auto			header	= edtDrawReq.makeCollapsingHeader(label);
		auto			pushId	= edtDrawReq.makePushID(label);
		
		auto lightType = comp.lightType();
		{
			//auto* drawer = edtCtx.findPropertyDrawer(position);
			edtDrawReq.showText(enumStr(lightType));
		}

		{
			auto v = comp.color();
			auto* drawer = edtCtx.findPropertyDrawer(v);
			drawer->draw(propDrawReq, "Color", &v);
			comp.setColor(v);
		}

		{
			auto v = comp.intensity();
			auto* drawer = edtCtx.findPropertyDrawer(v);
			drawer->draw(propDrawReq, "Intensity", &v);
			comp.setIntensity(v);
		}

		if (lightType != LightType::Directional)
		{
			float v = comp.range();
			auto* drawer = edtCtx.findPropertyDrawer(v);
			drawer->draw(propDrawReq, "Range", &v);
			comp.setRange(v);
		}

		if (lightType == LightType::Spot)
		{
			float spotAngle			= math::degrees(comp.spotAngle());
			float spotInnerAngle	= math::degrees(math::acos(comp.spotInnerCosAngle()));

			auto* drawer = edtCtx.findPropertyDrawer(spotAngle);
			drawer->draw(propDrawReq, "SpotAngle",		&spotAngle);
			drawer->draw(propDrawReq, "SpotInnerAngle", &spotInnerAngle);

			comp.setSpotAngle(math::radians(spotAngle));
			comp.setSpotInnerCosAngle(math::cos(math::radians(spotInnerAngle)));
		}
	}
}

#endif



}