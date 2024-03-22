#pragma once

#include "rds_render_api_layer/mesh/rdsRenderMesh.h"
#include "rds_render_api_layer/command/rdsRenderRequest.h"
#include "rds_render_api_layer/rdsRenderFrame.h"
#include "rds_render_api_layer/graph/rdsRenderGraph.h"

#include "rds_editor.h"

#define RDS_RENDER_GRAPH_PRESENT 0
#define RDS_TEST_COMPUTE_GROUP_THREAD 256
#define RDS_TEST_BUFFER_ELEMENT_COUNT 4

namespace rds
{

struct TestBindlessBuffer
{
	Color4f color;
};


#if 1

EditMesh getFullScreenTriangleMesh();

struct MeshAssets
{
	void create()
	{
		EditMesh mesh;
		WavefrontObjLoader::loadFile(mesh, "asset/mesh/box.obj");
		box.create(mesh);

		WavefrontObjLoader::loadFile(mesh, "asset/mesh/sphere.obj");
		sphere.create(mesh);

		fullScreenTriangle.create(getFullScreenTriangleMesh());
	}

	void destroy()
	{
		box.clear();
		sphere.clear();
		fullScreenTriangle.clear();
	}

	RenderMesh box;
	RenderMesh sphere;

	RenderMesh fullScreenTriangle;

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

#if 0


class ObjectTransformBuffer
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using T = Mat4f;

public:
	ObjectTransformBuffer()
	{

	}

	void setValue(SizeType i, const T& v)
	{
		at(i) = v;
	}

	void rotateFrame()
	{
		//u32 iFrame = _gpuBufs.iFrame();


	}

public:
			T& at(SizeType i)			{ return sCast<			T&>(cpuBuffer()[i * sizeof(T)]); }
	const	T& at(SizeType i) const		{ return sCast<const	T&>(cpuBuffer()[i * sizeof(T)]); }

			RenderGpuBuffer& gpuBuffer();
	const	RenderGpuBuffer& gpuBuffer() const;

			Vector<u8>& cpuBuffer();
	const	Vector<u8>& cpuBuffer() const;

private:
	RenderGpuMultiBuffer						_gpuBufs;
	Vector<Vector<u8>, s_kFrameInFlightCount>	_cpuBufs;;
};

class CSystem_Base : public NonCopyable
{
public:

};

class CRenderableSystem : public CSystem_Base
{
public:

	void render(RenderRequest& rdReq)
	{

	}
};

class CTransform;

using EntityId = u32;
class Entity : public RefCount_Base
{
public:

private:
	CTransform	_transform;
	EntityId	_id = 0;
};

class Scene
{
public:



private:
	Vector<Entity*>					_entities;
	VectorMap<EntityId, Entity*>	_entityTable;
};

class CComponent : public RefCount_Base
{
public:

protected:

};

class CTransform : public CComponent
{
public:


private:
	Vec3f	_localPosition;
	Vec3f	_localScale;
	Quat4f	_localRotation;

	Mat4f _matLocal;
	//Mat4f _matWorld;


	bool _isDirty = false;

};

class CRenderable : public CComponent
{
public:

	virtual void render(RenderRequest& rdReq)
	{

	}

protected:

};

class CMeshRenderable : public CRenderable
{
public:
	using Base = CRenderable;

public:

