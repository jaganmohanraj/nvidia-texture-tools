/* -----------------------------------------------------------------------------

    Copyright (c) 2006 Simon Brown                          si@sjbrown.co.uk
    Copyright (c) 2006 Ignacio Castano                      icastano@nvidia.com

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the
    "Software"), to	deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
    OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

   -------------------------------------------------------------------------- */

#include "ClusterFit.h"
#include "nvmath/Fitting.h"
#include "nvmath/Vector.inl"
#include "nvimage/ColorBlock.h"

#include <float.h> // FLT_MAX

using namespace nv;

ClusterFit::ClusterFit()
{
}

void ClusterFit::setColourSet(const ColorSet * set)
{
    // initialise the best error
#if NVTT_USE_SIMD
    m_besterror = SimdVector( FLT_MAX );
    Vector3 metric = m_metric.toVector3();
#else
    m_besterror = FLT_MAX;
    Vector3 metric = m_metric;
#endif

    // cache some values
    m_count = set->colorCount;

    Vector3 values[16];
    for (uint i = 0; i < m_count; i++)
    {
        values[i] = set->colors[i].xyz();
    }

    Vector3 principle = Fit::computePrincipalComponent(m_count, values, set->weights, metric);

    // build the list of values
    int order[16];
    float dps[16];
    for (uint i = 0; i < m_count; ++i)
    {
        dps[i] = dot(values[i], principle);
        order[i] = i;
    }

    // stable sort
    for (uint i = 0; i < m_count; ++i)
    {
        for (uint j = i; j > 0 && dps[j] < dps[j - 1]; --j)
        {
            swap(dps[j], dps[j - 1]);
            swap(order[j], order[j - 1]);
        }
    }

    // weight all the points
#if NVTT_USE_SIMD
    m_xxsum = SimdVector( 0.0f );
    m_xsum = SimdVector( 0.0f );
#else
    m_xxsum = Vector3(0.0f);
    m_xsum = Vector3(0.0f);
    m_wsum = 0.0f;
#endif
	
    for (uint i = 0; i < m_count; ++i)
    {
        int p = order[i];
#if NVTT_USE_SIMD
        Vector4 tmp(values[p] * set->weights[p], set->weights[p]);
        m_weighted[i] = SimdVector(tmp.component);
        m_xxsum += m_weighted[i] * m_weighted[i];
        m_xsum += m_weighted[i];
#else
        m_weighted[i] = values[p] * set->weights[p];
        m_xxsum += m_weighted[i] * m_weighted[i];
        m_xsum += m_weighted[i];
        m_weights[i] = set->weights[p];
        m_wsum += m_weights[i];
#endif
    }
}


void ClusterFit::setMetric(Vector4::Arg w)
{
#if NVTT_USE_SIMD
    Vector4 tmp(w.xyz(), 1);
    m_metric = SimdVector(tmp.component);
#else
    m_metric = w.xyz();
#endif
    m_metricSqr = m_metric * m_metric;
}

float ClusterFit::bestError() const
{
#if NVTT_USE_SIMD
    SimdVector x = m_xxsum * m_metricSqr;
    SimdVector error = m_besterror + x.splatX() + x.splatY() + x.splatZ();
    return error.toFloat();
#else
    return m_besterror + dot(m_xxsum, m_metricSqr);
#endif

}

#if NVTT_USE_SIMD

