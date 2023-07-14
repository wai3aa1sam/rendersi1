#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsVertex.h"

namespace rds
{

#if 0
#pragma mark --- rdsVertexLayoutManager-Decl ---
#endif // 0
#if 1

class VertexLayoutManager : public StackSingleton<VertexLayoutManager>
{
public:
	//static VertexLayoutManager* instance();

public:
	VertexLayoutManager();
	~VertexLayoutManager();
	
	const VertexLayout* get(const VertexType& t);

	template<class VERTEX> const VertexLayout* registerLayout();

protected:
	const VertexLayout* find(const VertexType& t) const;

protected:
	VertexLayout* _createLayout(const VertexType& t);

private:
	Map<VertexType, VertexLayout> _layoutTable;
};

template<class VERTEX> inline
const VertexLayout* 
VertexLayoutManager::registerLayout()
{
	auto* p = _createLayout(VERTEX::s_kVertexType);
	VERTEX::_internal_onRegister(p);
	p->_vertexType	= VERTEX::s_kVertexType;
	p->_stride		= sizeof(VERTEX);
	return p;
}



#endif
}