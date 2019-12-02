#pragma once
#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <vector>
#include <map>

namespace Workers {
	using Text = std::list<std::string>;
	namespace Utils {
		class TellingException : public std::exception {
			std::string expl;
		public:
			TellingException(std::string expl) : expl(expl) {}
			const char* what() const {
				return expl.data();
			}
		};
		class EofException : public std::exception {};
		class EolException : public std::exception {};
		class Worker {
		public:
			virtual void process(Text&) = 0;
		};
		class FileParser {
		public:
			virtual void openFile(const std::string& fileName) = 0;
			virtual const std::string& operator*() = 0;
			virtual const std::string* operator->() = 0;
			virtual FileParser& operator++() = 0;
			virtual bool eof() = 0;
		};
		class StringsFromFileReader : public FileParser {
			std::ifstream inputStream;
			std::string currentString;
			bool endOfFile;
			bool needToRead;
		public:
			StringsFromFileReader() = default;
			StringsFromFileReader(const std::string& fileName){
				openFile(fileName);
			}
			virtual void openFile(const std::string& fileName) {
				inputStream.exceptions(std::ifstream::eofbit | std::ifstream::failbit | std::ifstream::badbit);
				try {
					inputStream.open(fileName);
				}
				catch (std::istream::failure) {
					throw TellingException("Something wrong happened with input flow.");
				}
				endOfFile = false;
				needToRead = true;
			}
			virtual const std::string& operator*() {
				if (needToRead) {
					if (endOfFile)
						throw EofException();
					try {
						std::getline(inputStream, currentString);
					}
					catch (std::istream::failure ex) {
						if (inputStream.eof()) {
							endOfFile = true;
						}
						else {
							throw TellingException("Something wrong happened with input flow.");
						}
					}
					needToRead = false;
				}
				return currentString;
			}
			virtual const std::string* operator->() {
				return &*(*this);
			}
			virtual StringsFromFileReader& operator++() {
				if (needToRead)
					*(*this);
				needToRead = true;
				return *this;
			}
			virtual bool eof() {
				return endOfFile;
			}
		};
		inline bool isDelimiter(char a) {
			return !(a >= 'a' && a <= 'z' || a >= 'A' && a <= 'Z' || a >= '0' && a <= '9' || a == '_');
		}
		class WordsFromFileReader : public FileParser {
			StringsFromFileReader strReader;
			std::string curWord;
			size_t curStrPos;
			bool needToRead;
		public:
			WordsFromFileReader() = default;
			WordsFromFileReader(const std::string& fileName){
				openFile(fileName);
			}
			virtual void openFile(const std::string& fileName) {
				strReader.openFile(fileName);
				curStrPos = 0;
				needToRead = true;
			}
			virtual std::string& operator*() {
				if (!needToRead)
					return curWord;
				if (curStrPos >= strReader->length()) {
					++strReader;
					*strReader;
					curStrPos = 0;
					curWord.clear();
					throw EolException();
				}
				curWord.clear();
				const std::string& curStr = *strReader;
				size_t curStrLength = curStr.length();
				unsigned char curSymbol;
				curSymbol = curStr[curStrPos++];
				while (!isDelimiter(curSymbol) && curStrPos <= curStrLength) {
					curWord += curSymbol;
					curSymbol = curStr[curStrPos++];
				}
				needToRead = false;
				return curWord;
			}
			virtual const std::string* operator->() {
				return &*(*this);
			}
			virtual WordsFromFileReader& operator++() {
				if (needToRead)
					*(*this);
				needToRead = true;
				return *this;
			}
			virtual bool eof() {
				return (strReader.eof() && curStrPos >= strReader->length());
			}
			void printWordsFromFile(const std::string& fileName) {
				openFile(fileName);
				try {
					while (true)
						try {
							std::cout << '<';
							std::cout << *(*this);
							++(*this);
							while (true) {
								*(*this);
								std::cout << '|' << *(*this);
								++(*this);
							}
						}
						catch (EolException) {
							std::cout << '>' << std::endl;
						}
				}
				catch (EofException) {
					std::cout << '>' << std::endl;
				}
			}
		};
		size_t findWord(std::string& str, std::string& word, size_t startPos) {
			size_t pos = str.find(word, startPos);
			if (pos == 0 && pos + word.length() < str.length() && isDelimiter(str[pos + word.length()])) {
				return pos;
			}
			while (pos != std::string::npos) {
				if (isDelimiter(str[pos - 1]) && pos + word.length() < str.length() && isDelimiter(str[pos + word.length()]))
					return pos;
				pos = str.find(word, pos + word.length() + 1);
			}
			return std::string::npos;
		}
		bool isDecimal(std::string& str) {
			if (str.length() == 0)
				return false;
			if (str[0] == 0 && str.length() > 1)
				return false;
			for (auto symbolPtr = str.begin(); symbolPtr != str.end(); ++symbolPtr)
				if (!(*symbolPtr >= 0 && *symbolPtr <= 9))
					return false;
			return true;
		}

		
		template<typename parentType, typename firstInheritor, typename... otherInheritors>
		class _DifferentInheritorsMap : public _DifferentInheritorsMap<parentType, otherInheritors> {
		protected:
			std::vector<firstInheritor> inheritorGroup;
		};
		
