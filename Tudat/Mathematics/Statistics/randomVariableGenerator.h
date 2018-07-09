/*    Copyright (c) 2010-2018, Delft University of Technology
 *    All rigths reserved
 *
 *    This file is part of the Tudat. Redistribution and use in source and
 *    binary forms, with or without modification, are permitted exclusively
 *    under the terms of the Modified BSD license. You should have received
 *    a copy of the license with this file. If not, please or visit:
 *    http://tudat.tudelft.nl/LICENSE.
 */

#ifndef TUDAT_RANDOMVARIABLEGENERATION_H
#define TUDAT_RANDOMVARIABLEGENERATION_H

#include <ctime>

#include <functional>
#include <boost/random.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/make_shared.hpp>

#include "Tudat/Mathematics/Statistics/boostProbabilityDistributions.h"

namespace tudat
{

namespace statistics
{

//! Base class for random number generation
template< typename DependentVariableType >
class RandomVariableGenerator
{
public:

    //! Constructor, sets the seed for the base random number generator.
    /*!
     *  Constructor, sets the seed for the base random number generator.
     *  \param seed Seed of random number generator (default is pseudo-random time(0))
     */
    RandomVariableGenerator( const double seed ):randomNumberGenerator_( seed ){ }

    //! Destructor
    virtual ~RandomVariableGenerator( ){ }

    //! Function to generate random number
    /*!
     *  This function generates a random number from the distribution defined by the derived class.
     *  It maps the random number (representing the cdf value) generated by the  randomNumberGenerator_ member to that of
     *  the value of the independent variable according to the distribution implemented in the derived class..
     *  \return Randomly generated number of derived class distribution.
     */
    virtual DependentVariableType getRandomVariableValue( ) = 0;

protected:

    //! Uniform (0,1) random number generator, to be mapped to specific distribution by derived classes.
    boost::random::mt19937 randomNumberGenerator_;
};

//! Random number generator generated directly from inverse cdf function of probability distribution
/*!
 *  This class uses a probability distributio (or more specifically the inverse cumulative distribution function) from which
 *  numbers are randomly generated.  The inverse cdf function is evaluated from an object of type
 *  InvertibleContinuousProbabilityDistribution< double >.
 *  The uniform random number generator that is mapped to the random number from the specific
 *  distribution is generated from the boost::random::mt19937 generator; its seed may be either pseudo-random (time( 0 ))
 *  or user-defined.
 */
class ContinuousRandomVariableGenerator: public RandomVariableGenerator< double >
{
public:

    //! Constructor
    /*!
     * Constructor
     * \param randomVariable Probability distribution from which random number is generated.
     * \param seed Seed of random number generator.
     */
    ContinuousRandomVariableGenerator(
            const std::shared_ptr< InvertibleContinuousProbabilityDistribution< double > > randomVariable,
            const double seed ):
        RandomVariableGenerator< double >( seed ), randomVariable_( randomVariable ),
        randomUniformCdfGenerator_( randomNumberGenerator_ ){ }

    //! Function to generate random number
    /*!
     *  This function generates a random number from the distribution defined by randomVariable_
     *  It maps the random number (representing the cdf value) generated by the  randomNumberGenerator_ member to that of
     *  the value of the independent variable according to the distribution in randomVariable_.
     *  \return Randomly generated number of from given distribution
     */
    double getRandomVariableValue( )
    {
        return randomVariable_->evaluateInverseCdf( randomUniformCdfGenerator_( ) );
    }

private:

    //! Probability distribution from which random number is generated.
    std::shared_ptr< InvertibleContinuousProbabilityDistribution< double > > randomVariable_;

    //! Random number generator in uniform (0,1) distribution, representing a random probability value.
    /*!
     *  Random number generator in uniform (0,1) distribution, representing a random probability value. This value is then
     *  mapped to a random number by calling the inverse cdf function in randomVariable_.
     */
    boost::random::uniform_01< boost::random::mt19937 > randomUniformCdfGenerator_;

};


//! Function to create a random number generating function from a continuous univariate distribution implemented in boost
/*!
 *  Function to create a random number generating function from a continuous univariate distribution implemented in boost
 *  \param boostDistribution Type of distribution of which a random variable class is to be generated
 *  \param parameters Free parameters of requested distribution (for instance mean and standard deviation for Gaussian
 *  distribution, see implementation for order of parameters, if relevant)     * \param seed Seed of random number generator.
 *  \param seed Seed of random number generator.
 *  \return Random number generator according to given distribution.
 */
std::function< double( ) > createBoostContinuousRandomVariableGeneratorFunction(
        const ContinuousBoostStatisticalDistributions boostDistribution,
        const std::vector< double >& parameters,
        const double seed );

//! Function to create a random number generator from a continuous univariate distribution implemented in boost
/*!
 *  Function to create a random number generator from a continuous univariate distribution implemented in boost
 *  \param boostDistribution Type of distribution of which a random variable class is to be generated
 *  \param parameters Free parameters of requested distribution (for instance mean and standard deviation for Gaussian
 *  distribution, see implementation for order of parameters, if relevant)     * \param seed Seed of random number generator.
 *  \param seed Seed of random number generator.
 *  \return Random number generator according to given distribution.
 */
std::shared_ptr< RandomVariableGenerator< double > > createBoostContinuousRandomVariableGenerator(
        const ContinuousBoostStatisticalDistributions boostDistribution,
        const std::vector< double >& parameters,
        const double seed  );
} // namespace statistics

} // namespace tudat

#endif // TUDAT_RANDOMVARIABLEGENERATION_H