bool ClusterFit::compress3( Vector3 * start, Vector3 * end )
{
    int const count = m_count;
    SimdVector const one = SimdVector(1.0f);
    SimdVector const zero = SimdVector(0.0f);
    SimdVector const half(0.5f, 0.5f, 0.5f, 0.25f);
    SimdVector const two = SimdVector(2.0);
    SimdVector const grid( 31.0f, 63.0f, 31.0f, 0.0f );
    SimdVector const gridrcp( 1.0f/31.0f, 1.0f/63.0f, 1.0f/31.0f, 0.0f );

    // declare variables
    SimdVector beststart = SimdVector( 0.0f );
    SimdVector bestend = SimdVector( 0.0f );
    SimdVector besterror = SimdVector( FLT_MAX );

    SimdVector x0 = zero;

    int b0 = 0, b1 = 0;

    // check all possible clusters for this total order
    for( int c0 = 0; c0 <= count; c0++)
    {
        SimdVector x1 = zero;

        for( int c1 = 0; c1 <= count-c0; c1++)
        {
            SimdVector const x2 = m_xsum - x1 - x0;

            //Vector3 const alphax_sum = x0 + x1 * 0.5f;
            //float const alpha2_sum = w0 + w1 * 0.25f;
            SimdVector const alphax_sum = multiplyAdd(x1, half, x0); // alphax_sum, alpha2_sum
            SimdVector const alpha2_sum = alphax_sum.splatW();

            //Vector3 const betax_sum = x2 + x1 * 0.5f;
            //float const beta2_sum = w2 + w1 * 0.25f;
            SimdVector const betax_sum = multiplyAdd(x1, half, x2); // betax_sum, beta2_sum
            SimdVector const beta2_sum = betax_sum.splatW();

            //float const alphabeta_sum = w1 * 0.25f;
            SimdVector const alphabeta_sum = (x1 * half).splatW(); // alphabeta_sum

            // float const factor = 1.0f / (alpha2_sum * beta2_sum - alphabeta_sum * alphabeta_sum);
            SimdVector const factor = reciprocal( negativeMultiplySubtract(alphabeta_sum, alphabeta_sum, alpha2_sum*beta2_sum) );

            SimdVector a = negativeMultiplySubtract(betax_sum, alphabeta_sum, alphax_sum*beta2_sum) * factor;
            SimdVector b = negativeMultiplySubtract(alphax_sum, alphabeta_sum, betax_sum*alpha2_sum) * factor;

            // clamp to the grid
            a = min( one, max( zero, a ) );
            b = min( one, max( zero, b ) );
            a = truncate( multiplyAdd( grid, a, half ) ) * gridrcp;
            b = truncate( multiplyAdd( grid, b, half ) ) * gridrcp;

            // compute the error (we skip the constant xxsum)
            SimdVector e1 = multiplyAdd( a*a, alpha2_sum, b*b*beta2_sum );
            SimdVector e2 = negativeMultiplySubtract( a, alphax_sum, a*b*alphabeta_sum );
            SimdVector e3 = negativeMultiplySubtract( b, betax_sum, e2 );
            SimdVector e4 = multiplyAdd( two, e3, e1 );

            // apply the metric to the error term
            SimdVector e5 = e4 * m_metricSqr;
            SimdVector error = e5.splatX() + e5.splatY() + e5.splatZ();

            // keep the solution if it wins
            if( compareAnyLessThan( error, besterror ) )
            {
                besterror = error;
                beststart = a;
                bestend = b;
                b0 = c0;
                b1 = c1;
            }

            x1 += m_weighted[c0+c1];
        }

        x0 += m_weighted[c0];
    }

    // save the block if necessary
    if( compareAnyLessThan( besterror, m_besterror ) )
    {

        *start = beststart.toVector3();
        *end = bestend.toVector3();

        // save the error
        m_besterror = besterror;

        return true;
    }

    return false;
}