	virtual void render(RenderRequest& rdReq)
	{
		Base::render(rdReq);


	}

protected:

};

#endif // 1


class TestScene
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	static constexpr SizeType s_kObjectCount = 25;

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
		
	}

	void drawScene(RenderRequest& rdReq, Material* mtl, Function<void(Material* mtl, int)>* setMtlFn)
	{
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
			auto row = math::sqrt(sCast<int>(s_kObjectCount));
			auto col = row;

			auto stepPos	= Tuple2f{3.0f,	3.0f};
			auto startPos	= Tuple2f{-0.0f, 0.0f};


			for (size_t r = 0; r < row; r++)
			{
				for (size_t c = 0; c < col; c++)
				{
					auto i = r * col + c;
					auto& srcMtl = _mtls[i];
					Mat4f matModel	= Mat4f::s_translate(Vec3f{startPos.x + stepPos.x * c, 0.0f, startPos.y + stepPos.y * r});
					if (setMtlFn)
					{
						(*setMtlFn)(srcMtl, sCast<int>(i));
					}
					//rdReq.drawMesh(RDS_SRCLOC, meshAssets.sphere, srcMtl, matModel);

					PerObjectParam perObjectParam;
					perObjectParam.id = sCast<u32>(i);
					rdReq.drawMesh(RDS_SRCLOC, meshAssets.sphere, srcMtl, perObjectParam);
					rdReq.setMaterialCommonParams(srcMtl, matModel);
				}
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

class RenderFeaturePass {};
class RfpPbr : RenderFeaturePass
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	virtual ~RfpPbr()
	{
		destroy();
	}

	SPtr<Shader> _shaderHdrToCube;
	SPtr<Shader> _shaderIrradianceEnvCube;
	SPtr<Shader> _shaderPrefilteredEnvCube;
	SPtr<Shader> _shaderBrdfLut;

	using CubeMaterial = Vector<SPtr<Material>, TextureCube::s_kFaceCount >;
	CubeMaterial _mtlsHdrToCube;
	CubeMaterial _mtlsIrradianceEnvCube;
	CubeMaterial _mtlsPrefilteredEnvCube;

	void create(Shader* shaderHdrToCube, Shader* shaderIrradianceEnvCube, Shader* shaderPrefilteredEnvCube, Shader* shaderBrdfLut)
	{
		_shaderHdrToCube			= shaderHdrToCube;
		_shaderIrradianceEnvCube	= shaderIrradianceEnvCube;
		_shaderPrefilteredEnvCube	= shaderPrefilteredEnvCube;
		_shaderBrdfLut				= shaderBrdfLut;
	}

	void destroy()
	{

	}

	struct OutputTextures
	{
		TextureCube*	texCubeEnvMap			= nullptr;
		TextureCube*	texCubeIrradinceMap		= nullptr;
		TextureCube*	texCubePrefilteredMap	= nullptr;
		Texture2D*		texBrdfLut				= nullptr;
	};

public:
	void requestIrradianceEnvCube(RenderGraph* outRdGraph, OutputTextures* outTexs, Texture2D* texHdrEnvMap)
	{
		auto* rdGraph = outRdGraph;

		//auto		cubeSize	= sCast<float>(size);

		#if 1
		auto* passHdrToCube			= renderToCube("hdrToCube",			rdGraph, outTexs->texCubeEnvMap, _shaderHdrToCube, nullptr
			, _mtlsHdrToCube
			, [=](Material* mtl, u32 mip, u32 face)
			{
				mtl->setParam("equirectangularMap", texHdrEnvMap);
			}
		); RDS_UNUSED(passHdrToCube);
		#if 1
		auto* passIrradianceEnvCube = renderToCube("irradianceEnvCube", rdGraph, outTexs->texCubeIrradinceMap, _shaderIrradianceEnvCube, passHdrToCube
			, _mtlsIrradianceEnvCube
			, [=](Material* mtl, u32 mip, u32 face)
			{
				mtl->setParam("deltaPhi",	0.025f);
				mtl->setParam("deltaTheta", 0.025f);
				mtl->setParam("envCubeMap", outTexs->texCubeEnvMap);
			}
		); RDS_UNUSED(passIrradianceEnvCube);
		#endif // 0
		#if 1
		auto* passPrefilteredEnvCube = renderToCube("PrefilteredEnvCube", rdGraph, outTexs->texCubePrefilteredMap, _shaderPrefilteredEnvCube, passIrradianceEnvCube
			, _mtlsPrefilteredEnvCube
			, [=](Material* mtl, u32 mip, u32 face)
			{
				mtl->setParam("roughness",	sCast<float>(mip) / outTexs->texCubePrefilteredMap->mipCount());
				mtl->setParam("envCubeMap", outTexs->texCubeEnvMap);
			}
		); RDS_UNUSED(passPrefilteredEnvCube);
		#endif // 0
		#else
		RdgPass* passPrefilteredEnvCube = nullptr;
		#endif // 0

		requestBrdfLut(rdGraph, outTexs->texBrdfLut, meshAssets.fullScreenTriangle, passPrefilteredEnvCube);
	}

	void requestBrdfLut(RenderGraph* outRdGraph, Texture2D* outBrdfLut, RenderMesh& quad, RdgPass* dependency)
	{
		auto* rdGraph = outRdGraph;

		auto texDst		= rdGraph->importTexture("texBrdfLut", outBrdfLut);
		auto viewport	= Rect2f{ Tuple2f::s_zero(), Tuple2f::s_cast(outBrdfLut->size())};


		auto& passBrdfLut = rdGraph->addPass("brdfLut", RdgPassTypeFlags::Graphics);
		passBrdfLut.runAfter(dependency);
		passBrdfLut.setRenderTarget(texDst,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		passBrdfLut.setExecuteFunc(
			[=](RenderRequest& rdReq) 
			{
				rdReq.reset(rdGraph->renderContext());

				auto mtl = Renderer::rdDev()->createMaterial(_shaderBrdfLut);

				rdReq.setViewport(viewport);
				rdReq.setScissorRect(viewport);

				rdReq.setMaterialCommonParams(mtl, Mat4f::s_identity());
				rdReq.drawMesh(RDS_SRCLOC, meshAssets.fullScreenTriangle, mtl);
			});

		auto& passEnvMapTransit = rdGraph->addPass("brdfLut_transit", RdgPassTypeFlags::Graphics);
		passEnvMapTransit.readTexture(texDst);
		passEnvMapTransit.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
			}
		);
	}

	// std::function can accept l and r value type
	static RdgPass* renderToCube(StrView name, RenderGraph* outRdGraph, TextureCube* outCube, Shader* shader, RdgPass* dependency, CubeMaterial& mtls, std::function<void(Material*, u32, u32)> setMtlFunc)
	{
		//Vector<SPtr<Material>, TextureCube::s_kFaceCount> mtls;
		RdgTextureHnd	texDst;
		RdgPass*		lastPass = nullptr;

		auto*		rdGraph		= outRdGraph;
		ColorType	outFormat	= outCube->format();
		auto		cubeSize	= sCast<float>(outCube->size());
		auto		center		= Vec3f{ 0.0f, 0.0f, 0.0f};
		auto		mipCount	= Texture_mipCount(sCast<u32>(cubeSize));
		//mipCount = 1;

		{
			//matProj[1][1] *= -1.0f;
			if (mtls.is_empty())
			{
				RDS_TODO("remove, material should not store mvp related param");
				mtls.resize(TextureCube::s_kFaceCount * mipCount);
				for (auto& e : mtls)
				{
					e = Renderer::rdDev()->createMaterial(shader);
				}
			}

			TempString renderToCubeName;
			fmtToNew(renderToCubeName, "{}_tex_temp_rdToCube", name);

			RdgPass* lastCopyPass = nullptr;
			RdgTextureHnd texTempToCubeColor = rdGraph->createTexture(renderToCubeName
						, Texture2D_CreateDesc{ sCast<u32>(cubeSize), sCast<u32>(cubeSize), outFormat, TextureUsageFlags::RenderTarget | TextureUsageFlags::TransferSrc});

			for (u32 mip = 0; mip < mipCount; mip++)
			{
				for (u32 face = 0; face < TextureCube::s_kFaceCount; face++)
				{
					auto* mtl = mtls[mip * TextureCube::s_kFaceCount + face].ptr(); RDS_UNUSED(mtl);

					auto viewportFactor = math::pow(0.5f, sCast<float>(mip));
					auto viewport		= Rect2f{ Tuple2f::s_zero(), Tuple2f{cubeSize * viewportFactor, cubeSize * viewportFactor}};

					setMtlFunc(mtl, mip, face);

					fmtToNew(renderToCubeName, "{}-m{}-f{}", name, mip, face);
					auto& passRdToCube = rdGraph->addPass(renderToCubeName, RdgPassTypeFlags::Graphics);
					passRdToCube.runAfter(lastCopyPass);
					passRdToCube.setRenderTarget(texTempToCubeColor,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
					if (face == 0 && mip == 0)
					{
						passRdToCube.runAfter(dependency);
					}
					passRdToCube.setExecuteFunc(
						[=](RenderRequest& rdReq) 
						{
							rdReq.reset(rdGraph->renderContext());

							Vector<Mat4f, TextureCube::s_kFaceCount> matViews = {
								Mat4f::s_lookAt(center, Vec3f::s_right(),	Vec3f{0.0f, -1.0f,  0.0f}),
								Mat4f::s_lookAt(center, Vec3f::s_left(),	Vec3f{0.0f, -1.0f,  0.0f}),
								Mat4f::s_lookAt(center, Vec3f::s_up(),		Vec3f{0.0f,  0.0f,  1.0f}),
								Mat4f::s_lookAt(center, Vec3f::s_down(),	Vec3f{0.0f,  0.0f, -1.0f}),
								Mat4f::s_lookAt(center, Vec3f::s_forward(), Vec3f{0.0f, -1.0f,  0.0f}),
								Mat4f::s_lookAt(center, Vec3f::s_back(),	Vec3f{0.0f, -1.0f,  0.0f}),
							};

							auto	matProj	= Mat4f::s_perspective(math::radians(90.0f), 1.0, 0.1f, viewport.size.x);
							auto&	matView	= matViews[face];
							auto	mvp		= matProj * matView;

							rdReq.setViewport(viewport);
							rdReq.setScissorRect(viewport);
							rdReq.matrix_proj = matProj;
							rdReq.matrix_view = matView;
							rdReq.drawMesh(RDS_SRCLOC, meshAssets.box, mtl);
						});

					fmtToNew(renderToCubeName, "{}_tex-m{}-f{}", name, mip, face);
					texDst = rdGraph->importTexture(renderToCubeName, outCube);

					fmtToNew(renderToCubeName, "{}_copyToDst-m{}-f{}", name, mip, face);
					auto& passCopyPbrEnvToCube = rdGraph->addPass(renderToCubeName, RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Transfer);
					passCopyPbrEnvToCube.readTexture(texTempToCubeColor);
					passCopyPbrEnvToCube.writeTexture(texDst);
					passCopyPbrEnvToCube.setExecuteFunc(
						[=](RenderRequest& rdReq)
						{
							rdReq.copyTexture(RDS_SRCLOC, outCube, texTempToCubeColor.renderResource(), sCast<u32>(viewport.w), sCast<u32>(viewport.h), 0, face, 0, mip);
						}
					);

					lastCopyPass = &passCopyPbrEnvToCube;
				}
			}

			#if 1
			fmtToNew(renderToCubeName, "{}_transit", name);
			auto& passEnvMapTransit = rdGraph->addPass(renderToCubeName, RdgPassTypeFlags::Graphics);
			passEnvMapTransit.readTexture(texDst);
			passEnvMapTransit.setExecuteFunc(
				[=](RenderRequest& rdReq)
				{
				}
			);
			#else
			rdGraph->exportTexture(texDstIrradiance, TextureUsageFlags::ShaderResource);		// this is not work, idk why, but export only transit in the end
			#endif // 0

			lastPass = &passEnvMapTransit;
		}

		return lastPass;
	}
};

class Test_RenderGraph : public NonCopyable
{
public:
	RfpPbr _rfpPbr;
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
			auto texCDesc = Texture2D::makeCDesc();

			texCDesc.create("asset/texture/uvChecker.png");
			_uvCheckerTex = Renderer::rdDev()->createTexture2D(texCDesc);
			_uvCheckerTex->setDebugName("uvChecker");

			texCDesc.create("asset/texture/uvChecker2.png");
			_uvCheckerTex2 = Renderer::rdDev()->createTexture2D(texCDesc);
			_uvCheckerTex2->setDebugName("uvChecker2");
		}

		{
			if (false)
			{
				_testShader = Renderer::rdDev()->createShader("asset/shader/test/test_texture.shader"); RDS_UNUSED(_testShader);
				_testMtl = Renderer::rdDev()->createMaterial();
				_testMtl->setShader(_testShader);
				_testMtl->setParam("texture0", _uvCheckerTex);
			}

			_testComputeShader	= Renderer::rdDev()->createShader("asset/shader/test/test_compute_bindless.shader");
			_testComputeMtl		= Renderer::rdDev()->createMaterial(_testComputeShader);
			testCompute(&_rdGraph, nullptr, true);

			_shaderTestBindless	= Renderer::rdDev()->createShader("asset/shader/test/test_bindless.shader");
			_mtlTestBindless	= Renderer::rdDev()->createMaterial(_shaderTestBindless);

			auto bufCDesc = RenderGpuBuffer::makeCDesc();
			bufCDesc.typeFlags		= RenderGpuBufferTypeFlags::Compute;
			bufCDesc.bufSize		= sizeof(TestBindlessBuffer) * RDS_TEST_BUFFER_ELEMENT_COUNT;

			/*static SPtr<RenderGpuBuffer> k;
			k = Renderer::rdDev()->createRenderGpuBuffer(bufCDesc);*/

			_testBindlessBuffer		= Renderer::rdDev()->createRenderGpuBuffer(bufCDesc);
			_testBindlessRwBuffer	= Renderer::rdDev()->createRenderGpuBuffer(bufCDesc);
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

		{
			_shaderPbr					= Renderer::rdDev()->createShader("asset/shader/pbr/pbrBasic.shader");
			_shaderPbrIbl				= Renderer::rdDev()->createShader("asset/shader/pbr/pbrIbl.shader");
			_shaderHdrToCube			= Renderer::rdDev()->createShader("asset/shader/pbr/hdrToCube.shader");
			_shaderIrradianceEnvCube	= Renderer::rdDev()->createShader("asset/shader/pbr/irradianceEnvCube.shader");
			_shaderPrefilteredEnvCube	= Renderer::rdDev()->createShader("asset/shader/pbr/PrefilteredEnvCube.shader");
			_shaderBrdfLut				= Renderer::rdDev()->createShader("asset/shader/pbr/brdfLut.shader");

			_mtlPbr			= Renderer::rdDev()->createMaterial(_shaderPbr);
			_mtlPbrIbl		= Renderer::rdDev()->createMaterial(_shaderPbrIbl);

			SamplerState samplerState;
			samplerState.wrapU = SamplerWrap::ClampToEdge;
			samplerState.wrapV = SamplerWrap::ClampToEdge;
			samplerState.wrapS = SamplerWrap::ClampToEdge;

			auto texCDesc = Texture2D::makeCDesc();
			texCDesc.create("asset/texture/hdr/newport_loft.hdr");
			_texHdrEnvMap = Renderer::rdDev()->createTexture2D(texCDesc);
			_texHdrEnvMap->setDebugName("texHdrEnvMap");

			auto texCubeCDesc = TextureCube::makeCDesc();
			texCubeCDesc.create(512, ColorType::RGBAf, true, TextureUsageFlags::TransferDst | TextureUsageFlags::ShaderResource, samplerState);
			_texCubeEnvMap = Renderer::rdDev()->createTextureCube(texCubeCDesc);
			_texCubeEnvMap->setDebugName("texCubeHdrEnvMap");

			texCubeCDesc.create(32, ColorType::RGBAf, true, TextureUsageFlags::TransferDst | TextureUsageFlags::ShaderResource, samplerState);
			_texCubeIrradianceEnvMap = Renderer::rdDev()->createTextureCube(texCubeCDesc);
			_texCubeIrradianceEnvMap->setDebugName("texCubeIrradianceEnvMap");
			
			texCubeCDesc.create(512, ColorType::RGBAh, true, TextureUsageFlags::TransferDst | TextureUsageFlags::ShaderResource, samplerState);
			_texCubePrefilteredEnvMap = Renderer::rdDev()->createTextureCube(texCubeCDesc);
			_texCubePrefilteredEnvMap->setDebugName("texCubePrefilteredEnvMap");

			texCDesc = Texture2D_CreateDesc{ Tuple2u{ 512, 512 }, ColorType::RGh, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource, samplerState };
			//texCDesc.create("asset/texture/brdf_lut.png");
			_texBrdfLut = Renderer::rdDev()->createTexture2D(texCDesc);
			_texBrdfLut->setDebugName("texBrdfLut");
		}

		_rfpPbr.create(_shaderHdrToCube, _shaderIrradianceEnvCube, _shaderPrefilteredEnvCube, _shaderBrdfLut);
	}

	void prepare()
	{
		auto* rdGraph = &_rdGraph;
		rdGraph->reset();

		RfpPbr::OutputTextures outTexs;
		outTexs.texBrdfLut				= _texBrdfLut;
		outTexs.texCubeEnvMap			= _texCubeEnvMap;
		outTexs.texCubeIrradinceMap		= _texCubeIrradianceEnvMap;
		outTexs.texCubePrefilteredMap	= _texCubePrefilteredEnvMap;
		_rfpPbr.requestIrradianceEnvCube(rdGraph, &outTexs, _texHdrEnvMap);

		_rdGraph.compile();
		_rdGraph.execute();
	}

	void update()
	{
		if (!_rdCtx->isValidFramebufferSize())
			return;

		scene()->update(1.0f / 60.0f, _rdCtx->aspectRatio());

		_rdGraph.reset();

		RdgTextureHnd oTex;
		RdgTextureHnd oTexDepth;
		RdgTextureHnd oTexEnvIrradianceCube;

		//oTex = testCompute(&_rdGraph, &oTexDepth, false);
		//oTex = testDeferred(&_rdGraph, &oTexDepth);
		//oTex = testPbr(&_rdGraph, &oTexDepth, &oTexEnvIrradianceCube);
		oTex = testBindless(&_rdGraph, &oTexDepth);

		auto* texSkybox = 0 ? &oTexEnvIrradianceCube : nullptr; RDS_UNUSED(texSkybox);

		RDS_TODO("has bug in render graph, when testCompute + testSkybox");
		oTex = testSkybox(&_rdGraph, oTex, oTexDepth, texSkybox);
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

				scene()->drawScene(rdReq, _gBufferMtl, nullptr);
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

	RdgTextureHnd testCompute(RenderGraph* outRdGraph, RdgTextureHnd* outTexDepth, bool isCreate)
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
		RdgBufferHnd particlesRead	= rdGraph->importBuffer("particlesRead",	_testComputeLastFrameParticles);
		RdgBufferHnd particlesWrite	= rdGraph->createBuffer("particlesWrite",	RenderGpuBuffer_CreateDesc{ sizeof(Particle) * s_kMaxParticleCount, sizeof(Particle), RenderGpuBufferTypeFlags::Compute | RenderGpuBufferTypeFlags::Vertex });

		auto& test_compute = rdGraph->addPass("test_compute", RdgPassTypeFlags::Compute);
		test_compute.readBuffer(particlesRead);
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

		if (outTexDepth)
		{
			*outTexDepth = test_compute_depth_tex;
		}

		return test_compute_present_tex;
	}

	RdgTextureHnd testSkybox(RenderGraph* outRdGraph, RdgTextureHnd texColor, RdgTextureHnd texDepth, RdgTextureHnd* texSkybox = nullptr)
	{
		auto* rdGraph	= outRdGraph;

		auto screenSize		= Vec2u::s_cast(_rdCtx->framebufferSize()).toTuple2();
		//auto skyboxTarget	= _rdGraph.createTexture("skyboxTarget", Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource });

		auto& skyboxPass = _rdGraph.addPass("test_skybox", RdgPassTypeFlags::Graphics);
		skyboxPass.setRenderTarget(texColor, RenderTargetLoadOp::Load, RenderTargetStoreOp::Store);
		if (texDepth)
			skyboxPass.setDepthStencil(texDepth, RenderAccess::Read, RenderTargetLoadOp::Load, RenderTargetLoadOp::Load);
		if (texSkybox)
			skyboxPass.readTexture(*texSkybox);
		skyboxPass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), *_camera);

				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearColor(Color4f{0.1f, 0.2f, 0.3f, 1.0f});
				clearValue->setClearDepth(1.0f);

				SamplerState s = {};
				#if 1
				s.minFliter = SamplerFilter::Nearest;
				s.magFliter = SamplerFilter::Nearest;
				#endif // 0

				auto& mtl = _mtlSkybox;
				mtl->setParam("skybox", _texDefaultSkybox, s);
				
				//mtl->setParam("skybox", texSkybox ? texSkybox->textureCube() : _texDefaultSkybox);
				mtl->setParam("skybox", _texCubeEnvMap);
				//mtl->setParam("skybox", _texCubeIrradianceEnvMap);
				//mtl->setParam("skybox", _texCubePrefilteredEnvMap);
				rdReq.setMaterialCommonParams(mtl, Mat4f::s_identity());
				rdReq.drawMesh(RDS_SRCLOC, meshAssets.box, mtl);
			});

		return texColor;
	}

	RdgTextureHnd testPbr(RenderGraph* outRdGraph, RdgTextureHnd* outTexDepth, RdgTextureHnd* outTexEnvIrrdianceMap = nullptr)
	{
		auto*	rdGraph		= outRdGraph;
		auto*	rdCtx		= _rdCtx;
		auto	screenSize	= Vec2u::s_cast(rdCtx->framebufferSize()).toTuple2();

		auto* mtl = _mtlPbr.ptr();
		mtl = _mtlPbrIbl.ptr();

		RdgTextureHnd texPbrColor	= rdGraph->createTexture("pbr_color",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource});
		RdgTextureHnd texPbrDepth	= rdGraph->createTexture("pbr_depth",	Texture2D_CreateDesc{ screenSize, ColorType::Depth, TextureUsageFlags::DepthStencil | TextureUsageFlags::ShaderResource});

		auto& passPbr = rdGraph->addPass("pbrIbl", RdgPassTypeFlags::Graphics);
		passPbr.setRenderTarget(texPbrColor,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		passPbr.setDepthStencil(texPbrDepth,	RdgAccess::Write, RenderTargetLoadOp::Clear, RenderTargetLoadOp::Clear);	// currently use the pre-pass will cause z-flight
		passPbr.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), *_camera);

				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearColor(Color4f{0.1f, 0.2f, 0.3f, 1.0f});
				clearValue->setClearDepth(1.0f);

				#if 1

				static Tuple3f posLight		= Tuple3f{0.0f, 20.0f, 0.0f};

				static Tuple3f colorLight	= Tuple3f{0.8f, 0.2f, 0.1f};
				static Tuple3f color		= Tuple3f{0.2f, 0.4f, 0.6f};
				static Tuple3f colorSpec	= Tuple3f{0.2f, 0.4f, 0.6f};
				static float   roughness	= 0.0f;
				static float   ao			= 0.2f;
				static float   albedo		= 0.2f;
				static float   metallic		= 1.0f;

				Function<void(Material*, int i)> fn  

					= [&](Material* mtl, int i)
					{
						mtl->setParam("brdfLut",			_texBrdfLut);
						mtl->setParam("irradianceEnvMap",	_texCubeIrradianceEnvMap);
						mtl->setParam("prefilteredEnvMap",	_texCubePrefilteredEnvMap);

						{
							{
								//ImGui::Begin("test pbr");

								auto dragFloat3 = [](Material* mtl, const char* name, float* v)
									{
										Tuple3f temp {v[0], v[1], v[2]};
										//ImGui::DragFloat3(name, v, 0.01f);
										mtl->setParam(name, temp);
										//reinCast<Tuple3f&>(*v) = temp;
									};

								auto dragFloat = [](Material* mtl, const char* name, float* v)
									{
										float temp  = *v;
										//ImGui::DragFloat(name, v, 0.01f);
										mtl->setParam(name, temp);
										//reinCast<float&>(*v) = temp;
									};

								dragFloat3	(mtl, "posView",	&constCast(_camera->pos()).x);
								dragFloat3	(mtl, "posLight",	posLight.data);
								dragFloat3	(mtl, "colorLight",	colorLight.data);
								dragFloat3	(mtl, "color",		color.data);
								dragFloat3	(mtl, "colorSpec",	colorSpec.data);
								dragFloat	(mtl, "roughness",	&roughness);
								dragFloat	(mtl, "ao",			&ao);
								dragFloat	(mtl, "albedo",		&albedo);
								dragFloat	(mtl, "metallic",	&metallic);

								//ImGui::End();
							}
						}
					};

				if (false)
				{
					Mat4f matModel = Mat4f::s_translate(Vec3f{posLight});
					rdReq.drawMesh(RDS_SRCLOC, meshAssets.sphere, _testMtl, matModel);
				}

				#else

				Function<void(Material*, int i)> fn
					= [&](Material* mtl, int i)
					{

					};

				#endif // 0

				scene()->drawScene(rdReq, mtl, &fn);
			}
		);

		if (outTexDepth)
		{
			*outTexDepth = texPbrDepth;
		}

		if (outTexEnvIrrdianceMap)
		{
			*outTexEnvIrrdianceMap = {};
		}

		return texPbrColor;
	}
	
	RdgTextureHnd testBindless(RenderGraph* outRdGraph, RdgTextureHnd* outTexDepth)
	{
		auto*	rdGraph		= outRdGraph;
		auto*	rdCtx		= _rdCtx;
		auto	screenSize	= Vec2u::s_cast(rdCtx->framebufferSize()).toTuple2();

		auto* mtl = _mtlTestBindless.ptr();

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

				Function<void(Material*, int i)> fn  

					= [&](Material* mtl, int i)
					{

						SamplerState s = {};
						#if 1
						s.minFliter = SamplerFilter::Nearest;
						s.magFliter = SamplerFilter::Nearest;
						#endif // 0

						mtl->setParam("texture0",		_uvCheckerTex, s);
						mtl->setParam("testBuffer",		_testBindlessBuffer);
						mtl->setParam("testRwBuffer",	_testBindlessRwBuffer);
						
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

	void finalComposite(RenderGraph* outRdGraph, RdgTextureHnd presentTex)
	{
		auto*	rdGraph			= outRdGraph;
		auto	backBufferRt	= rdGraph->importTexture("back_buffer", _rdCtx->backBuffer()); RDS_UNUSED(backBufferRt);

		auto& finalComposePass = _rdGraph.addPass("final_composite", RdgPassTypeFlags::Graphics);
		finalComposePass.readTexture(presentTex);
		finalComposePass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				_presentMtl->setParam("texture0",			presentTex.texture2D());
				//_presentMtl->setParam("texture0",			_uvCheckerTex);

				_presentMtl->setParam("rds_matrix_model",	Mat4f::s_scale(Vec3f{1.0f, 1.0f, 1.0f}));
			}
		);
	}

	void present(RenderContext* rdCtx, RenderRequest& rdReq, TransferRequest& tsfReq, bool isPresent = true)
	{
		RDS_TODO("the rdReq here must be framed, otherwise some rsc will be deleted while executing");

		// _rdReq should be per frame, but all the present resources are higher lifetime scope
		rdReq.reset(rdCtx);
		if (isPresent)
		{
			auto* clearValue = rdReq.clearFramebuffers();
			clearValue->setClearColor(Color4f{0.1f, 0.2f, 0.3f, 1.0f});
			clearValue->setClearDepth(1.0f);

			rdReq.drawMesh(RDS_SRCLOC, _fullScreenTriangle, _presentMtl, Mat4f::s_identity());
			rdReq.swapBuffers();
		}
		RDS_TODO("move to endRender when upload buffer is cpu prefered");

		rdCtx->drawUI(rdReq);		
		tsfReq.commit();

		rdCtx->commit(rdReq);
	}

	void dump(StrView filename = "debug/render_graph")
	{
		_rdGraph.dumpGraphviz(filename);
	}

	void drawUi()
	{
		// test bindless
		{
			static float   rds_test_1	= {};
			static float   rds_test_3	= {};
			static float   rds_test_5	= {};

			auto* mtl = _mtlTestBindless.ptr();

			{
				auto dragFloat3 = [&](Material* mtl, const char* name, float* v)
					{
						Tuple3f temp {v[0], v[1], v[2]};
						ImGui::DragFloat3(name, v);
						mtl->setParam(name, temp);
						//reinCast<Tuple3f&>(*v) = temp;
					};

				auto dragAndUploadFloat4 = [&](Material* mtl, const char* name, float* v)
					{
						Tuple4f temp {v[0], v[1], v[2], v[4]};
						ImGui::DragFloat4(name, v, 0.01f);
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

				static Vector<u8, 256> data;
				data.resize(sizeof(TestBindlessBuffer) * RDS_TEST_BUFFER_ELEMENT_COUNT);
				static CallOnce co;
				co.callOnce([&]()
					{
						for (size_t i = 0; i < RDS_TEST_BUFFER_ELEMENT_COUNT; i++)
						{
							auto buf = reinCast<TestBindlessBuffer*>(data.data()) + i;
							buf->color.r = 1.0f;
							buf->color.g = 1.0f;
							buf->color.b = 1.0f;
							buf->color.a = 1.0f;
						}
						
					});

				ImGui::Begin("test bindless");
				// 
				//dragFloat(mtl, "rds_test_1", &rds_test_1);
				//dragFloat(mtl, "rds_test_3", &rds_test_3);
				//dragFloat(mtl, "rds_test_5", &rds_test_5);

				for (size_t i = 0; i < RDS_TEST_BUFFER_ELEMENT_COUNT; i++)
				{
					dragAndUploadFloat4(mtl, fmtAs_T<TempString>("color-{}", i).c_str(), (float*)((TestBindlessBuffer*)data.data() + i));
				}
				mtl->setParam("testBuffer", _testBindlessBuffer);
				_testBindlessBuffer->uploadToGpu(data);
				//_testBindlessRwBuffer->uploadToGpu(data);

				ImGui::End();

				_testBindlessBuffer->setDebugName("_testBindlessBuffer");
			}
		}
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

	SPtr<Shader>		_shaderPbrIbl;
	SPtr<Material>		_mtlPbrIbl;

	SPtr<Shader>		_shaderHdrToCube;
	SPtr<Shader>		_shaderIrradianceEnvCube;
	SPtr<Shader>		_shaderPrefilteredEnvCube;
	SPtr<Shader>		_shaderBrdfLut;

	SPtr<Texture2D>		_texHdrEnvMap;
	SPtr<Texture2D>		_texBrdfLut;
	SPtr<TextureCube>	_texCubeEnvMap;
	SPtr<TextureCube>	_texCubeIrradianceEnvMap;
	SPtr<TextureCube>	_texCubePrefilteredEnvMap;
	Vector<SPtr<Material>, TextureCube::s_kFaceCount> _mtlHdrToCubes;

	
	SPtr<Shader>			_shaderTestBindless;
	SPtr<Material>			_mtlTestBindless;
	SPtr<RenderGpuBuffer>	_testBindlessBuffer;
	SPtr<RenderGpuBuffer>	_testBindlessRwBuffer;

	SPtr<Shader>				_testComputeShader;
	SPtr<Material>				_testComputeMtl;
	SPtr<RenderGpuBuffer>		_testComputeLastFrameParticles;

	RenderMesh	_fullScreenTriangle;

	RenderRequest _rdReq;
};

}