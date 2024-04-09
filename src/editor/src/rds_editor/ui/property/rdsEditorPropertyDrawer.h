#pragma once

#include "rds_editor/common/rds_editor_common.h"

/*
references:
~ EditorPropertyDrawer.h in		https://github.com/SimpleTalkCpp/SimpleGameEngine/tree/main
~ SceneHierarchyPanel.cpp in	https://github.com/TheCherno/Hazel/blob/master/Hazelnut/src/Panels/SceneHierarchyPanel.cpp
*/

namespace rds
{

class EditorUiDrawRequest;
class EditorPropertyDrawRequest;


#if 0
#pragma mark --- rdsEditorPropertyDrawer-Decl ---
#endif // 0
#if 1

class EditorPropertyDrawer : public NonCopyable
{
public:
	using DrawRequest = EditorPropertyDrawRequest;

public:
	/* return: hasValueChanged */
	virtual bool draw(DrawRequest* drawReq);

	template<class T>
	bool draw(DrawRequest* drawReq, StrView label, T* o, const T& resetValue)
	{
		drawReq->resetValue = &resetValue;
		return draw(drawReq, label, o);
	}

	template<class T>
	bool draw(DrawRequest* drawReq, StrView label, T* o)
	{
		drawReq->value		= o;
		drawReq->label		= label;
		return draw(drawReq);
	}

	bool draw(DrawRequest* drawReq, StrView label, void* o);
};

template<class T>
class EditorPropertyDrawerT : public EditorPropertyDrawer
{
public:
	using ValueT = T;
	
public:
	ValueT& value(		DrawRequest* drawReq) { return *reinCast<ValueT*>(drawReq->value); }
	ValueT	resetValue(	DrawRequest* drawReq) { return drawReq->resetValue ? *reinCast<const ValueT*>(drawReq->resetValue) : ValueT{}; }
};

template<>
class EditorPropertyDrawerT<void> : public EditorPropertyDrawer
{
public:

};

#endif

#if 0
#pragma mark --- rdsEditorPropertyDrawer_-Decl ---
#endif // 0
#if 1

class EditorPropertyDrawer_struct : public EditorPropertyDrawerT<void>
{
public:
	virtual bool draw(DrawRequest* drawReq) override;
};

class EditorPropertyDrawer_float : public EditorPropertyDrawerT<float>
{
public:
	virtual bool draw(DrawRequest* drawReq) override;
};

class EditorPropertyDrawer_i32 : public EditorPropertyDrawerT<i32>
{
public:
	virtual bool draw(DrawRequest* drawReq) override;
};

class EditorPropertyDrawer_u64 : public EditorPropertyDrawerT<u64>
{
public:
	virtual bool draw(DrawRequest* drawReq) override;
};

class EditorPropertyDrawer_bool : public EditorPropertyDrawerT<bool>
{
public:
	virtual bool draw(DrawRequest* drawReq) override;
};

class EditorPropertyDrawer_Vec2f : public EditorPropertyDrawerT<Vec2f>
{
public:
	virtual bool draw(DrawRequest* drawReq) override;
};

class EditorPropertyDrawer_Vec3f : public EditorPropertyDrawerT<Vec3f>
{
public:
	virtual bool draw(DrawRequest* drawReq) override;
};

class EditorPropertyDrawer_Vec4f : public EditorPropertyDrawerT<Vec4f>
{
public:
	virtual bool draw(DrawRequest* drawReq) override;
};

class EditorPropertyDrawer_Color4f : public EditorPropertyDrawerT<Color4f>
{
public:
	virtual bool draw(DrawRequest* drawReq) override;
};

class EditorPropertyDrawer_Texture2D : public EditorPropertyDrawerT<Texture2D*>
{
public:
	virtual bool draw(DrawRequest* drawReq) override;
};



#endif
}