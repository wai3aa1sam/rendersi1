#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsRenderGraphResource.h"

/*
references:
~ dump graphviz
	- https://github.com/skaarj1989/FrameGraph
*/

namespace rds
{

class RenderGraph;

#if 0
#pragma mark --- rdsRdgDrawer-Decl ---
#endif // 0
#if 1

// for dumping graphviz
// visualization in https://dreampuf.github.io/GraphvizOnline/
class RdgDrawer
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
	struct StyleSheet 
	{
		bool useClusters = true;
		const char* rankDir = "TB"; // TB, LR, BT, RL

		struct 
		{
			const char* name	= "helvetica";
			int size			= 10;
		} font;
		struct 
		{
			// https://graphviz.org/doc/info/colors.html
			struct 
			{
				const char* executed	= "orange";
				const char* culled		= "lightgray";
			} pass;
			struct 
			{
				const char* imported	= "lightsteelblue";
				const char* transient	= "skyblue";
			} resource;
			struct 
			{
				const char* read		= "olivedrab3";
				const char* write		= "orangered";
			} edge;
		} color;
	};

public:
	static constexpr StrView s_passNodeName		= "pass";
	static constexpr StrView s_resourceNodeName = "resource";

public:
	void dump(StrView filename, RenderGraph* graph);

	const StyleSheet& style() const;

protected:
	void writePassNodeName		(RdgId id);
	void writeResourceNodeName	(RdgId id);

protected:
	void reset(StrView filename, RenderGraph* graph);
	void flush();
	
	class ScopedBracket;
	class ScopedLine;

	void indent();
	void newLine(SizeType n = 1);

	template<class... ARGS> void write		(const char* fmt, ARGS&&... args);		// indent
	template<class... ARGS> void writeLine	(const char* fmt, ARGS&&... args);

	template<class... ARGS> void headerComment	(const char* fmt, ARGS&&... args);
	template<class... ARGS> void comment		(const char* fmt, ARGS&&... args);

protected:
	void _dumpPasses();
	void _dumpResources();
	void _dumpWriteEdge();
	void _dumpReadEdge();
	void _dumpCluster();

	template<class... ARGS> void _write		(const char* fmt, ARGS&&... args);		// no indent
	template<class... ARGS> void _writeLine	(const char* fmt, ARGS&&... args);

protected:
	String			_filename;
	RenderGraph*	_rdgGraph = nullptr;
	
	String			_buf;
	SizeType _indentCount = 0;
};

inline
const RdgDrawer::StyleSheet& 
RdgDrawer::style() const
{
	static StyleSheet s_style;
	return s_style;
}

inline
void 
RdgDrawer::indent()
{
	for (size_t i = 0; i < _indentCount; i++)
	{
		_buf += "\t";
	}
}

inline
void 
RdgDrawer::newLine(SizeType n)
{
	for (size_t i = 0; i < n; i++)
	{
		_buf += "\n";
	}
}

template<class... ARGS> inline
void 
RdgDrawer::write(const char* fmt, ARGS&&... args)
{
	indent();
	_write(fmt, rds::forward<ARGS>(args)...);
}

template<class... ARGS> inline
void 
RdgDrawer::writeLine(const char* fmt, ARGS&&... args)
{
	newLine();
	indent();
	_write(fmt, rds::forward<ARGS>(args)...);
}

template<class... ARGS> inline
void 
RdgDrawer::headerComment(const char* fmt, ARGS&&... args)
{
	newLine();
	newLine();
	comment(fmt, rds::forward<ARGS>(args)...);
}

template<class... ARGS> inline
void 
RdgDrawer::comment(const char* fmt, ARGS&&... args)
{
	indent();
	_buf += "# ";
	_write(fmt, rds::forward<ARGS>(args)...);
}

template<class... ARGS> inline
void 
RdgDrawer::_write(const char* fmt, ARGS&&... args)
{
	fmtTo(_buf, fmt, rds::forward<ARGS>(args)...);
}

template<class... ARGS> inline
void 
RdgDrawer::_writeLine(const char* fmt, ARGS&&... args)
{
	newLine();
	_write(fmt, rds::forward<ARGS>(args)...);
}


#endif

}