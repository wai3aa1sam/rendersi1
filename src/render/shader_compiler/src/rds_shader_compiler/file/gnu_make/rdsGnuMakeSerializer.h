#pragma once

#include "rds_shader_compiler/common/rds_shader_compiler_common.h"

namespace rds
{

#if 0
#pragma mark --- rdsGnuMakeSerializer-Decl ---
#endif // 0
#if 1

class GnuMakeSerializer : public NonCopyable
{
public:
	struct RequestBase;
	using Request = RequestBase;
	
public:
	template<class STR> static void			getVariableTo(STR& str, const char* name)	{ str.clear(); str.append("$("); str.append(name); str.append(")"); }
						static TempString	toVariable(const char* name)				{ TempString tmp; getVariableTo(tmp, name); return tmp.c_str(); }
	
protected:
	GnuMakeSerializer(Request& req);

	void phony(InitList<StrView> names);
	void assignVariable(StrView var, StrView assign, StrView value);
	void includePath(StrView path);

	void write(				StrView sv);
	void onWrite(			StrView sv);
	void onWriteVariable(	StrView sv);

	void writeLine(			StrView sv);
	void writeVariable(		StrView sv);
	void nextLine();

	void flush(StrView filename);

protected:

	#if 0
	#pragma mark --- rdsGnuMakeSerializer::SpecialBase-Impl ---
	#endif // 0
	#if 1

	struct SpecialBase
	{
		Request* request = nullptr;

		SpecialBase(Request& request_)
		{
			request = &request_;
		}

		void write(StrView sv)				{ request->write(sv); }
		//void onWrite(StrView sv)			{ request->onWrite(sv); }
		//void onWriteVariable(StrView sv)	{ request->onWriteVariable(sv); }

		void nextLine()					{ request->nextLine(); }
		void writeLine(StrView sv)		{ request->writeLine(sv); }
		void writeVariable(StrView sv)	{ request->writeVariable(sv); }

		void setOnWriteStr(StrView sv)	{ clearOnWriteStr(); request->setOnWriteStr(sv); }
		void clearOnWriteStr()			{ request->clearOnWriteStr(); }
	};

	struct Ifeq : public SpecialBase
	{
		using Base = SpecialBase;
		RDS_NODISCARD static Ifeq ctor(Request& request_, StrView val0, StrView val1) { return Ifeq(request_, val0, val1); }

		~Ifeq()
		{
			clearOnWriteStr();
			nextLine();
			writeLine("endif");
		}

	private:
		Ifeq(Request& request_, StrView var, StrView val)
			: Base(request_)
		{
			request = &request_;
			write("ifeq (");
			writeVariable(var); write(", "); write(val);  write(")");
		}
	};

	struct Target : public SpecialBase
	{
		using Base = SpecialBase;

		RDS_NODISCARD static Target ctor(Request& request_, StrView target, InitList<StrView> deps)		{ return Target(request_, target, deps); }
		RDS_NODISCARD static Target ctor(Request& request_, StrView target)								{ return Target(request_, target); }

		~Target()
		{
			clearOnWriteStr();
		}
	private:
		Target(Request& request_, StrView target, InitList<StrView> deps)
			: Base(request_)
		{
			init(request_, target);
			for (size_t i = 0; i < deps.size(); i++)
			{
				write(" "); write(deps[i]);
			}
			nextLine();
			setOnWriteStr("\t");
		}

		Target(Request& request_, StrView target)
			: Base(request_)
		{
			init(request_, target);
			setOnWriteStr("\t");
		}

		void init(Request& request_, StrView target)
		{
			request = &request_;
			nextLine();
			write(target.data()); write(":"); 
		}
	};

	struct BeginString : public SpecialBase
	{
		using Base = SpecialBase;
		RDS_NODISCARD static BeginString ctor(Request& request_, StrView str)	{ return BeginString(request_, str); }
		RDS_NODISCARD static BeginString ctor(Request& request_)				{ return BeginString(request_); }

		~BeginString()
		{
			write("\"");
		}

	private:
		BeginString(Request& request_, StrView str)
			: Base(request_)
		{
			request = &request_;
			write("\"");
			write(str);
		}

		BeginString(Request& request_)
			: Base(request_)
		{
			request = &request_;
			write("\"");
		}
	};

	struct BeginCmd : public SpecialBase
	{
		using Base = SpecialBase;
		RDS_NODISCARD static BeginCmd ctor(Request& request_) { return BeginCmd(request_); }

		~BeginCmd()
		{
			//write(" ");
		}

	private:

		BeginCmd(Request& request_)
			: Base(request_)
		{
			request = &request_;
			write(" ");
		}
	};

	#endif

public:
	struct RequestBase
	{
		RequestBase(size_t reserveSize = 4096) { _content.reserve(reserveSize); }

		void write(StrView sv)			{  _content.append(sv.data()); }
		//void write(const char* c_str) { content.append(c_str); }

		void nextLine()					{ write("\n"); }
		void writeLine(StrView sv)		{ write(sv); nextLine(); }

		void writeVariable(StrView sv)	{ write("$("); write(sv); write(")"); }

		void onWrite(StrView sv)			
		{ 
			_content.append(_onWriteStr); 
			write(sv); 
		}

		void onWriteVariable(StrView sv)	{ _content.append(_onWriteStr); writeVariable(sv); }

		// cuurently is wrong, should use a stack
		void setOnWriteStr(StrView sv)	{ clearOnWriteStr(); _onWriteStr.assign(sv.data()); }
		void clearOnWriteStr()			{ _onWriteStr.clear(); }

		void flush(StrView filename)	 { File::writeFileIfChanged(filename, _content, false); }

	private:
		void _onWrite() { write(_onWriteStr); }

	private:
		String _onWriteStr;
		String _content;
	};

protected:
	Request* _GNUMakeGeneratorRequest = nullptr;
};


inline void GnuMakeSerializer::write(StrView sv)			{ auto& req = *_GNUMakeGeneratorRequest; req.write(sv); }
inline void GnuMakeSerializer::onWrite(StrView sv)			{ auto& req = *_GNUMakeGeneratorRequest; req.onWrite(sv); }
inline void GnuMakeSerializer::onWriteVariable(StrView sv)	{ auto& req = *_GNUMakeGeneratorRequest; req.onWriteVariable(sv); }

inline void GnuMakeSerializer::writeLine(StrView sv)		{ auto& req = *_GNUMakeGeneratorRequest; req.writeLine(sv); }
inline void GnuMakeSerializer::writeVariable(StrView sv)	{ auto& req = *_GNUMakeGeneratorRequest; req.writeVariable(sv); }
inline void GnuMakeSerializer::nextLine()					{ auto& req = *_GNUMakeGeneratorRequest; req.nextLine(); }

inline void GnuMakeSerializer::flush(StrView filename)		{ auto& req = *_GNUMakeGeneratorRequest; req.flush(filename);}


#endif

}