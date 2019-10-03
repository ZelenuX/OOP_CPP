#pragma once
#include <iostream>
using namespace std;

enum Nucleotide { A, G, C, T };

class RNA {
	char* nucs = nullptr;
	unsigned int length = 0;
	unsigned int allocatedLength = 0;
	class NucleotideReference {
		RNA* rna;
		unsigned int idx;
	public:
		NucleotideReference(RNA* rna, unsigned int idx) {
			this->rna = rna;
			this->idx = idx;
		}
		operator Nucleotide() {
			switch ((rna->nucs[idx / 4] >> 2 * (3 - idx % 4)) & 3) {
			case 0:
				return A;
			case 1:
				return G;
			case 2:
				return C;
			case 3:
				return T;
			default:
				cout << "Error in Nucleotide() in nucleotideReference.\n";	//todo change
				return G;
			}
		}
		Nucleotide operator=(Nucleotide nucleotide) {
			switch (nucleotide) {
			case A:
				rna->nucs[idx / 4] = rna->nucs[idx / 4] & ~(3 << 2 * (3 - idx % 4));
				break;
			case G:
				rna->nucs[idx / 4] = (rna->nucs[idx / 4] & ~(3 << 2 * (3 - idx % 4))) + (1 << 2 * (3 - idx % 4));
				break;
			case C:
				rna->nucs[idx / 4] = (rna->nucs[idx / 4] & ~(3 << 2 * (3 - idx % 4))) + (2 << 2 * (3 - idx % 4));
				break;
			case T:
				rna->nucs[idx / 4] = (rna->nucs[idx / 4] & ~(3 << 2 * (3 - idx % 4))) + (3 << 2 * (3 - idx % 4));
				break;
			}
			return nucleotide;
		}
		Nucleotide operator=(NucleotideReference reference) {
			Nucleotide nuc = (Nucleotide)reference;
			(*this) = nuc;
			return nuc;
		}
	};
	unsigned int newBytesAmount(unsigned int newLength) {
		unsigned int resBytesAmount;
		unsigned int curBytesAmount = allocatedLength / 4 + (allocatedLength % 4 ? 1 : 0);
		unsigned int newBytesAmount = newLength / 4 + (newLength % 4 ? 1 : 0);
		if (curBytesAmount >= newBytesAmount) {
			if (newBytesAmount * 4 > curBytesAmount) {
				resBytesAmount = curBytesAmount;
			}
			else {
				resBytesAmount = curBytesAmount / 2;
			}
		}
		else {
			unsigned int shift = 0;
			--newBytesAmount;
			while (newBytesAmount > 0) {
				newBytesAmount >>= 1;
				++shift;
			}
			resBytesAmount = (unsigned int)1 << shift;
		}
		return (resBytesAmount < 4 ? 4 : resBytesAmount);
	}
	void allocMem(unsigned int rnaLength) {
		allocatedLength = newBytesAmount(rnaLength);
		nucs = new char[allocatedLength];
		allocatedLength *= 4;
		length = rnaLength;
	}
	void resize(unsigned int newLength) {
		if (allocatedLength == newBytesAmount(newLength))
			return;
		char* oldNucs = nucs;
		unsigned int copyLength = newLength < length ? newLength : length;
		allocMem(newLength);
		for (unsigned int i = 0; i != copyLength / 4; ++i)
			nucs[i] = oldNucs[i];
		if (copyLength % 4)
			nucs[copyLength / 4] = oldNucs[copyLength / 4];
		delete[] oldNucs;
	}
public:
	RNA() {}
	explicit RNA(unsigned int rnaLength) {
		allocMem(rnaLength);
	}
	RNA(Nucleotide nucleotide, unsigned int rnaLength) : RNA(rnaLength) {
		for (unsigned int i = 0; i != rnaLength; ++i)
			(*this)[i] = nucleotide;
	}
	RNA(RNA& rna) : RNA(rna.length) {
		for (unsigned int i = 0; i != length; ++i)
			(*this)[i] = rna[i];
	}
	~RNA() {
		delete[] nucs;
	}
	unsigned int size() {
		return length;
	}
	unsigned int capacity() {
		return allocatedLength;
	}
	RNA operator+(RNA& rna2) {
		RNA rna(length + rna2.length);
		for (unsigned int i = 0; i != length; ++i)
			rna[i] = (*this)[i];
		for (unsigned int i = 0; i != rna2.length; ++i)
			rna[length + i] = rna2[i];
		return rna;
	}
	RNA operator+(Nucleotide nucleotide) {
		RNA rna(length + 1);
		for (unsigned int i = 0; i != length; ++i)
			rna[i] = (*this)[i];
		rna[length] = nucleotide;
		return rna;
	}
	bool operator!=(RNA& rna2) {
		if (length != rna2.length)
			return true;
		for (unsigned int i = 0; i != length / 4; ++i)
			if (nucs[i] != rna2.nucs[i])
				return true;
		for (char i = 0; i != length % 4; ++i)
			if ((*this)[length - 1 - i] != rna2[length - 1 - i])
				return true;
		return false;
	}
	bool operator==(RNA& rna2) {
		return !(*this != rna2);
	}
	RNA& operator=(RNA& rna2) {
		if (&rna2 == this)
			return rna2;
		resize(rna2.length);
		for (int i = 0; i != length / 4; ++i)
			nucs[i] = rna2.nucs[i];
		if (length % 4)
			nucs[length / 4] = rna2.nucs[length / 4];
		return rna2;
	}
	RNA operator!() {
		RNA rna(*this);
		for (unsigned int i = 0; i != size(); ++i)
			switch ((*this)[i]) {
			case A:
				rna[i] = T;
				break;
			case C:
				rna[i] = G;
				break;
			case G:
				rna[i] = C;
				break;
			case T:
				rna[i] = A;
				break;
			}
		return rna;
	}
	NucleotideReference operator[](unsigned int idx) {
		return NucleotideReference(this, idx);
	}
	bool isComplementary(RNA& rna) {
		if (rna.length != length)
			return false;
		for (unsigned int i = 0; i != length; ++i)
			switch ((*this)[i]) {
			case A:
				if (rna[i] != T)
					return false;
				break;
			case C:
				if (rna[i] != G)
					return false;
				break;
			case G:
				if (rna[i] != C)
					return false;
				break;
			case T:
				if (rna[i] != A)
					return false;
				break;
			}
		return true;
	}
	RNA split(unsigned int tailStartIdx) {
		RNA tail(length - tailStartIdx);
		for (unsigned int i = 0; i != length - tailStartIdx; ++i)
			tail[i] = (*this)[tailStartIdx + i];
		resize(tailStartIdx);
		return tail;
	}
	unsigned int cardinality(Nucleotide nucleotide) {
		unsigned int res = 0;
		for (int i = 0; i != length; ++i)
			res += ((*this)[i] == nucleotide);
		return res;
	}
	void print() {
		for (unsigned int i = 0; i != this->length; ++i)
			switch ((*this)[i]) {
			case A:
				cout << 'A';
				break;
			case G:
				cout << 'G';
				break;
			case C:
				cout << 'C';
				break;
			case T:
				cout << 'T';
				break;
			}
	}
	void println() {
		print();
		cout << endl;
	}
};

class DNA {
private:
	RNA rna0, rna1;
public:
	DNA(RNA& rna0, RNA& rna1) {
		if (rna0.isComplementary(rna1)) {
			this->rna0 = rna0;
			this->rna1 = rna1;
		}
	}
	void print() {
		cout << '/';
		rna0.print();
		cout << '\\' << endl;
		cout << '\\';
		rna1.print();
		cout << '/';
	}
	void println() {
		print();
		cout << endl;
	}
};

RNA operator+(Nucleotide nuc, RNA& rna) {
	return RNA(nuc,1) + rna;
}
RNA operator+(Nucleotide nuc1, Nucleotide nuc2) {
	return RNA(nuc1, 1) + nuc2;
}
