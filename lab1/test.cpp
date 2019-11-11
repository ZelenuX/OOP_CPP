#pragma once
#include "pch.h"
#include <crtdbg.h>
#define new new( _NORMAL_BLOCK, __FILE__, __LINE__)
#define _CRTDBG_MAP_ALLOC
#include "DNA.h"

TEST(RNATest, constructors) {
	RNA rna0;
	EXPECT_EQ(rna0.size(), 0);
	EXPECT_EQ(rna0.capacity(), 0);

	int size = 3;
	RNA rna1(size);
	EXPECT_TRUE(rna1.capacity() >= size);
	EXPECT_EQ(rna1.size(), size);
	
	size = 5;
	Nucleotide nuc = C;
	RNA rna2(nuc, size);
	EXPECT_TRUE(rna2.capacity() >= size);
	EXPECT_EQ(rna2.size(), 5);
	for (unsigned int i = 0; i != size; ++i)
		EXPECT_TRUE(rna2[i] == nuc);

	RNA rna3(rna2);
	EXPECT_TRUE(rna3.capacity() == rna2.capacity());
	EXPECT_EQ(rna3.size(), rna2.size());
	for (unsigned int i = 0; i != rna2.size(); ++i)
		EXPECT_TRUE(rna3[i] == rna2[i]);

	RNA rna4(RNA(RNA(RNA(RNA(A, 7)))) + RNA(A, 8));
	EXPECT_EQ(rna4.size(), 15);
	EXPECT_TRUE(rna4 == RNA(A, 15));
}
TEST(RNATest, destructor) {
	RNA rna(A, 9);
	EXPECT_EQ(rna.size(), 9);
	EXPECT_EQ(rna.capacity(), 16);
	
	rna.~RNA();
	EXPECT_EQ(rna.size(), 0);
	EXPECT_EQ(rna.capacity(), 0);

	rna.~RNA();
	EXPECT_EQ(rna.size(), 0);
	EXPECT_EQ(rna.capacity(), 0);
}
TEST(RNATest, operatorSum) {
	unsigned int size1 = 12;
	unsigned int size2 = 3;
	RNA rna1(T, size1);
	RNA rna2(C, size2);

	rna2 = T + rna2 + (A + G);
	EXPECT_EQ(rna2.size(), size2 + 3);
	EXPECT_TRUE(rna2.capacity() >= rna2.size());
	EXPECT_TRUE(rna2[0] == T);
	for (unsigned int i = 1; i != size2; ++i)
		EXPECT_TRUE(rna2[i] == C);
	EXPECT_TRUE(rna2[size2 + 1] == A);
	EXPECT_TRUE(rna2[size2 + 2] == G);

	RNA rna3 = rna1 + rna2;
	EXPECT_TRUE(rna3.size() == rna2.size() + rna1.size());
	EXPECT_TRUE(rna3.capacity() >= rna2.capacity() + rna1.capacity());
	for (unsigned int i = 0; i != rna1.size(); ++i)
		EXPECT_TRUE(rna3[i] == rna1[i]);
	for (unsigned int i = 0; i != rna2.size(); ++i)
		EXPECT_TRUE(rna3[i + rna1.size()] == rna2[i]);
}
TEST(RNATest, operatorInc) {
	RNA rna0(A, 10);
	rna0 += G;
	EXPECT_TRUE(rna0 == RNA(A, 10) + G);

	rna0 += rna0;
	EXPECT_TRUE(rna0 == RNA(A, 10) + G + RNA(A, 10) + G);
}
TEST(RNATest, operatorsEqNotEq) {
	RNA rna1(A, 14);
	RNA rna2(T, 12);
	RNA rna3(rna1);
	RNA rna4(rna1 + A);
	EXPECT_TRUE(rna1 != rna2);
	EXPECT_FALSE(rna1 == rna2);
	EXPECT_TRUE(rna1 == rna3);
	EXPECT_FALSE(rna1 != rna3);
	EXPECT_TRUE(rna1 != rna4);
	EXPECT_FALSE(rna1 == rna4);
}
TEST(RNATest, operatorAssignment) {
	RNA rna0(C, 127);
	RNA rna1(rna0);
	RNA rna2(T, 257);
	RNA rna3 = rna1 = rna2 = rna1;
	EXPECT_TRUE(rna1 == rna0);
	EXPECT_TRUE(rna2 == rna0);
	EXPECT_TRUE(rna3 == rna0);
	rna2 = rna0 + T;
	EXPECT_TRUE(rna2 == rna0 + T);
	EXPECT_TRUE(rna2 == rna3 + T);
}
TEST(RNATest, operatorInvert) {
	RNA rna0 = RNA(T, 14) + RNA(G, 11);
	RNA rna1 = RNA(A, 14) + RNA(C, 11);
	EXPECT_TRUE(rna0 == !!rna0);
	EXPECT_TRUE(rna0 == !rna1);
	EXPECT_TRUE(rna0 == !rna1);

}
TEST(RNATest, operatorIdx) {
	unsigned int size1 = 12;
	unsigned int size2 = 10;
	RNA rna0 = RNA(T, size1) + RNA(G, size2) + A;
	const RNA rna2(A, 100);
	Nucleotide n = rna2[0];
	for (unsigned int i = 0; i != size1; ++i)
		EXPECT_TRUE(rna0[i] == T);
	for (unsigned int i = 0; i != size2; ++i)
		EXPECT_TRUE(rna0[size1 + i] == G);
	EXPECT_TRUE(rna0[size1 + size2] == A);

	rna0[size1 + size2 - 1] = rna0[size1 + size2 - 2] = rna0[size1 + size2 - 3] = C;
	rna0[0] = rna0[size1 + size2 - 1];
	EXPECT_TRUE(rna0[0] == C);
	for (unsigned int i = 1; i != size1; ++i)
		EXPECT_TRUE(rna0[i] == T);
	for (unsigned int i = 0; i != size2 - 3; ++i)
		EXPECT_TRUE(rna0[size1 + i] == G);
	EXPECT_TRUE(rna0[size1 + size2 - 3] == C);
	EXPECT_TRUE(rna0[size1 + size2 - 2] == C);
	EXPECT_TRUE(rna0[size1 + size2 - 1] == C);
	EXPECT_TRUE(rna0[size1 + size2] == A);

	EXPECT_TRUE(RNA(A, 1)[0] == RNA(A, 1)[0]);
}
TEST(RNATest, isComplementary) {
	RNA rna0 = RNA(A, 4) + RNA(C, 3) + RNA(G, 2) + RNA(T, 1);
	EXPECT_TRUE(rna0.isComplementary(!rna0));
	EXPECT_FALSE(rna0.isComplementary(!rna0 + A));
	EXPECT_FALSE((rna0 + T).isComplementary(!rna0));
}
TEST(RNATest, split) {
	RNA rna0 = RNA(A, 4) + RNA(C, 3) + RNA(G, 2) + RNA(T, 1);
	RNA rna1 = rna0.split(6);
	EXPECT_TRUE(rna0 == RNA(A, 4) + RNA(C, 2));
	EXPECT_TRUE(rna1 == RNA(C, 1) + RNA(G, 2) + RNA(T, 1));
	EXPECT_EQ(rna0.size(), 6);
	EXPECT_EQ(rna1.size(), 4);

	RNA rna2 = rna0;
	rna0 = rna1.split(5);
	EXPECT_EQ(rna0.size(), 0);
	EXPECT_EQ(rna1.size(), 4);
	rna2 = rna1.split(-3);
	EXPECT_EQ(rna2.size(), 0);
	EXPECT_EQ(rna1.size(), 4);
	rna0 = rna1.split(0);
	EXPECT_EQ(rna0.size(), 4);
	EXPECT_EQ(rna1.size(), 0);

	rna0 = rna1.split(0);
	EXPECT_EQ(rna1.size(), 0);
	EXPECT_EQ(rna0.size(), 0);
}
TEST(RNATest, cardinality) {
	RNA rna0 = RNA(A, 4) + RNA(C, 3) + RNA(G, 2) + RNA(T, 1);
	rna0 = rna0 + rna0 + RNA(G, 5);
	EXPECT_EQ(rna0.cardinality(A), 8);
	EXPECT_EQ(rna0.cardinality(C), 6);
	EXPECT_EQ(rna0.cardinality(G), 9);
	EXPECT_EQ(rna0.cardinality(T), 2);
	std::unordered_map<Nucleotide, size_t, std::hash<int>> cards;
	EXPECT_EQ(rna0.cardinality(A), rna0.cardinality(A));
	EXPECT_EQ(rna0.cardinality(C), rna0.cardinality(C));
	EXPECT_EQ(rna0.cardinality(G), rna0.cardinality(G));
	EXPECT_EQ(rna0.cardinality(T), rna0.cardinality(T));
}
TEST(RNATest, memoryAllocation) {
	RNA rna;
	EXPECT_EQ(rna.size(), 0);
	EXPECT_EQ(rna.capacity(), 0);
	rna = rna + A;
	EXPECT_EQ(rna.size(), 1);
	EXPECT_EQ(rna.capacity(), 16);
	rna = rna + A + A + A;
	EXPECT_EQ(rna.size(), 4);
	EXPECT_EQ(rna.capacity(), 16);
	rna = rna + A;
	EXPECT_EQ(rna.size(), 5);
	EXPECT_EQ(rna.capacity(), 16);
	rna = rna + A + A + A;
	EXPECT_EQ(rna.size(), 8);
	EXPECT_EQ(rna.capacity(), 16);
	rna = rna + A;
	EXPECT_EQ(rna.size(), 9);
	EXPECT_EQ(rna.capacity(), 16);
	rna = rna + RNA(A, 7);
	EXPECT_EQ(rna.size(), 16);
	EXPECT_EQ(rna.capacity(), 16);
	rna = rna + A;
	EXPECT_EQ(rna.size(), 17);
	EXPECT_EQ(rna.capacity(), 32);
	rna = rna + RNA(A, 15);
	EXPECT_EQ(rna.size(), 32);
	EXPECT_EQ(rna.capacity(), 32);
	rna = rna + A;
	EXPECT_EQ(rna.size(), 33);
	EXPECT_EQ(rna.capacity(), 64);

	rna.split(32);
	EXPECT_EQ(rna.size(), 32);
	EXPECT_EQ(rna.capacity(), 64);
	rna.split(17);
	EXPECT_EQ(rna.size(), 17);
	EXPECT_EQ(rna.capacity(), 64);
	rna.split(16);
	EXPECT_EQ(rna.size(), 16);
	EXPECT_EQ(rna.capacity(), 32);
	rna.split(9);
	EXPECT_EQ(rna.size(), 9);
	EXPECT_EQ(rna.capacity(), 32);
	rna.split(8);
	EXPECT_EQ(rna.size(), 8);
	EXPECT_EQ(rna.capacity(), 16);
	rna.split(5);
	EXPECT_EQ(rna.size(), 5);
	EXPECT_EQ(rna.capacity(), 16);
	rna.split(4);
	EXPECT_EQ(rna.size(), 4);
	EXPECT_EQ(rna.capacity(), 16);
	rna.split(1);
	EXPECT_EQ(rna.size(), 1);
	EXPECT_EQ(rna.capacity(), 16);
	rna.split(0);
	EXPECT_EQ(rna.size(), 0);
	EXPECT_EQ(rna.capacity(), 16);
}
TEST(RNATest, manyAddings) {
	RNA a;
	for (size_t i = 1; i != 1000'000'000; ++i)
		a += A;
}

void main(int argc, wchar_t** argv) {
	::testing::InitGoogleTest(&argc, argv);
	_CrtMemState state1;
	_CrtMemCheckpoint(&state1);
	RUN_ALL_TESTS();
	_CrtMemDumpAllObjectsSince(&state1);
}