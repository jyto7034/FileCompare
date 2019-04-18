#include <iostream>
#include <fstream>
#include <vector>
#include <windows.h>
#include <time.h>
#include <cstdlib>
#include <io.h>

#define BUFSIZE 256
#define DIVIDE 200000
#define SIZE(x) _msize(x) / sizeof(x)
#define COPY(x) static char temp[BUFSIZE]; strcpy(temp, x);
#define _CONST(x) const_cast<char*>(x)
#define CHECK std::cout<<"CHECK"<<std::endl;


typedef struct FILEDATA
{
	const char *buf;
	const char *path;
	const char *name;
	int size, *code;

	FILEDATA() {}

	FILEDATA(const char *buf, const char *path, const char *name, int size) {
		this->path = path;
		this->buf = buf;
		this->name = name;
		this->size = size;
	}


	void ShowData(int index) {
		std::cout << std::endl << "################ " << index << " ################" << std::endl;
		std::cout << "Path :" << this->path << std::endl;
		std::cout << "Size :" << this->size << std::endl;
		std::cout << "Name :" << this->name;
		std::cout << std::endl << "###################################" << std::endl;
	}

	void Release() {
		if (this->code == nullptr) return;
		delete[] this->code;
	}

	static bool Compare(FILEDATA file, FILEDATA _file) {
		if (strcmp(file.name, _file.name) == 0 && file.size == _file.size) {
			return true;
		}
		else {
			for (int i = 0; i < SIZE(file.code); i++) {
				if (file.buf[file.code[i]] != _file.buf[_file.code[i]]) {
					return false;
				}
			}
			return true;
		}

		return true;
	}
}FILEINFO;


int FindFiles(char path[], std::vector<std::string> &folderNames) {
	COPY(path);
	_finddata_t fd;
	long handle = _findfirst(strcat(path, _CONST("\\*.*")), &fd);
	strcpy(path, temp);
	int result = 1;

	if (handle == -1)
		return -1;
	while (result != -1)
	{
		folderNames.push_back(fd.name);
		result = _findnext(handle, &fd);
	}
	_findclose(handle);
	return folderNames.size();
}


bool Getfile(const char *folderpath, const char *filename, char *&buf, int &filesize, char *&filepath) {
	static char _filepath[BUFSIZE] = { 0 };
	COPY(folderpath);
	strcat(strcpy(_filepath, strcat(_CONST(temp), "\\")), filename);
	filepath = _filepath;
	std::ifstream file(_filepath, std::ios::binary);
	if (!file.is_open())
		return false;

	file.seekg(0, std::ios::end);
	int size = filesize = file.tellg();

	buf = new char[size];
	file.seekg(0, std::ios::beg);

	file.read(buf, size);
	return true;
}


void SelectCode(FILEINFO *file) {
	srand((unsigned int)time(NULL));
	int codeCnt;
	try {
		codeCnt = file->size / DIVIDE;
		file->code = new int[codeCnt];
	}
	catch (...) {
		codeCnt = file->size / (DIVIDE - 180000);
		file->code = new int[codeCnt];
	}

	for (int i = 0; i < codeCnt; i++) {
		file->code[i] = rand() % file->size;
		//std::cout << file->code[i] << std::endl;
	}
}


bool Compare(std::vector<FILEINFO> file, std::vector<FILEINFO> *overlapPaths) {
	for (int i = 0; i < file.size(); i++) {
		for (int j = 0; j < file.size(); j++) {
			if (FILEINFO::Compare(file[i], file[j]) && i != j) {
				std::cout << file[j].name << std::endl;
				std::cout << i << " : " << j << std::endl;
				overlapPaths->push_back(file[i]);
			}
		}
	}
	return true;
}


void DeleteDuplicate(std::vector<FILEINFO> overlapPaths) {
	int temp;
	for (int i = 0; i < overlapPaths.size() - 1; i++) {
		for (int j = 0; j < overlapPaths.size() - i - 1; j++) {
			if (overlapPaths[j].size > overlapPaths[j + 1].size) {
				temp = overlapPaths[j].size;
				overlapPaths[j].size = overlapPaths[j + 1].size;
				overlapPaths[j + 1].size = temp;
			}
		}
	}
	for (int i = 0; i < overlapPaths.size(); i++) {
		for (int j = 0; j < overlapPaths.size() - i; j++) {
			if (j < overlapPaths.size() && overlapPaths[j].size == overlapPaths[j + 1].size) {
				overlapPaths.erase(overlapPaths.begin() + j + 1);
			}
		}
	}

	for (int i = 0; i < overlapPaths.size(); i++) {
		std::cout << " : " << overlapPaths[i].name << std::endl;
	}
}


void Clear(std::vector<FILEINFO> *fileinfos, char **buf) {
	for (int i = 0; i < fileinfos->size(); i++) {
		fileinfos->at(i).Release();
	}
	if (*buf == nullptr) return;
	delete[] * buf;
}

int main() {
	char *buf, folderpath[256] = { 0 }, *filepath;
	int filesize, filecnt;
	std::vector<std::string> fileNames;
	std::vector<FILEINFO> fileinfos;
	std::vector<FILEINFO> overlapPaths;
	std::cout << "folder path :";
	std::cin.getline(folderpath, 256);
	filecnt = FindFiles(folderpath, fileNames);
	for (int i = 0; i < filecnt; i++) {
		if (Getfile(folderpath, fileNames[i].c_str(), buf, filesize, filepath)) {
			FILEINFO file(buf, filepath, fileNames[i].c_str(), filesize);
			SelectCode(&file);
			fileinfos.push_back(file);
			file.ShowData(i);
		}
	}

	Compare(fileinfos, &overlapPaths);
	DeleteDuplicate(overlapPaths);
	Clear(&fileinfos, &buf);
	for (;;);
	return 0;
}