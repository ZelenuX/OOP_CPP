#pragma once
#include <unordered_map>
#include <iostream>

enum Nucleotide { A, G, C, T };

class RNA {
	char* nucs = nullptr;
	size_t length = 0;
	size_t allocatedLength = 0;
	class NucleotideReference {
		RNA* rna;
		size_t idx;
	public:
		NucleotideReference(RNA* rna, size_t idx) {
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
				return A;
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
private:
	inline size_t filledBytes() {
		return (length / 4 + (bool)(length % 4));
	}
	size_t newBytesAmount(size_t newLength) {
		size_t resBytesAmount;
		size_t curBytesAmount = allocatedLength / 4 + (bool)(allocatedLength % 4);
		size_t newBytesAmount = newLength / 4 + (bool)(newLength % 4);
		if (curBytesAmount >= newBytesAmount) {
			if (newBytesAmount * 4 > curBytesAmount) {
				resBytesAmount = curBytesAmount;
			}
			else {
				resBytesAmount = curBytesAmount / 2;
			}
		}
		else {
			size_t shift = 0;
			--newBytesAmount;
			while (newBytesAmount > 0) {
				newBytesAmount >>= 1;
				++shift;
			}
			resBytesAmount = (size_t)1 << shift;
		}
		return (resBytesAmount < 4 ? 4 : resBytesAmount);
	}
	void allocMem(size_t rnaLength) {
		allocatedLength = newBytesAmount(rnaLength);
		nucs = new char[allocatedLength];
		allocatedLength *= 4;
		length = rnaLength;
	}
	void resize(size_t newLength, bool copy = true) {
		if (allocatedLength == newBytesAmount(newLength) * 4) {
			length = newLength;
			return;
		}
		char* oldNucs = nucs;
		size_t copyLength = newLength / 4 + (bool)(newLength % 4);
		if (filledBytes() < copyLength)
			copyLength = filledBytes();
		allocMem(newLength);
		if (copy && copyLength > 0) {
			for (size_t i = 0; i != copyLength; ++i)
				nucs[i] = oldNucs[i];
		}
		delete[] oldNucs;
	}
public:
	RNA() = default;
	explicit RNA(size_t rnaLength) {
		allocMem(rnaLength);
	}
	RNA(Nucleotide nucleotide, size_t rnaLength) : RNA(rnaLength) {
		size_t byteAmount = filledBytes();
		RNA tmpRna(4);
		tmpRna[0] = tmpRna[1] = tmpRna[2] = tmpRna[3] = nucleotide;
		char fillingByte = tmpRna.nucs[0];
		for (size_t i = 0; i != byteAmount; ++i)
			this->nucs[i] = fillingByte;
	}
	RNA(const RNA& rna) : RNA(rna.length) {
		if (length == 0)
			return;
		for (size_t i = filledBytes() - 1; i > 0; --i)
			nucs[i] = rna.nucs[i];
		nucs[0] = rna.nucs[0];
	}
	RNA(RNA&& rna) {
		this->allocatedLength = rna.allocatedLength;
		this->length = rna.length;
		this->nucs = rna.nucs;
		rna.nucs = nullptr;
		rna.allocatedLength = rna.length = 0;
	}
	~RNA() {
		delete[] nucs;
		nucs = nullptr;
		length = allocatedLength = 0;
	}
	size_t size() {
		return length;
	}
	size_t capacity() {
		return allocatedLength;
	}
	RNA operator+(RNA& rna2) {
		RNA rna(length + rna2.length);
		if (length > 0) {
			for (size_t i = filledBytes() - 1; i > 0; --i)
				rna.nucs[i] = this->nucs[i];
			rna.nucs[0] = this->nucs[0];
		}
		if (rna2.length > 0) {
			for (size_t i = 0; i != rna2.length; ++i)
				rna[length + i] = rna2[i];
		}
		return rna;
	}
	RNA operator+(Nucleotide nucleotide) {
		RNA rna(length + 1);
		if (length != 0) {
			for (size_t i = filledBytes() - 1; i > 0; --i)
				rna.nucs[i] = this->nucs[i];
			rna.nucs[0] = this->nucs[0];
		}
		rna[length] = nucleotide;
		return rna;
	}
	RNA& operator+=(Nucleotide nucleotide) {
		this->resize(this->length + 1);
		(*this)[this->length - 1] = nucleotide;
		return *this;
	}
	RNA& operator+=(RNA& rna) {
		*this = *this + rna;
		return *this;
	}
	bool operator!=(RNA& rna2) {
		if (length != rna2.length)
			return true;
		for (size_t i = 0; i != length / 4; ++i)
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
		resize(rna2.length, false);
		for (int i = 0; i != length / 4; ++i)
			nucs[i] = rna2.nucs[i];
		if (length % 4)
			nucs[length / 4] = rna2.nucs[length / 4];
		return rna2;
	}
	RNA& operator=(RNA&& rna2) {
		if (this == &rna2)
			return *this;
		delete[] this->nucs;
		this->allocatedLength = rna2.allocatedLength;
		this->length = rna2.length;
		this->nucs = rna2.nucs;
		rna2.nucs = nullptr;
		rna2.allocatedLength = rna2.length = 0;
		return *this;
	}
	RNA operator!() {
		RNA rna(this->size());
		for (size_t i = 0; i != size(); ++i)
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
	NucleotideReference operator[](size_t idx) {
		return NucleotideReference(this, idx);
	}
	Nucleotide operator[](size_t idx) const {
		return (*const_cast<RNA*>(this))[idx];
	}
	bool isComplementary(RNA& rna) {
		if (rna.length != length)
			return false;
		for (size_t i = 0; i != length; ++i)
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
	RNA split(size_t tailStartIdx) {
		if (tailStartIdx >= length)
			return RNA();
		RNA tail(length - tailStartIdx);
		for (size_t i = 0; i != length - tailStartIdx; ++i)
			tail[i] = (*this)[tailStartIdx + i];
		resize(tailStartIdx);
		return tail;
	}
	size_t cardinality(Nucleotide nucleotide) {
		size_t res = 0;
		for (int i = 0; i != length; ++i)
			res += ((*this)[i] == nucleotide);
		return res;
	}
	std::unordered_map<Nucleotide, size_t, std::hash<int>> cardinality() {
		std::unordered_map<Nucleotide, size_t, std::hash<int>> res;
		res[A] = cardinality(A);
		res[C] = cardinality(C);
		res[G] = cardinality(G);
		res[T] = cardinality(T);
		return res;
	}
};

class DNA {
private:
	RNA rna0, rna1;
	friend std::ostream& operator<<(std::ostream& stream, DNA& dna);
public:
	DNA(RNA& rna0, RNA& rna1) {
		if (rna0.isComplementary(rna1)) {
			this->rna0 = rna0;
			this->rna1 = rna1;
		}
	}
};

RNA operator+(Nucleotide nuc, RNA& rna) {
	return RNA(nuc,1) + rna;
}
RNA operator+(Nucleotide nuc1, Nucleotide nuc2) {
	return RNA(nuc1, 1) + nuc2;
}
std::ostream& operator<<(std::ostream& stream, RNA& rna) {
	for (size_t i = 0; i != rna.size(); ++i)
		switch (rna[i]) {
		case A:
			stream << 'A';
			break;
		case G:
			stream << 'G';
			break;
		case C:
			stream << 'C';
			break;
		case T:
			stream << 'T';
			break;
		}
	return stream;
}
std::ostream& operator<<(std::ostream& stream, DNA& dna) {
	stream << '/' << dna.rna0 << "\\\n\\" << dna.rna1 << '/';
	return stream;
}