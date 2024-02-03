#pragma once

#include "rds_render_api_layer/mesh/rdsRenderMesh.h"
#include "rds_render_api_layer/command/rdsRenderRequest.h"
#include "rds_render_api_layer/rdsRenderFrame.h"
#include "rds_render_api_layer/graph/rdsRenderGraph.h"

#include "rds_editor.h"

#define RDS_RENDER_GRAPH_PRESENT 0
#define RDS_TEST_COMPUTE_GROUP_THREAD 256

namespace rds
{

#if 1

struct MeshAssets
{
	void create()
	{
		EditMesh mesh;
		WavefrontObjLoader::loadFile(mesh, "asset/mesh/box.obj");
		box.create(mesh);

		WavefrontObjLoader::loadFile(mesh, "asset/mesh/sphere.obj");
		sphere.create(mesh);
	}

	void destroy()
	{
		box.clear();
		sphere.clear();
	}

	RenderMesh box;
	RenderMesh sphere;
};
MeshAssets meshAssets;

const VertexLayout* getVertexLayout_RndColorTriangle() { return Vertex_PosColorUvNormal<1>::vertexLayout(); }

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
		e.emplace_back(r0,	  0,	 0,		255);
		e.emplace_back( 0,	 r0,	 0,		255);
		e.emplace_back( 0,	  0,	r0,		255);
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
	{
		auto& e = editMesh.normal;
		e.reserve(s_kVtxCount);
		e.emplace_back(1.0f, 0.0f, 1.0f);
		e.emplace_back(0.0f, 0.0f, 1.0f);
		e.emplace_back(0.0f, 1.0f, 1.0f);
		e.emplace_back(1.0f, 1.0f, 1.0f);
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
		e.emplace_back( 3.0f, -1.0f, 0.0f);
		e.emplace_back(-1.0f,  3.0f, 0.0f);
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
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	static constexpr SizeType s_kObjectCount = 10;

public:
	TestScene()
	{
		_rdMesh1.create(makeRndColorTriangleMesh(0.0f, false));
		_rdMesh2.create(makeRndColorTriangleMesh(0.0f, false));

		_rdMeshes.emplace_back(&_rdMesh1);
		_rdMeshes.emplace_back(&_rdMesh2);

		_mtls.reserve(s_kObjectCount);

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

		model		= Mat4f::s_identity();

		Mat4f mvp		= _camera->viewProjMatrix() * model;

		//mvp = model;
		//mvp = Mat4f::s_identity();

		_mvp = mvp;
	}

	void drawScene(RenderRequest& rdReq, Material* mtl, Function<void(Material* mtl, int)>* setMtlFn = nullptr)
	{
		#if 0
		for (size_t i = 0; i < _rdMeshes.size(); i++)
		{
			auto drawCall = rdReq.addDrawCall();

			drawCall->setSubMesh(_rdMeshes[i]->subMesh());
			drawCall->material			= mtl;
			drawCall->materialPassIdx	= 0;
		}
		#endif // 0
		
		#if 0
		auto drawCall = rdReq.addDrawCall();

		drawCall->setSubMesh(meshAssets.box.subMesh());
		drawCall->material			= mtl;
		drawCall->materialPassIdx	= 0;

		mtl->setParam("rds_matrix_mvp", _mvp);
		#endif // 0

		if (_mtls.is_empty() || _mtls[0]->shader() != mtl->shader())
		{
			_mtls.clear();
			for (size_t i = 0; i < s_kObjectCount; i++)
			{
				auto& dst = _mtls.emplace_back();
				dst	= Renderer::rdDev()->createMaterial(mtl->shader());
			}
		}

		{
			float startPosX = -25.0f;
			float stepPosX	= 5.0f;
			for (size_t i = 0; i < s_kObjectCount; i++)
			{
				auto& srcMtl = _mtls[i];
				Mat4f matModel	= Mat4f::s_translate(Vec3f{startPosX + stepPosX * i, i % 2 ? 0.0f : 10.0f, 10.0f});
				if (setMtlFn)
				{
					(*setMtlFn)(srcMtl, sCast<int>(i));
				}
				rdReq.drawMesh(RDS_SRCLOC, meshAssets.sphere, srcMtl, matModel);
			}
		}
	}

	//const Mat4f&			mvp()			const { return _mvp; }
	Mat4f					projMatrix()	const { return _camera->projMatrix(); }
	const math::Camera3f&	camera()		const { return *_camera; }

private:
	Vector<RenderMesh*> _rdMeshes;

	RenderMesh _rdMesh1;
	RenderMesh _rdMesh2;

	Vector<SPtr<Material> > _mtls;

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

	void setup(RenderContext* rdCtx)
	{
		_rdCtx = rdCtx;
		_rdGraph.create("Test Render Graph", rdCtx);
		//return;

		{
			_testShader = Renderer::rdDev()->createShader("asset/shader/test_texture.shader"); RDS_UNUSED(_testShader);

			_testMtl = Renderer::rdDev()->createMaterial();
			_testMtl->setShader(_testShader);

			auto texCDesc = Texture2D::makeCDesc();

			texCDesc.create("asset/texture/uvChecker.png");
			_uvCheckerTex = Renderer::rdDev()->createTexture2D(texCDesc);
			_uvCheckerTex->setDebugName("uvChecker");

			texCDesc.create("asset/texture/uvChecker2.png");
			_uvCheckerTex2 = Renderer::rdDev()->createTexture2D(texCDesc);
			_uvCheckerTex2->setDebugName("uvChecker2");

			_shaderTestBindless	= Renderer::rdDev()->createShader("asset/shader/test_bindless.shader");
			_mtlTestBindless	= Renderer::rdDev()->createMaterial(_shaderTestBindless);
		}

		auto fullScreenTriangleMesh = getFullScreenTriangleMesh();
		_fullScreenTriangle.create(fullScreenTriangleMesh);

		_preDepthShader = Renderer::rdDev()->createShader("asset/shader/preDepth.shader");
		_preDepthMtl	= Renderer::rdDev()->createMaterial(_preDepthShader);

		_gBufferShader	= Renderer::rdDev()->createShader("asset/shader/gBuffer.shader");
		_gBufferMtl		= Renderer::rdDev()->createMaterial(_gBufferShader);

		_presentShader	= Renderer::rdDev()->createShader("asset/shader/present.shader");
		_presentMtl		= Renderer::rdDev()->createMaterial(_presentShader);
		_presentMtl->setParam("texture0", _uvCheckerTex);

		_testComputeShader	= Renderer::rdDev()->createShader("asset/shader/test_compute.shader");
		_testComputeMtl		= Renderer::rdDev()->createMaterial(_testComputeShader);
		testCompute(&_rdGraph, true);

		{
			_shaderSkybox	= Renderer::rdDev()->createShader("asset/shader/skybox.shader");
			_mtlSkybox		= Renderer::rdDev()->createMaterial();
			_mtlSkybox->setShader(_shaderSkybox);

			auto texCDesc = TextureCube::makeCDesc();
			Vector<StrView, TextureCube::s_kFaceCount> filenames;
			filenames.emplace_back("asset/texture/skybox/default/right.png");
			filenames.emplace_back("asset/texture/skybox/default/left.png");
			filenames.emplace_back("asset/texture/skybox/default/top.png");
			filenames.emplace_back("asset/texture/skybox/default/bottom.png");
			filenames.emplace_back("asset/texture/skybox/default/front.png");
			filenames.emplace_back("asset/texture/skybox/default/back.png");

			texCDesc.create(filenames);
			_texDefaultSkybox = Renderer::rdDev()->createTextureCube(texCDesc);
			_texDefaultSkybox->setDebugName("default_skybox");
		}

		_shaderPbr	= Renderer::rdDev()->createShader("asset/shader/pbr.shader");
		_mtlPbr		= Renderer::rdDev()->createMaterial(_shaderPbr);
	}

	void update()
	{
		if (!_rdCtx->isValidFramebufferSize())
			return;

		scene()->update(1.0f / 60.0f, _rdCtx->aspectRatio());

		_rdGraph.reset();

		RdgTextureHnd oTex;
		RdgTextureHnd oTexDepth;

		//oTex = testCompute(_rdGraph, false);
		//oTex = testDeferred(&_rdGraph, &oTexDepth);
		oTex = testPbr(&_rdGraph, &oTexDepth);
		//oTex = testBindless(&_rdGraph, &oTexDepth);

		oTex = testSkybox(&_rdGraph, oTex, oTexDepth);
		finalComposite(&_rdGraph, oTex);

		_rdGraph.compile();
		_rdGraph.execute();
	}

	void commit()
	{
		_rdGraph.commit();
	}

	RdgTextureHnd testDeferred(RenderGraph* outRdGraph, RdgTextureHnd* outTexDepth)
	{
		auto* rdGraph	= outRdGraph;
		auto* rdCtx		= _rdCtx;

		SPtr<Texture2D> backBuffer;
		SPtr<Texture2D> outColor;
		SPtr<Texture2D> out1;

		auto screenSize = Vec2u::s_cast(rdCtx->framebufferSize()).toTuple2();

		RdgTextureHnd testColorTex	= rdGraph->createTexture("testColorTex",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource});
		RdgTextureHnd depthTex		= rdGraph->createTexture("depth_tex",	Texture2D_CreateDesc{ screenSize, ColorType::Depth, TextureUsageFlags::DepthStencil | TextureUsageFlags::ShaderResource});

		#if 0
		auto& depthPrePass = rdGraph->addPass("depth_pre_pass", RdgPassTypeFlags::Graphics);
		depthPrePass.setRenderTarget(testColorTex,						RenderTargetLoadOp::Clear,		RenderTargetStoreOp::Store);
		depthPrePass.setDepthStencil(depthTex,		RdgAccess::Write,	RenderTargetLoadOp::DontCare,	RenderTargetLoadOp::DontCare);
		depthPrePass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearColor(Color4f{0.1f, 0.2f, 0.3f, 1.0f});
				clearValue->setClearDepth(1.0f);

				scene()->drawScene(rdReq, _preDepthMtl);
			});
		#endif // 0

		RdgTextureHnd albedoTex		= rdGraph->createTexture("albedo_tex",		Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource });
		RdgTextureHnd normalTex		= rdGraph->createTexture("normal_tex",		Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource });
		RdgTextureHnd positionTex	= rdGraph->createTexture("position_tex",		Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource });

		auto& gBufferPass = rdGraph->addPass("g_buffer_pass", RdgPassTypeFlags::Graphics);
		//gBufferPass.setRenderTarget({albedoTex, normalTex, positionTex});
		gBufferPass.setRenderTarget(albedoTex,		RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		gBufferPass.setRenderTarget(normalTex,		RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		gBufferPass.setRenderTarget(positionTex,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		gBufferPass.setDepthStencil(depthTex,		RdgAccess::Write, RenderTargetLoadOp::Clear, RenderTargetLoadOp::Clear);	// currently use the pre-pass will cause z-flight
		gBufferPass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), *_camera);

				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearColor(Color4f{0.1f, 0.2f, 0.3f, 1.0f});
				clearValue->setClearDepth(1.0f);

				scene()->drawScene(rdReq, _gBufferMtl);
			}
		);

		#if 0

		auto colorTex = rdGraph->createTexture("color_tex", { screenSize, ColorType::RGBAb, 1, TextureUsageFlags::RenderTarget });

		auto& deferredLightingPass = rdGraph->addPass("deferred_lighting_pass", RdgPassTypeFlags::Graphics);
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

		//rdGraph->exportTexture(outColor, colorTex);
		//rdGraph->exportTexture(out1, albedoTex);
		*outTexDepth = depthTex;
		return normalTex;
	}

	RdgTextureHnd testCompute(RenderGraph* outRdGraph, bool isCreate = false)
	{
		static constexpr int s_kMaxParticleCount = RDS_TEST_COMPUTE_GROUP_THREAD * 10;

		// align 16
		struct Particle
		{
			Tuple2f position;
			Tuple2f velocity;
			Color4f color;
		};

		if (isCreate)
		{
			std::default_random_engine rndEngine((unsigned)time(nullptr));
			std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);
			
			// Initial particle positions on a circle
			Vector<Particle> particles(s_kMaxParticleCount);
			for (auto& particle : particles) 
			{
				float r = 0.25f * sqrt(rndDist(rndEngine));
				float theta = (float)(rndDist(rndEngine) * 2 * 3.14159265358979323846);
				float x = r * cos(theta) * _rdCtx->aspectRatio();
				float y = r * sin(theta);
				particle.position	= Tuple2f(x, y);
				particle.velocity	= Vec2f{Vec2f(x, y) / Vec2f(x, y).magnitude()}.toTuple2();
				//particle.color		= Color4f(1.0f, 1.0f, 1.0f, 1.0f);
				//particle.color		= Color4b(sCast<u8>(rndDist(rndEngine)) * 255, sCast<u8>(rndDist(rndEngine)) * 255, sCast<u8>(rndDist(rndEngine)) * 255, 255);
				particle.color		= Color4f(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine), 1.0f);
			}
			auto cDesc = RenderGpuBuffer::makeCDesc(RDS_SRCLOC);
			cDesc.bufSize	= sizeof(Particle) * s_kMaxParticleCount;
			cDesc.stride	= sizeof(Particle);
			cDesc.typeFlags = RenderGpuBufferTypeFlags::Compute | RenderGpuBufferTypeFlags::Vertex;
			_testComputeLastFrameParticles = Renderer::rdDev()->createRenderGpuBuffer(cDesc);
			_testComputeLastFrameParticles->uploadToGpu(particles.byteSpan());
			return {};
		}

		auto* rdGraph = outRdGraph;
		RdgBufferHnd particlesRead	= rdGraph->importBuffer("particlesRead",		_testComputeLastFrameParticles, RenderGpuBufferTypeFlags::Compute, RenderAccess::Read);
		RdgBufferHnd particlesWrite	= rdGraph->createBuffer("particlesWrite",	RenderGpuBuffer_CreateDesc{ sizeof(Particle) * s_kMaxParticleCount, sizeof(Particle), RenderGpuBufferTypeFlags::Compute | RenderGpuBufferTypeFlags::Vertex });

		auto& test_compute = rdGraph->addPass("test_compute", RdgPassTypeFlags::Compute);
		test_compute.writeBuffer(particlesWrite);
		test_compute.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), *_camera);

				_testComputeMtl->setParam("rds_dt", 1 / 600.0f);
				_testComputeMtl->setParam("in_particle_buffer",		particlesRead.renderResource());
				_testComputeMtl->setParam("out_particle_buffer",	particlesWrite.renderResource());
				rdReq.dispatch(RDS_SRCLOC, _testComputeMtl, s_kMaxParticleCount / RDS_TEST_COMPUTE_GROUP_THREAD, 1, 1);
			}
		);
		
		rdGraph->exportBuffer(&_testComputeLastFrameParticles, particlesWrite, RenderGpuBufferTypeFlags::Compute, RenderAccess::Read);

		auto screenSize = Vec2u::s_cast(_rdCtx->framebufferSize()).toTuple2();
		RdgTextureHnd test_compute_depth_tex	= rdGraph->createTexture("test_compute_depth_tex",		Texture2D_CreateDesc{ screenSize, ColorType::Depth, TextureUsageFlags::DepthStencil });
		RdgTextureHnd test_compute_present_tex	= rdGraph->createTexture("test_compute_present_tex",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource });

		auto& test_compute_result = rdGraph->addPass("test_compute_result", RdgPassTypeFlags::Graphics);
		test_compute_result.readBuffer(particlesWrite, ShaderStageFlag::Vertex);
		test_compute_result.setRenderTarget(test_compute_present_tex, RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		test_compute_result.setDepthStencil(test_compute_depth_tex, RdgAccess::Write,	RenderTargetLoadOp::Clear, RenderTargetLoadOp::Clear);
		test_compute_result.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), *_camera);

				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearColor(Color4f{0.1f, 0.2f, 0.3f, 1.0f});
				clearValue->setClearDepth(1.0f);

				auto* drawCall = rdReq.addDrawCall();
				drawCall->vertexLayout			= Vectex_Pos2fUvColor4f<1>::vertexLayout();
				drawCall->vertexBuffer			= particlesWrite.renderResource();
				drawCall->vertexCount			= s_kMaxParticleCount;
				drawCall->material				= _testComputeMtl;
				drawCall->renderPrimitiveType	= RenderPrimitiveType::Point;	// current set in shader
			}
		);

		return test_compute_present_tex;
	}

	RdgTextureHnd testSkybox(RenderGraph* outRdGraph, RdgTextureHnd texColor, RdgTextureHnd texDepth)
	{
		auto* rdGraph	= outRdGraph;

		auto screenSize		= Vec2u::s_cast(_rdCtx->framebufferSize()).toTuple2();
		//auto skyboxTarget	= _rdGraph.createTexture("skyboxTarget", Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource });

		auto& skyboxPass = _rdGraph.addPass("test_skybox", RdgPassTypeFlags::Graphics);
		//skyboxPass.readTexture(texColor);
		skyboxPass.setRenderTarget(texColor, RenderTargetLoadOp::Load, RenderTargetStoreOp::Store);
		skyboxPass.setDepthStencil(texDepth, RenderAccess::Read, RenderTargetLoadOp::Load, RenderTargetLoadOp::Load);
		skyboxPass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), *_camera);

				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearColor(Color4f{0.1f, 0.2f, 0.3f, 1.0f});
				clearValue->setClearDepth(1.0f);

				auto& mtl = _mtlSkybox;
				mtl->setParam("skybox", _texDefaultSkybox);
				rdReq.drawMesh(RDS_SRCLOC, meshAssets.box, mtl);
			});

		return texColor;
	}

	RdgTextureHnd testPbr(RenderGraph* outRdGraph, RdgTextureHnd* outTexDepth)
	{
		auto*	rdGraph		= outRdGraph;
		auto*	rdCtx		= _rdCtx;
		auto	screenSize	= Vec2u::s_cast(rdCtx->framebufferSize()).toTuple2();

		auto* mtl = _mtlPbr.ptr();

		RdgTextureHnd texPbrColor	= rdGraph->createTexture("pbr_color",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource});
		RdgTextureHnd texPbrDepth	= rdGraph->createTexture("pbr_depth",	Texture2D_CreateDesc{ screenSize, ColorType::Depth, TextureUsageFlags::DepthStencil | TextureUsageFlags::ShaderResource});

		auto& gBufferPass = rdGraph->addPass("pbr", RdgPassTypeFlags::Graphics);
		gBufferPass.setRenderTarget(texPbrColor,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		gBufferPass.setDepthStencil(texPbrDepth,	RdgAccess::Write, RenderTargetLoadOp::Clear, RenderTargetLoadOp::Clear);	// currently use the pre-pass will cause z-flight
		gBufferPass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), *_camera);

				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearColor(Color4f{0.1f, 0.2f, 0.3f, 1.0f});
				clearValue->setClearDepth(1.0f);

				Function<void(Material*, int i)> fn 
					= [&](Material* mtl, int i)
					{
						mtl->setParam("texture0", _uvCheckerTex);
						mtl->setParam("texture1", _uvCheckerTex2);

						{
							static Tuple3f posLight		= {};
							static Tuple3f colorLight	= {};
							static float   roughness	= {};
							static float   ao			= {};
							static float   albedo		= {};
							static float   metallic		= {};


							{
								ImGui::Begin("test pbr");

								auto dragFloat3 = [&](Material* mtl, const char* name, float* v)
									{
										Tuple3f temp {v[0], v[1], v[2]};
										ImGui::DragFloat3(name, v);
										mtl->setParam(name, temp);
										//reinCast<Tuple3f&>(*v) = temp;
									};

								auto dragFloat = [&](Material* mtl, const char* name, float* v)
									{
										float temp  = *v;
										ImGui::DragFloat(name, v);
										mtl->setParam(name, temp);
										//reinCast<float&>(*v) = temp;
									};

								dragFloat3	(mtl, "posView",	&constCast(_camera->pos()).x);
								dragFloat3	(mtl, "posLight",	posLight.data);
								dragFloat3	(mtl, "colorLight",	colorLight.data);
								dragFloat	(mtl, "roughness",	&roughness);
								dragFloat	(mtl, "ao",			&ao);
								dragFloat	(mtl, "albedo",		&albedo);
								dragFloat	(mtl, "metallic",	&metallic);

								//RDS_DUMP_VAR(_camera->pos());

								ImGui::End();
							}
						}

						mtl->setParam("roughness", i % 2 ? 0.0f : 1.0f);
					};
				scene()->drawScene(rdReq, mtl, &fn);
			}
		);

		if (outTexDepth)
		{
			*outTexDepth = texPbrDepth;
		}

		return texPbrColor;
	}
	
	RdgTextureHnd testBindless(RenderGraph* outRdGraph, RdgTextureHnd* outTexDepth)
	{
		static float   rds_test_1	= {};
		static float   rds_test_3	= {};
		static float   rds_test_5	= {};

		auto* mtl = _mtlPbr.ptr();

		{
			ImGui::Begin("test bindless");

			auto dragFloat3 = [&](Material* mtl, const char* name, float* v)
				{
					Tuple3f temp {v[0], v[1], v[2]};
					ImGui::DragFloat3(name, v);
					mtl->setParam(name, temp);
					//reinCast<Tuple3f&>(*v) = temp;
				};

			auto dragFloat = [&](Material* mtl, const char* name, float* v)
				{
					float temp  = *v;
					ImGui::DragFloat(name, v);
					mtl->setParam(name, temp);
					//reinCast<float&>(*v) = temp;
				};

			dragFloat	(mtl, "rds_test_1",		&rds_test_1);
			dragFloat	(mtl, "rds_test_3",		&rds_test_3);
			dragFloat	(mtl, "rds_test_5",		&rds_test_5);

			ImGui::End();
		}

		auto*	rdGraph		= outRdGraph;
		auto*	rdCtx		= _rdCtx;
		auto	screenSize	= Vec2u::s_cast(rdCtx->framebufferSize()).toTuple2();

		RdgTextureHnd texPbrColor	= rdGraph->createTexture("test_bindless_color",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource});
		RdgTextureHnd texPbrDepth	= rdGraph->createTexture("test_bindless_depth",	Texture2D_CreateDesc{ screenSize, ColorType::Depth, TextureUsageFlags::DepthStencil | TextureUsageFlags::ShaderResource});

		auto& gBufferPass = rdGraph->addPass("test_bindless", RdgPassTypeFlags::Graphics);
		gBufferPass.setRenderTarget(texPbrColor,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		gBufferPass.setDepthStencil(texPbrDepth,	RdgAccess::Write, RenderTargetLoadOp::Clear, RenderTargetLoadOp::Clear);	// currently use the pre-pass will cause z-flight
		gBufferPass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), *_camera);

				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearColor(Color4f{0.1f, 0.2f, 0.3f, 1.0f});
				clearValue->setClearDepth(1.0f);

				scene()->drawScene(rdReq, mtl);
			}
		);

		if (outTexDepth)
		{
			*outTexDepth = texPbrDepth;
		}

		return texPbrColor;
	}

	void finalComposite(RenderGraph* outRdGraph, RdgTextureHnd presentTex)
	{
		auto*	rdGraph			= outRdGraph;
		auto	backBufferRt	= rdGraph->importTexture("back_buffer", _rdCtx->backBuffer()); RDS_UNUSED(backBufferRt);

		auto& finalComposePass = _rdGraph.addPass("final_composite", RdgPassTypeFlags::Graphics);
		finalComposePass.readTexture(presentTex);
		finalComposePass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				_presentMtl->setParam("texture0",			presentTex.renderResource());
				_presentMtl->setParam("rds_matrix_model",	Mat4f::s_scale(Vec3f{1.0f, 1.0f, 1.0f}));
			}
		);
	}

	void present(RenderContext* rdCtx, RenderRequest& rdReq, TransferRequest& tsfReq)
	{
		RDS_TODO("the rdReq here must be framed, otherwise some rsc will be deleted while executing");

		// _rdReq should be per frame, but all the present resources are higher lifetime scope
		rdReq.reset(rdCtx);
		auto* clearValue = rdReq.clearFramebuffers();
		clearValue->setClearColor(Color4f{0.1f, 0.2f, 0.3f, 1.0f});
		clearValue->setClearDepth(1.0f);

		rdReq.drawMesh(RDS_SRCLOC, _fullScreenTriangle, _presentMtl);
		rdCtx->drawUI(rdReq);
		tsfReq.commit();
		rdReq.swapBuffers();

		rdCtx->commit(rdReq);
	}

	void dump(StrView filename = "debug/render_graph")
	{
		_rdGraph.dumpGraphviz(filename);
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
	SPtr<Shader> _gBufferShader;
	SPtr<Shader> _presentShader;

	SPtr<Shader>		_testShader;
	SPtr<Material>		_testMtl;
	SPtr<Texture2D>		_uvCheckerTex;
	SPtr<Texture2D>		_uvCheckerTex2;

	SPtr<Shader>		_shaderSkybox;
	SPtr<Material>		_mtlSkybox;
	SPtr<TextureCube>	_texDefaultSkybox;

	SPtr<Shader>		_shaderPbr;
	SPtr<Material>		_mtlPbr;

	SPtr<Shader>		_shaderTestBindless;
	SPtr<Material>		_mtlTestBindless;

	SPtr<Shader>				_testComputeShader;
	SPtr<Material>				_testComputeMtl;
	SPtr<RenderGpuBuffer>		_testComputeLastFrameParticles;

	RenderMesh	_fullScreenTriangle;

	RenderRequest _rdReq;
};

}