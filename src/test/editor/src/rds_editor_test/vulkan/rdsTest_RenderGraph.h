#pragma once

#include "rds_render_api_layer/mesh/rdsRenderMesh.h"
#include "rds_render_api_layer/command/rdsRenderRequest.h"
#include "rds_render_api_layer/rdsRenderFrame.h"
#include "rds_render_api_layer/graph/rdsRenderGraph.h"

#include "rds_editor.h"

namespace rds
{

#if 1

const VertexLayout* getVertexLayout_RndColorTriangle() { return Vertex_PosColorUv<1>::vertexLayout(); }

EditMesh makeRndColorTriangleMesh(float z = 0.0f, bool isRnd = true)
{
	static size_t s_kVtxCount = 4;
	EditMesh editMesh;
	auto rnd = Random{};
	{
		auto& e = editMesh.pos;
		e.reserve(s_kVtxCount);
		auto v = isRnd ? rnd.range(0.0, 1.0) : 0.5;
		//v = 0.5f;

		e.emplace_back(-0.5f, -v,	 z);
		e.emplace_back( 0.5f,  0.5f, z);
		e.emplace_back( 0.5f, -0.5f, z);
		e.emplace_back(-0.5f,  v,	 z);
	}
	{
		auto& e = editMesh.color;
		e.reserve(s_kVtxCount);
		auto r0 = sCast<u8>(rnd.range(0, 255));
		//auto r1 = sCast<u8>(rnd.range(0, 255));
		e.emplace_back(r0,	  0,	 0,	255);
		e.emplace_back( 0,	 r0,	 0,	255);
		e.emplace_back( 0,	  0,	r0,	255);
		e.emplace_back(255,	 255,	255,	255);
	}
	{
		auto& e = editMesh.uvs[0];
		e.reserve(s_kVtxCount);
		e.emplace_back(1.0f, 0.0f);
		e.emplace_back(0.0f, 0.0f);
		e.emplace_back(0.0f, 1.0f);
		e.emplace_back(1.0f, 1.0f);
	}

	editMesh.indices = { 0, 2, 1, 2, 0, 3 };

	RDS_ASSERT(editMesh.getVertexLayout() == getVertexLayout_RndColorTriangle(), "");
	return editMesh;
}

EditMesh getFullScreenTriangleMesh()
{
	static size_t s_kVtxCount = 3;
	EditMesh editMesh;
	{
		auto& e = editMesh.pos;
		e.reserve(s_kVtxCount);
		e.emplace_back(-1.0f, -1.0f, 0.0f);
		e.emplace_back(3.0f,  -1.0f, 0.0f);
		e.emplace_back(-1.0f, 3.0f, 0.0f);
	}
	{
		auto& e = editMesh.uvs[0];
		e.reserve(s_kVtxCount);
		e.emplace_back(0.0f, 0.0f);
		e.emplace_back(2.0f, 0.0f);
		e.emplace_back(0.0f, 2.0f);
	}

	editMesh.indices = { 0, 1, 2 };

	RDS_ASSERT(editMesh.getVertexLayout() == Vertex_PosUv<1>::vertexLayout(), "");
	return editMesh;
}

#endif // 0

class TestScene
{
public:
	TestScene()
	{
		_rdMesh1.create(makeRndColorTriangleMesh(0.0f, false));
		_rdMesh2.create(makeRndColorTriangleMesh(0.0f, false));

		_rdMeshes.emplace_back(&_rdMesh1);
		_rdMeshes.emplace_back(&_rdMesh2);
	}

	void create(math::Camera3f* camera)
	{
		_camera = camera;
	}

	void update(float dt, float aspectRatio)
	{
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		Mat4f model		= Mat4f::s_rotateZ(math::radians(90.0f) * time);
		//Mat4f view		= Mat4f::s_lookAt(Vec3f{ 2.0f, 2.0f, 2.0f }, Vec3f::s_zero(), Vec3f{ 0.0f, 0.0f, 1.0f });
		//Mat4f proj		= Mat4f::s_perspective(math::radians(45.0f), aspectRatio, 0.1f, 10.0f);
		//proj[1][1]		*= -1;		// no need this line as enabled VK_KHR_Maintenance1 
		Mat4f mvp		= _camera->viewProjMatrix() * model;

		mvp = Mat4f::s_identity();
		
		_mvp = mvp;
	}

