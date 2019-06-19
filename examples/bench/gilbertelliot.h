#pragma once

#include <random>
#include <chrono>

class GilbertElliot
{
public:
    GilbertElliot( double beta = 1.0, double gamma = 0.0 )
        :_goodState(true)
    {
        generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
    }

    GilbertElliot( const GilbertElliot& other )
    {
        *this = other;
    }

    virtual ~GilbertElliot()
    {

    }

    GilbertElliot operator = (const GilbertElliot& other )
    {
        beta = other.beta;
        gamma = other.gamma;
        _goodState = other._goodState;
        generator = other.generator;
        return *this;
    }

    bool sample()
    {
        bool oldState = _goodState;

        // Update state using model parameters
        std::uniform_real_distribution<double> distribution(0.0, 1.0);
        double p = distribution(generator);

        // If in good state...
        if( _goodState )
        {
            // Will we go to bad ?
            _goodState = (p > gamma);
        }
        else
        {
            _goodState = (p > (1.0 - beta));
        }

        return oldState;
    }

    double beta;
    double gamma;

    static GilbertElliot fromBadProbabilityAndBurstLength( double ssBadProba, double expectedBurstLength )
    {
        ssBadProba /= 1.0;
        GilbertElliot ret;
        ret.beta = 1.0 / expectedBurstLength;
        // ret.gamma = (-1.0 * ssBadProba * ret.beta) / (ssBadProba - 1.0 );
        ret.gamma = -( ssBadProba /(expectedBurstLength * (ssBadProba - 1.0) ) );
        return ret;
    }

private:
    std::default_random_engine generator;
    bool _goodState;

protected:

};
