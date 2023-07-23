#include "rds_render_api_layer-pch.h"
#include "rdsTransferRequest.h"

namespace rds
{

TransferRequest::TransferRequest()
{
	_transferCommandBuffers.resize(s_kThreadCount);
}

TransferRequest::~TransferRequest()
{

}

void 
TransferRequest::clear()
{
	for (auto& e : _transferCommandBuffers)
	{
		e.clear();
	}
}



}