		template<typename parentType, typename firstInheritor>
		class _DifferentInheritorsMap {
		protected:
			std::map<unsigned int, parentType*> ptrs;
			size_t inheritorGroupsNumber = 0;
			std::vector<Worker>* inheritors = reinterpret_cast<std::vector<Worker>*>(&inheritorGroup);
			std::vector<firstInheritor> inheritorGroup;
		};

		template<typename parentType, typename... inheritorTypes>
		class DifferentInheritorsMap : public _DifferentInheritorsMap<parentType, inheritorTypes> {
			DifferentInheritorsMap() {
				inheritorGroupsNumber = (&inheritorGroup - inheritors);
				std::cout << inheritorGroupsNumber;
			}
		};//*/
	}
	class ReadFileWorker : public Utils::Worker {
		std::ifstream inputFlow;
	public:
		ReadFileWorker(std::string& fileName) {
			inputFlow.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			try {
				inputFlow.open(fileName);
			}
			catch (std::ios_base::failure) {
				throw Utils::TellingException(std::string("ReadFileWorker initialization error: can not operate with file \"") + fileName + "\".");
			}
		}
		void process(Text& text) {
			inputFlow.exceptions(std::ifstream::eofbit);
			std::string tmp;
			try {
				while (true) {
					std::getline(inputFlow, tmp);
					text.push_back(tmp);
				}
			}
			catch (std::ios_base::failure) {
				text.push_back(tmp);
			}
		}
	};
	class WriteFileWorker : public Utils::Worker {
		std::ofstream outputFlow;
	public:
		WriteFileWorker(std::string& fileName) {
			outputFlow.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			try {
				outputFlow.open(fileName);
			}
			catch (std::ios_base::failure) {
				throw Utils::TellingException(std::string("WriteFileWorker initialization error: can not operate with file \"") + fileName + "\".");
			}
		}
		void process(Text& text) {
			if (!text.empty()) {
				outputFlow << text.front();
				text.pop_front();
			}
			while (!text.empty()) {
				outputFlow << std::endl << text.front();
				text.pop_front();
			}
		}
	};
	class GrepWorker : public Utils::Worker {
		std::string word;
	public:
		GrepWorker(std::string& word) : word(word) {}
		void process(Text& text) {
			auto nextIterator = text.begin();
			auto curIterator = nextIterator;
			auto endIterator = text.end();
			while (nextIterator != endIterator) {
				curIterator = nextIterator++;
				if (Utils::findWord(*curIterator, word, 0) == std::string::npos)
					text.erase(curIterator);
			}
		}
	};
	class SortWorker : public Utils::Worker {
	public:
		void process(Text& text) {
			text.sort([](std::string& str1, std::string& str2)->bool { return (str1.compare(str2) < 0); });
		}
	};
	class ReplaceWorker : public Utils::Worker {
		std::string word;
		std::string newWord;
	public:
		ReplaceWorker(std::string& word, std::string& newWord) : word(word), newWord(newWord) {}
		void process(Text& text) {
			auto nextIterator = text.begin();
			auto curIterator = nextIterator;
			auto endIterator = text.end();
			size_t pos;
			while (nextIterator != endIterator) {
				curIterator = nextIterator++;
				pos = Utils::findWord(*curIterator, word, 0);
				while (pos != std::string::npos) {
					(*curIterator).replace(pos, word.length(), newWord);
					pos = Utils::findWord(*curIterator, word, pos + newWord.length() + 1);
				}
			}
		}
	};
	class Conveyor {
		std::map<unsigned int, Utils::Worker*> workers;
		std::vector<ReadFileWorker> readers;
		std::vector<WriteFileWorker> writers;
		std::vector<GrepWorker> grepers;
		std::vector<SortWorker> sorters;
		std::vector<ReplaceWorker> replacers;
		std::vector<Utils::Worker>* workerGroups = reinterpret_cast<std::vector<Utils::Worker>*>(&readers);
		//todo
	};
}