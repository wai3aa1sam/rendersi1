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

		auto& mtl = *comp.material.ptr();

		bool isUseProp = true;
		if (!isUseProp)
		{
			mtl.info();
			//mtl.info().props[0].
		}
		else
		{
			for (size_t iPass = 0; iPass < mtl.passes().size(); iPass++)
			{
				auto& pass		= *mtl.getPass(iPass);
				auto& shaderRsc	= pass.shaderResources();
				for (const auto& e : mtl.info().props)
				{
					using SRC = ShaderPropType;
					
					auto*		drawer	= edtCtx.findPropertyDrawer(e.type);
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
							auto* value = shaderRsc.findParam(e.name);
							if (!value)
								continue;
							drawer->draw(propDrawReq, name.c_str(), value);
						} break;
						case SRC::Texture2D:
						{
							auto* rsc = shaderRsc.findTexParam(e.name);
							if (!rsc)
								continue;
							RDS_CORE_ASSERT(rsc->info().dataType == RenderDataType::Texture2D, "only support Texture2D");

							auto* tex = sCast<Texture2D*>(rsc->resource());
							bool hasChanged = drawer->draw(propDrawReq, name, tex);
							if (hasChanged)
								mtl.setParam(e.name, tex);
						} break;
						
					}
				}
			}
		}
	}
}

#endif



}