	void drawScene(RenderRequest& rdReq, Material* mtl)
	{
		for (size_t i = 0; i < _rdMeshes.size(); i++)
		{
			auto drawCall = rdReq.addDrawCall();

			drawCall->setSubMesh(&_rdMeshes[i]->subMesh());
			drawCall->material			= mtl;
			drawCall->materialPassIdx	= 0;
		}

		mtl->setParam("rds_matrix_mvp", _mvp);
	}

private:
	Vector<RenderMesh*> _rdMeshes;

	RenderMesh _rdMesh1;
	RenderMesh _rdMesh2;

	Mat4f _mvp;
	math::Camera3f* _camera = nullptr;
};

class Test_RenderGraph : public NonCopyable
{
public:

	Test_RenderGraph()
	{
	}

	void setCamera(math::Camera3f* camera)
	{
		_camera = camera;
		_scene.create(camera);
	}

	void setup(RenderContext* rdCtx, bool create = true)
	{
		if (create)
		{
			_rdCtx = rdCtx;
			_rdGraph.create("Test Render Graph", rdCtx);
			//return;

			auto fullScreenTriangleMesh = getFullScreenTriangleMesh();
			_fullScreenTriangle.create(fullScreenTriangleMesh);

			_preDepthShader = Renderer::rdDev()->createShader("asset/shader/preDepth.shader");
			_preDepthMtl	= Renderer::rdDev()->createMaterial(_preDepthShader);

			_gBufferShader	= Renderer::rdDev()->createShader("asset/shader/gBuffer.shader");
			_gBufferMtl		= Renderer::rdDev()->createMaterial(_gBufferShader);

			_presentShader	= Renderer::rdDev()->createShader("asset/shader/present.shader");
			_presentMtl		= Renderer::rdDev()->createMaterial(_presentShader);
		}

		SPtr<Texture2D> backBuffer;
		SPtr<Texture2D> outColor;
		SPtr<Texture2D> out1;

		auto screenSize = Vec2u::s_cast(rdCtx->framebufferSize()).toTuple2();

		RdgTextureHnd testColorTex	= _rdGraph.createTexture("testColorTex",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, 1, TextureFlags::RenderTarget });
		RdgTextureHnd depthTex		= _rdGraph.createTexture("depth_tex",		Texture2D_CreateDesc{ screenSize, ColorType::Depth, 1, TextureFlags::DepthStencil | TextureFlags::ShaderResource});

