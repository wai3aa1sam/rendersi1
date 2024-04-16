#include "rds_shader_compiler-pch.h"
#include "rdsGnuMakeSerializer.h"


namespace rds
{

#if 0
#pragma mark --- rdsGnuMakeSerializer-Impl ---
#endif // 0
#if 1

GnuMakeSerializer::GnuMakeSerializer(Request& req)
{
	_GNUMakeGeneratorRequest = &req;
}

void 
GnuMakeSerializer::phony(InitList<StrView> names)
{
	nextLine();
	write(".PHONY:");
	for (size_t i = 0; i < names.size(); i++)
	{
		write(" "); write(names[i]);
	}
	nextLine();
}

void 
GnuMakeSerializer::assignVariable(StrView var, StrView assign, StrView value) 
{  
	bool isAssignValid = !(assign.compare(":=") && assign.compare("=")); (void)isAssignValid;
	RDS_CORE_ASSERT(isAssignValid);

	nextLine();
	write(var); 
	write(assign); write(value); 
	nextLine();
}

void 
GnuMakeSerializer::includePath(StrView path)
{
	//RDS_ASSERT(Directory::exists(path) || FilePath::exists(path));	// should use map to deal with $(var) value
	nextLine();
	write("-include "); write(path.data()); 
	//nextLine();
}

#endif


}