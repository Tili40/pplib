#include <classes.hpp>

AnsiString ppGetFileList(AnsiString path){
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	hFind = FindFirstFile(path.c_str(), &ffd);
	if(hFind == INVALID_HANDLE_VALUE)
		return "";
	TStringList * str = new TStringList();
	do{
		if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			str->Add(ffd.cFileName);
	}while (FindNextFile(hFind, &ffd) != 0);
	FindClose(hFind);
	AnsiString result = str->Text;
	delete str;
	return result;
}

int ppFileExists(AnsiString fname){
	WIN32_FIND_DATA FindFileData;
	HANDLE handle = FindFirstFile(fname.c_str(), &FindFileData) ;
	int found = handle != INVALID_HANDLE_VALUE;
	if(found)
		FindClose(handle);
	return found;
}