bool ClusterFit::compress4( Vector3 * start, Vector3 * end )
{
    int const count = m_count;
    SimdVector const one = SimdVector(1.0f);
    SimdVector const zero = SimdVector(0.0f);
    SimdVector const half = SimdVector(0.5f);
    SimdVector const two = SimdVector(2.0);
    SimdVector const onethird( 1.0f/3.0f, 1.0f/3.0f, 1.0f/3.0f, 1.0f/9.0f );
    SimdVector const twothirds( 2.0f/3.0f, 2.0f/3.0f, 2.0f/3.0f, 4.0f/9.0f );
    SimdVector const twonineths = SimdVector( 2.0f/9.0f );
    SimdVector const grid( 31.0f, 63.0f, 31.0f, 0.0f );
    SimdVector const gridrcp( 1.0f/31.0f, 1.0f/63.0f, 1.0f/31.0f, 0.0f );

    // declare variables
    SimdVector beststart = SimdVector( 0.0f );
    SimdVector bestend = SimdVector( 0.0f );
    SimdVector besterror = SimdVector( FLT_MAX );

    SimdVector x0 = zero;
    int b0 = 0, b1 = 0, b2 = 0;

    // check all possible clusters for this total order
    for( int c0 = 0; c0 <= count; c0++)
    {
        SimdVector x1 = zero;

        for( int c1 = 0; c1 <= count-c0; c1++)
        {
            SimdVector x2 = zero;

            for( int c2 = 0; c2 <= count-c0-c1; c2++)
            {
                SimdVector const x3 = m_xsum - x2 - x1 - x0;

                //Vector3 const alphax_sum = x0 + x1 * (2.0f / 3.0f) + x2 * (1.0f / 3.0f);
                //float const alpha2_sum = w0 + w1 * (4.0f/9.0f) + w2 * (1.0f/9.0f);
                SimdVector const alphax_sum = multiplyAdd(x2, onethird, multiplyAdd(x1, twothirds, x0)); // alphax_sum, alpha2_sum
                SimdVector const alpha2_sum = alphax_sum.splatW();

                //Vector3 const betax_sum = x3 + x2 * (2.0f / 3.0f) + x1 * (1.0f / 3.0f);
                //float const beta2_sum = w3 + w2 * (4.0f/9.0f) + w1 * (1.0f/9.0f);
                SimdVector const betax_sum = multiplyAdd(x2, twothirds, multiplyAdd(x1, onethird, x3)); // betax_sum, beta2_sum
                SimdVector const beta2_sum = betax_sum.splatW();

                //float const alphabeta_sum = (w1 + w2) * (2.0f/9.0f);
                SimdVector const alphabeta_sum = twonineths*( x1 + x2 ).splatW(); // alphabeta_sum

                // float const factor = 1.0f / (alpha2_sum * beta2_sum - alphabeta_sum * alphabeta_sum);
                SimdVector const factor = reciprocal( negativeMultiplySubtract(alphabeta_sum, alphabeta_sum, alpha2_sum*beta2_sum) );

                SimdVector a = negativeMultiplySubtract(betax_sum, alphabeta_sum, alphax_sum*beta2_sum) * factor;
                SimdVector b = negativeMultiplySubtract(alphax_sum, alphabeta_sum, betax_sum*alpha2_sum) * factor;

                // clamp to the grid
                a = min( one, max( zero, a ) );
                b = min( one, max( zero, b ) );
                a = truncate( multiplyAdd( grid, a, half ) ) * gridrcp;
                b = truncate( multiplyAdd( grid, b, half ) ) * gridrcp;

                // compute the error (we skip the constant xxsum)
                SimdVector e1 = multiplyAdd( a*a, alpha2_sum, b*b*beta2_sum );
                SimdVector e2 = negativeMultiplySubtract( a, alphax_sum, a*b*alphabeta_sum );
                SimdVector e3 = negativeMultiplySubtract( b, betax_sum, e2 );
                SimdVector e4 = multiplyAdd( two, e3, e1 );

                // apply the metric to the error term
                SimdVector e5 = e4 * m_metricSqr;
                SimdVector error = e5.splatX() + e5.splatY() + e5.splatZ();

                // keep the solution if it wins
                if( compareAnyLessThan( error, besterror ) )
                {
                    besterror = error;
                    beststart = a;
                    bestend = b;
                    b0 = c0;
                    b1 = c1;
                    b2 = c2;
                }

                x2 += m_weighted[c0+c1+c2];
            }

            x1 += m_weighted[c0+c1];
        }

        x0 += m_weighted[c0];
    }

    // save the block if necessary
    if( compareAnyLessThan( besterror, m_besterror ) )
    {
        *start = beststart.toVector3();
        *end = bestend.toVector3();

        // save the error
        m_besterror = besterror;

        return true;
    }

    return false;
}

#else

