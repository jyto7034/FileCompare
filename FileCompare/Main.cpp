//
//		   File Compare
//   Enter the folder path
//   If nothing to compare 
//       occur the error
//

#include <windows.h>
#include <iostream>
#include <fstream>
#include <direct.h>
#include <string>
#include <vector>
#include <time.h>
#include <cstdlib>
#include <io.h>

#define BUFSIZE 256
#define DIVIDE 200000
#define SIZE(x) _msize(x) / sizeof(x)
#define _CONST(x) const_cast<char*>(x)
#define CHECK std::cout<<"CHECK"<<std::endl;
#define cout(x) std::cout<<x<<std::endl;
#define COPY(x) static char temp[BUFSIZE]; strcpy(temp, x);

static int cnt;
static int msize = 0;
bool GetFileData(const char* folderpath, const char* filename, char*& buf, int& filesize, std::vector<std::string>* filepath);

void gotoxy(int x, int y)
{
	COORD pos = { x - 1, y - 1 };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

bool Release(char* buf) {
	if (buf) return false;
	buf = nullptr;
	return true;
}

bool Release(const char** buf) {
	if (buf == nullptr)	return false;
	delete[] * buf;
	return true;
}

typedef struct FILEDATA
{
	const char* buf;
	const char* path;
	const char* name;
	int size, * code;

	FILEDATA() {
		buf = nullptr;
		path = nullptr;
		name = nullptr;
		code = nullptr;
		size = 0;
	}

	FILEDATA(const char* buf, const char* path, const char* name, int size) {

		this->path = path;
		this->buf = buf;
		this->name = name;
		this->size = size;
	}


	void ShowData(int index) {
		std::cout << std::endl << "################ " << index << " ###############" << std::endl;
		std::cout << "CodeCnt :" << _msize(this->code) << std::endl;
		std::cout << "Path :" << this->path << std::endl;
		std::cout << "Size :" << this->size << std::endl;
		std::cout << "Name :" << this->name;
		std::cout << std::endl << "###################################" << std::endl;
	}

}FILEINFO;

bool Compare(FILEDATA * file, FILEDATA * _file) {
	if (file->size == _file->size || strcmp(file->name, _file->name) == 0)
		return true;

	try {
		for (int i = 0; i < SIZE(file->code); i++) {
			if (file->code[i] == _file->code[i]) {
				cnt++;
			}
			else {
				return false;
			}
		}
	}
	catch (...) {
		return false;
	}
	return false;
}

bool MoveTo(FILEDATA SrcFile, const char* DstPath) {
	char* temp;
	std::string Path(SrcFile.path);
	std::vector<std::string> filepath;
	Path.erase(Path.end() - strlen(SrcFile.name), Path.end());
	GetFileData(Path.c_str(), SrcFile.name, temp, SrcFile.size, &filepath);
	SrcFile.buf = temp;

	std::ofstream File(DstPath, std::ios::binary);
	if (!File.is_open()) {
		return false;
	}
	File.write(SrcFile.buf, SrcFile.size);
	remove(SrcFile.path);

	Release(temp);
	Release(&SrcFile.buf);
	File.close();
	return 0;
}

int FindFiles(char path[], std::vector<std::string> & folderNames) {
	COPY(path);
	_finddata_t fd;
	long handle = _findfirst(strcat(temp, _CONST("\\*.*")), &fd);
	int result = 1;

	if (handle == -1)
		return -1;
	while (result != -1)
	{
		folderNames.push_back(fd.name);
		result = _findnext(handle, &fd);
	}
	_findclose(handle);
	Release(temp);
	return folderNames.size();
}


bool GetFileData(const char* folderpath, const char* filename, char*& buf, int& filesize, std::vector<std::string> * filepath) {
	char _filepath[BUFSIZE] = { 0 };
	COPY(folderpath);
	strcat(strcpy(_filepath, strcat(_CONST(temp), "\\")), filename);
	filepath->push_back(_filepath);
	std::ifstream file(_filepath, std::ios::binary);
	if (!file.is_open())
		return false;

	file.seekg(0, std::ios::end);
	int size = filesize = file.tellg();

	buf = new char[size];
	file.seekg(0, std::ios::beg);

	file.read(buf, size);

	Release(temp);
	Release(_filepath);

	file.close();
	return true;
}


void SelectCode(FILEINFO * file) {
	srand((unsigned int)time(NULL));
	int codeCnt = 0;

	if ((file->size / DIVIDE) == 0) {
		codeCnt = file->size / 15000;
		file->code = new int[codeCnt];
	}

	else {
		codeCnt = file->size / DIVIDE;
		file->code = new int[codeCnt];
	}

	for (int i = 0; i < codeCnt; i++) {
		file->code[i] = (int)file->buf[rand() % file->size - 1];
	}
}


bool Compare(std::vector<FILEINFO> *file, std::vector<FILEINFO> *overlapPaths) {
	for (int i = 0; i < file->size(); i++) {
		for (int j = 0; j < file->size(); j++) {
			if (Compare(&file->at(i), &file->at(j)) && i != j) {
				std::cout << file->at(i).name << " - " << file->at(j).name << std::endl;
				std::cout << i << " : " << j << std::endl;
				overlapPaths->push_back(file->at(i));
			}
		}
	}
	return true;
};


void DeleteDuplicate(std::vector<FILEINFO> overlapPaths, const char* Path) {
	FILEINFO _temp;
	mkdir(Path);


	for (int i = 0; i < overlapPaths.size() - 1; i++) {
		for (int j = 0; j < overlapPaths.size() - i - 1; j++) {
			if (overlapPaths[j].size > overlapPaths[j + 1].size) {
				_temp = overlapPaths[j];
				overlapPaths[j] = overlapPaths[j + 1];
				overlapPaths[j + 1] = _temp;
			}
		}
	}

	for (int i = 0; i < overlapPaths.size(); i++) {
		std::cout << overlapPaths[i].name << std::endl;
	}

	for (int j = 0; j < overlapPaths.size(); j++) {
		if (j != overlapPaths.size() - 1) {
			if (overlapPaths[j].size == overlapPaths[j + 1].size) {
				COPY(Path);
				strcat(strcat(temp, "\\"), overlapPaths[j].name);
				if (MoveTo(overlapPaths[j], temp)) {
					std::cout << "move failed : " << overlapPaths[j].path << std::endl;
				}
				else {
					std::cout << "Move " << overlapPaths[j].path << " to " << Path;
				}
			}
		}
	}
}

int main() {
	char* buf, folderpath[256] = { "D:\\t" }, binPath[256] = { "D:\\bin" };
	int filesize, filecnt;
	std::vector<std::string> fileNames;
	std::vector<std::string> filepath;
	std::vector<FILEINFO> fileinfos;
	std::vector<FILEINFO> overlapPaths;
	//std::cout << "folder path :";
	//std::cin.getline(folderpath, 256);
	//std::cout << std::endl << "bin folder path :";
	//std::cin.getline(binPath, 256);
	filecnt = FindFiles(folderpath, fileNames);
	for (int i = 0; i < filecnt; i++) {
		if (GetFileData(folderpath, fileNames[i].c_str(), buf, filesize, &filepath)) {
			FILEINFO file(buf, filepath[i].c_str(), fileNames[i].c_str(), filesize);
			SelectCode(&file);
			fileinfos.push_back(file);
			file.ShowData(i);
			Release(&file.buf);
		}
	}
	Compare(&fileinfos, &overlapPaths);
	DeleteDuplicate(overlapPaths, binPath);
	return 0;
}