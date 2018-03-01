#include "lib7zip.h"

#ifdef S_OK
#undef S_OK
#endif

#include "CPP/7zip/Archive/IArchive.h"
#include "CPP/Windows/PropVariant.h"
#include "CPP/Common/MyCom.h"
#include "CPP/7zip/ICoder.h"
#include "CPP/7zip/IPassword.h"
#include "CPP/7zip/Common/FileStreams.h"

#include "HelperFuncs.h"

class C7ZipArchiveItemImpl : public virtual C7ZipArchiveItem
{
public:
	C7ZipArchiveItemImpl(wstring fullPath, bool isDir, unsigned int nIndex);
	virtual ~C7ZipArchiveItemImpl();

public:
	virtual wstring GetFullPath() const;
	virtual bool IsDir() const;
	virtual unsigned int GetArchiveIndex() const;

private:
	wstring m_FullPath;
	bool m_bIsDir;
	unsigned int m_nIndex;
};

C7ZipArchiveItemImpl::C7ZipArchiveItemImpl(wstring fullPath, bool isDir, unsigned int nIndex) :
m_FullPath(fullPath),
m_bIsDir(isDir),
m_nIndex(nIndex)
{
}

C7ZipArchiveItemImpl::~C7ZipArchiveItemImpl()
{
}

wstring C7ZipArchiveItemImpl::GetFullPath() const
{
	return m_FullPath;
}

bool C7ZipArchiveItemImpl::IsDir() const
{
	return m_bIsDir;
}

unsigned int C7ZipArchiveItemImpl::GetArchiveIndex() const
{
	return m_nIndex;
}

bool Create7ZipArchiveItem(C7ZipArchive * pArchive, 
						   const wstring & fullpath, 
						   bool isDir, 
						   unsigned int nIndex,
						   C7ZipArchiveItem ** ppItem)
{
	*ppItem = new C7ZipArchiveItemImpl(fullpath, isDir, nIndex);

	return true;
}