bool ClusterFit::compress3(Vector3 * start, Vector3 * end)
{
    const uint count = m_count;
    const Vector3 grid( 31.0f, 63.0f, 31.0f );
    const Vector3 gridrcp( 1.0f/31.0f, 1.0f/63.0f, 1.0f/31.0f );

    // declare variables
    Vector3 beststart( 0.0f );
    Vector3 bestend( 0.0f );
    float besterror = FLT_MAX;

    Vector3 x0(0.0f);
    float w0 = 0.0f;

    int b0 = 0, b1 = 0;

    // check all possible clusters for this total order
    for (uint c0 = 0; c0 <= count; c0++)
    {
        Vector3 x1(0.0f);
        float w1 = 0.0f;

        for (uint c1 = 0; c1 <= count-c0; c1++)
        {
            float w2 = m_wsum - w0 - w1;

            // These factors could be entirely precomputed.
            float const alpha2_sum = w0 + w1 * 0.25f;
            float const beta2_sum = w2 + w1 * 0.25f;
            float const alphabeta_sum = w1 * 0.25f;
            float const factor = 1.0f / (alpha2_sum * beta2_sum - alphabeta_sum * alphabeta_sum);

            Vector3 const alphax_sum = x0 + x1 * 0.5f;
            Vector3 const betax_sum = m_xsum - alphax_sum;

            Vector3 a = (alphax_sum*beta2_sum - betax_sum*alphabeta_sum) * factor;
            Vector3 b = (betax_sum*alpha2_sum - alphax_sum*alphabeta_sum) * factor;

            // clamp to the grid
            a = clamp(a, 0, 1);
            b = clamp(b, 0, 1);
            a = floor(grid * a + 0.5f) * gridrcp;
            b = floor(grid * b + 0.5f) * gridrcp;

            // compute the error
            Vector3 e1 = a*a*alpha2_sum + b*b*beta2_sum + 2.0f*( a*b*alphabeta_sum - a*alphax_sum - b*betax_sum );

            // apply the metric to the error term
            float error = dot(e1, m_metricSqr);

            // keep the solution if it wins
            if (error < besterror)
            {
                besterror = error;
                beststart = a;
                bestend = b;
                b0 = c0;
                b1 = c1;
            }

            x1 += m_weighted[c0+c1];
            w1 += m_weights[c0+c1];
        }

        x0 += m_weighted[c0];
        w0 += m_weights[c0];
    }

    // save the block if necessary
    if( besterror < m_besterror )
    {

        *start = beststart;
        *end = bestend;

        // save the error
        m_besterror = besterror;

        return true;
    }

    return false;
}

bool ClusterFit::compress4(Vector3 * start, Vector3 * end)
{
    const uint count = m_count;
    const Vector3 grid( 31.0f, 63.0f, 31.0f );
    const Vector3 gridrcp( 1.0f/31.0f, 1.0f/63.0f, 1.0f/31.0f );

    // declare variables
    Vector3 beststart( 0.0f );
    Vector3 bestend( 0.0f );
    float besterror = FLT_MAX;

    Vector3 x0(0.0f);
    float w0 = 0.0f;
    int b0 = 0, b1 = 0, b2 = 0;

    // check all possible clusters for this total order
    for (uint c0 = 0; c0 <= count; c0++)
    {
        Vector3 x1(0.0f);
        float w1 = 0.0f;

        for (uint c1 = 0; c1 <= count-c0; c1++)
        {
            Vector3 x2(0.0f);
            float w2 = 0.0f;

            for (uint c2 = 0; c2 <= count-c0-c1; c2++)
            {
                float w3 = m_wsum - w0 - w1 - w2;

                float const alpha2_sum = w0 + w1 * (4.0f/9.0f) + w2 * (1.0f/9.0f);
                float const beta2_sum = w3 + w2 * (4.0f/9.0f) + w1 * (1.0f/9.0f);
                float const alphabeta_sum = (w1 + w2) * (2.0f/9.0f);
                float const factor = 1.0f / (alpha2_sum * beta2_sum - alphabeta_sum * alphabeta_sum);

                Vector3 const alphax_sum = x0 + x1 * (2.0f / 3.0f) + x2 * (1.0f / 3.0f);
                Vector3 const betax_sum = m_xsum - alphax_sum;

                Vector3 a = ( alphax_sum*beta2_sum - betax_sum*alphabeta_sum )*factor;
                Vector3 b = ( betax_sum*alpha2_sum - alphax_sum*alphabeta_sum )*factor;

                // clamp to the grid
                a = clamp(a, 0, 1);
                b = clamp(b, 0, 1);
                a = floor(a * grid + 0.5f) * gridrcp;
                b = floor(b * grid + 0.5f) * gridrcp;

                // compute the error
                Vector3 e1 = a*a*alpha2_sum + b*b*beta2_sum + 2.0f*( a*b*alphabeta_sum - a*alphax_sum - b*betax_sum );

                // apply the metric to the error term
                float error = dot( e1, m_metricSqr );

                // keep the solution if it wins
                if (error < besterror)
                {
                    besterror = error;
                    beststart = a;
                    bestend = b;
                    b0 = c0;
                    b1 = c1;
                    b2 = c2;
                }

                x2 += m_weighted[c0+c1+c2];
                w2 += m_weights[c0+c1+c2];
            }

            x1 += m_weighted[c0+c1];
            w1 += m_weights[c0+c1];
        }

        x0 += m_weighted[c0];
        w0 += m_weights[c0];
    }

    // save the block if necessary
    if (besterror < m_besterror)
    {
        *start = beststart;
        *end = bestend;

        // save the error
        m_besterror = besterror;

        return true;
    }

    return false;
}

#endif // NVTT_USE_SIMD
