#include "rds_render_api_layer-pch.h"
#include "rdsRenderGraphDrawer.h"
#include "rdsRenderGraph.h"

#define scopedBracket()		RDS_UNIQUE_VAR(ScopedBracket)(this)
#define scopedLine()		RDS_UNIQUE_VAR(ScopedLine)(this)

namespace rds
{

class RdgDrawer::ScopedBracket
{
public:
	ScopedBracket(RdgDrawer* drawer)
		: _drawer(drawer)
	{
		_drawer->writeLine("{{");
		_drawer->newLine();
		_drawer->_indentCount++;
	}

	~ScopedBracket()
	{
		_drawer->_indentCount--;
		_drawer->newLine();
		_drawer->write("}}");
	}

private:
	RdgDrawer* _drawer = nullptr;
};

class RdgDrawer::ScopedLine
{
public:
	ScopedLine(RdgDrawer* drawer)
		: _drawer(drawer)
	{
		_drawer->newLine();
		//_drawer->indent();
	}

	~ScopedLine()
	{
		//_drawer->newLine();
	}

private:
	RdgDrawer* _drawer = nullptr;
};

#if 0
#pragma mark --- rdsRdgDrawer-Impl ---
#endif // 0
#if 1

void 
RdgDrawer::reset(StrView filename, RenderGraph* graph)
{
	static constexpr SizeType s_kTempSize = 2048;

	_filename = filename;
	_rdgGraph = graph;

	_buf.clear();
	_buf.reserve(2048);
}

void 
RdgDrawer::flush()
{
	const auto& filename	= _filename;
	const auto& name		= _rdgGraph->_name;

	TempString filenameBuf;
	if (Path::isDirectory(filename))
	{
		auto dir = Path::realpath(filename);
		fmtTo(filenameBuf, "{}/{}.dot", dir, name);
	}
	else
	{
		fmtTo(filenameBuf, "{}.dot", filename, name);
	}

	File::writeFile(filenameBuf, _buf, true);
}

void 
RdgDrawer::dump(StrView filename, RenderGraph* graph)
{
	RDS_TODO("change the information vertical line to horizontal line");
	RDS_TODO("resource state and follow execution order");

	reset(filename, graph);

	const auto& graphName = graph->_name;

	StrView passName				= "pass";
	StrView resourceName			= "resource";
	StrView importedResourceName	= "resource_imported";
	StrView exportedResourceName	= "resource_exported";

	{
		writeLine("# visualize in https://dreampuf.github.io/GraphvizOnline/");
		writeLine("digraph \"{}\"", graphName);
		scopedBracket();
		write		("graph	[style = invis,	rankdir = \"{}\"	ordering = out,		splines = spline]",			style().rankDir);
		writeLine	("node	[shape = record, fontname = \"{}\",	fontsize= \"{}\",	margin = \"0.2, 0.03\"]",	style().font.name, style().font.size);

		_dumpPasses();
		_dumpResources();
		_dumpWriteEdge();
		_dumpReadEdge();
		_dumpCluster();
	}

	flush();
}

void 
RdgDrawer::_dumpPasses()
{
	bool		hasSideEffect = true;
	const char* sideEffectStr = hasSideEffect ? "&#x2605; " : "";
	//auto		fillColorIf	  = [&](const RdgPass* pass) { return pass-> }

	headerComment("dump passes");

	for (const auto& e : _rdgGraph->resultPasses()) 
	{
		auto id					= e->id();
		const char* passColor	= e->isCulled() ? style().color.pass.culled : style().color.pass.executed;
		//auto refCount			= 0;

		newLine();
		writePassNodeName(id);
		_write("[");
		//_write("label = < {{ <B>{}</B>}} | {{ Id: {} {} Refs: {} }}>",	e->name(), id, sideEffectStr, refCount);
		_write("label = < {{ <B>{}</B>}} | {{ Id: {} {} }}>",	e->name(), id, sideEffectStr);
		_write("style = \"rounded, filled\", fillcolor = {}",					passColor);
		_write("]");
	}
}

void RdgDrawer::_dumpResources()
{
	headerComment("dump resources");

	for (const auto& e : _rdgGraph->resources())
	{
		auto id				= e->id();
		const auto& name	= e->name();
		auto type			= e->type();
		auto fillColor		= e->isExported() ? style().color.resource.imported : style().color.resource.transient;
		//auto refCount		= 0;

		/*
		if want to have resource state, we have to save a VectorMap for each transition and its corresponding pass
		create a resource drawNode for each transition
		*/
		#if 0
		TempString state = "n/a";
		auto* rdRsc = e->renderResource();
		if (rdRsc)
		{
			state = enumStr(rdRsc->renderResourceStateFlags());
		}
		#endif // 0

		newLine();
		writeResourceNodeName(id);
		_write("[");
		//_write("label = < {{ \t<B>\"{}\"</B> <BR/>{} }} \t|\t {{ Id: {} \t Refs: {} }} > ",	name, enumStr(type), id, refCount);
		_write("label = < {{ \t<B>\"{}\"</B> <BR/>{} }} \t|\t {{ Id: {} }} > ",	name, enumStr(type), id);
		_write("style = filled, fillcolor = {}",											fillColor);
		_write("]");
	}

}

void RdgDrawer::_dumpWriteEdge()
{
	headerComment("dump write edge");

	for (const auto& e : _rdgGraph->passes()) 
	{
		auto id	= e->id();

		newLine();
		writePassNodeName(id);
		_write("->");
		{
			scopedBracket();
			for (const auto& w : e->_writes)
			{
				auto wId = w->id();
				writeResourceNodeName(wId);
			}
			writeLine("label=\"Resource State\"");
		}
		write("[color={}]", style().color.edge.write);
	}
}

void RdgDrawer::_dumpReadEdge()
{
	headerComment("dump read edge");

	for (const auto& e : _rdgGraph->resources()) 
	{
		auto id	= e->id();

		newLine();
		writeResourceNodeName(id);
		write("->");
		{
			scopedBracket();
			for (const auto& pass : _rdgGraph->passes())
			{
				// read if same
				for (auto& read : pass->_reads)
				{
					if (read->id() == id)
					{
						writePassNodeName(pass->id());
					}
				}
			}
		}
		write("[color={}]", style().color.edge.read);
	}
}

void RdgDrawer::_dumpCluster()
{

	const auto& passes		= _rdgGraph->renderGraphFrame().passes;
	const auto& resources	= _rdgGraph->renderGraphFrame().resources;

	if (style().useClusters)
	{
		headerComment("dump cluster");

		// created by this pass
		for (const auto& e : passes) 
		{
			auto id	= e->id();

			writeLine("subgraph ");
			_write("\"cluster_{}_{}\" ", s_passNodeName, id);
			{
				scopedBracket();
				writePassNodeName(id);
				//for (auto& create : e._create)
				{

				}
			}
		}

		{
			headerComment("imported resources");
			writeLine("subgraph cluster_imported_resources");
			scopedBracket();
			write("graph [style=dotted, fontname=\"helvetica\", label=< <B>Imported</B> >]");
			scopedBracket();
			for (auto& e : resources)
			{
				if (e->isImported())
				{
					writeResourceNodeName(e->id());
				}
			}
		}

		{
			headerComment("exported resources");
			writeLine("subgraph cluster_exported_resources ");
			scopedBracket();
			write("graph [style=dotted, fontname=\"helvetica\", label=< <B>Exported</B> >]");
			scopedBracket();
			for (auto& e : resources)
			{
				if (e->isExported())
				{
					writeResourceNodeName(e->id());
				}
			}
		}
	}
}

void 
RdgDrawer::writePassNodeName(RdgId id)
{
	write("{}_{} ", s_passNodeName, id); 
}

void 
RdgDrawer::writeResourceNodeName(RdgId id)
{
	write("{}_{}\t", s_resourceNodeName, id);
}

#endif

}


#undef scopedLine
#undef scopedBracket