		auto& depthPrePass = _rdGraph.addPass("depth_pre_pass", RdgPassTypeFlags::Graphics);
		depthPrePass.setRenderTarget(testColorTex,		RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		depthPrePass.setDepthStencil(depthTex, RdgAccess::Write, RenderTargetLoadOp::DontCare, RenderTargetLoadOp::DontCare);
		depthPrePass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearColor(Color4f{0.1f, 0.2f, 0.3f, 1.0f});
				clearValue->setClearDepth(1.0f);

				scene()->drawScene(rdReq, _preDepthMtl);
			});

		
		#if 1
		RdgTextureHnd albedoTex		= _rdGraph.createTexture("albedo_tex",		Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, 1, TextureFlags::RenderTarget });
		RdgTextureHnd normalTex		= _rdGraph.createTexture("normal_tex",		Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, 1, TextureFlags::RenderTarget });
		RdgTextureHnd positionTex	= _rdGraph.createTexture("position_tex",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, 1, TextureFlags::RenderTarget });

		auto& gBufferPass = _rdGraph.addPass("g_buffer_pass", RdgPassTypeFlags::Graphics);
		//gBufferPass.setRenderTarget({albedoTex, normalTex, positionTex});
		gBufferPass.setRenderTarget(albedoTex,		RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		gBufferPass.setRenderTarget(normalTex,		RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		gBufferPass.setRenderTarget(positionTex,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		gBufferPass.setDepthStencil(depthTex,		RdgAccess::Read, RenderTargetLoadOp::Load, RenderTargetLoadOp::Load);
		gBufferPass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearColor(Color4f{0.1f, 0.2f, 0.3f, 1.0f});
				clearValue->setClearDepth(1.0f);

				scene()->drawScene(rdReq, _gBufferMtl);
			}
		);
		#endif // 0

		#if 0

		auto colorTex = _rdGraph.createTexture("color_tex", { screenSize, ColorType::RGBAb, 1, TextureFlags::RenderTarget });

		auto& deferredLightingPass = _rdGraph.addPass("deferred_lighting_pass", RdgPassTypeFlags::Graphics);
		deferredLightingPass.readTextures({albedoTex, normalTex, positionTex, depthTex});
		//deferredLightingPass.setDepthStencil(depthTex, RdgAccess::Read, RenderTargetLoadOp::Load, RenderTargetLoadOp::Load);
		deferredLightingPass.setRenderTarget(colorTex, RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		//deferredLightingPass.setDepthStencil(depthTex, RdgAccess::Write, RenderTargetLoadOp::Load, RenderTargetLoadOp::Load);
		deferredLightingPass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{

			}
		);

		#endif // 0

		#if 1
		auto backBufferRt = _rdGraph.importTexture("back_buffer", _rdCtx->backBuffer()); RDS_UNUSED(backBufferRt);

		auto& finalComposePass = _rdGraph.addPass("final_compose", RdgPassTypeFlags::Graphics);
		finalComposePass.readTexture(albedoTex);
		finalComposePass.setRenderTarget(testColorTex, RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		finalComposePass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				_presentMtl->setParam("texture0",			albedoTex.renderResource());
				_presentMtl->setParam("rds_matrix_model",	Mat4f::s_scale(Vec3f{1.0f, -1.0f, 1.0f}));
				//_presentMtl->setParam("texture0", albedoTex.renderResource());
				//rdReq.drawMesh(RDS_SRCLOC, _fullScreenTriangle);
				//rdReq.swapBuffers();
			}
		);
		#endif // 0

		//_rdGraph.exportTexture(outColor, colorTex);
		//_rdGraph.exportTexture(out1, albedoTex);

	}

	void update()
	{
		if (_rdCtx->framebufferSize().x <= 0 || _rdCtx->framebufferSize().y <= 0)
			return;

		scene()->update(1.0f / 60.0f, _rdCtx->aspectRatio());

		_rdGraph.reset();
		setup(_rdCtx, false);
		_rdGraph.compile();
		_rdGraph.execute();
	}

	void commit()
	{
		_rdGraph.commit();

		{
			_rdReq.reset(_rdCtx);
			_rdReq.drawMesh(RDS_SRCLOC, _fullScreenTriangle, _presentMtl);
			_rdCtx->drawUI(_rdReq);
			_rdReq.swapBuffers();

			_rdCtx->commit(constCast(_rdReq.commandBuffer()));
		}
	}

	void dump(StrView filename = "debug/render_graph")
	{
		_rdGraph.dumpGraphviz(filename);
	}


	void test()
	{

	}

	TestScene* scene() { return &_scene; }

public:
	RenderContext*	_rdCtx	= nullptr;
	math::Camera3f* _camera = nullptr;

	RenderGraph		_rdGraph;
	TestScene		_scene;

	SPtr<Material> _preDepthMtl;
	SPtr<Material> _gBufferMtl;
	SPtr<Material> _presentMtl;

	SPtr<Shader> _preDepthShader;
	SPtr<Shader> _gBufferShader;;
	SPtr<Shader> _presentShader;;

	RenderMesh	_fullScreenTriangle;

	RenderRequest _rdReq;
};

}