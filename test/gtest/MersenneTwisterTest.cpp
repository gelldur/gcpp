//
// Created by cezary on 21.09.16.
//

#include <gtest/gtest.h>
#include <log.h>
#include <map/generator/MersenneTwister.h>
#include <random>

TEST(MersenneTwisterTest, simpleGeneratorTest)
{
	MersenneTwister mersenneTwister;

	mersenneTwister.initialize(100);
	unsigned int number = mersenneTwister.extract();
	EXPECT_EQ(2333906440u, number);

	mersenneTwister.initialize(101);
	number = mersenneTwister.extract();
	EXPECT_EQ(2217915231u, number);

	mersenneTwister.initialize(102);
	number = mersenneTwister.extract();
	EXPECT_EQ(2567022848u, number);

	mersenneTwister.initialize(103);
	number = mersenneTwister.extract();
	EXPECT_EQ(1855903495u, number);

	mersenneTwister.initialize(104);
	number = mersenneTwister.extract();
	EXPECT_EQ(635314245u, number);

	mersenneTwister.initialize(105);
	number = mersenneTwister.extract();
	EXPECT_EQ(359660224u, number);

	mersenneTwister.initialize(1000045);
	number = mersenneTwister.extract();
	EXPECT_EQ(1237860087u, number);

	mersenneTwister.initialize(555666);
	number = mersenneTwister.extract();
	EXPECT_EQ(37304022u, number);

	mersenneTwister.initialize(121321);
	number = mersenneTwister.extract();
	EXPECT_EQ(4193140867u, number);
}

TEST(MersenneTwisterTest, simpleGeneratorTestWithDistribution)
{
	MersenneTwister mersenneTwister;

	mersenneTwister.initialize(100);
	//TODO FIXME uniform_int_distribution isn't cross platform solution
	std::uniform_int_distribution<> distribution(0, 10);
	EXPECT_EQ(5, distribution(mersenneTwister));

	mersenneTwister.initialize(555666);
	EXPECT_EQ(0, distribution(mersenneTwister));

	mersenneTwister.initialize(1000045);
	EXPECT_EQ(3, distribution(mersenneTwister));
}
