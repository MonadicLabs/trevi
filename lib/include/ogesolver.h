#pragma once

#include "codeblock.h"
#include "sourceblock.h"
#include "decodeoutput.h"
#include "profiler.h"

#include <memory>
#include <vector>
#include <algorithm>
#include <sstream>
#include <stack>
#include <fstream>

#include <unistd.h>
#include <cstdint>

using namespace std;

// #define DEBUG_ORDER
// #define USE_LOG
// #define IMMEDIATE_MODE
// #define SKIP_BP

class OGESolver
{
public:

    OGESolver( int decodingWindowSize )
        :_decodingWindowSize(decodingWindowSize)
    {
        reset();
#ifdef DEBUG_ORDER
        _ofs.open( "/tmp/order.txt" );
#endif
    }

    virtual ~OGESolver()
    {

    }

    void addBlock( std::shared_ptr< trevi::CodeBlock > cb )
    {

#ifdef USE_PROFILING
        rmt_ScopedCPUSample(OGESolver_addBlock, 0);
#endif

        std::set< uint32_t > compo = cb->getCompositionSet();
#ifdef USE_LOG
        cerr << "@@@@@@ new input: " << endl;
        printSet( compo );
#endif
        stringstream sstr;
        sstr << "{{{{{{{{{{{{{{{{{{{{{{ " << (int)cb->get_stream_sequence_idx() << endl;
#ifdef DEBUG_ORDER
        _ofs << sstr.str();
#endif
        std::vector <uint32_t> compoVec;
        std::copy(compo.begin(), compo.end(), std::back_inserter(compoVec));

        std::vector<uint32_t> before = unique_blocks();
        addEquation( compoVec, cb->clone() );
        std::vector<uint32_t> after = unique_blocks();
        std::vector<uint32_t> diff;
        std::set_difference(after.begin(), after.end(), before.begin(), before.end(),
                            std::inserter(diff, diff.begin()));

#ifdef USE_LOG
        cerr << "*** NEW DECODED BLOCKS AFTER OGE:" << endl;
        printVector( diff );
#endif

#ifdef USE_LOG
        cerr << "AFTER OGE:" << endl;
        cerr << str() << endl;
        cerr << "****" << endl;
#endif

#ifndef SKIP_BP
        bpPass( after );
        after = unique_blocks();
        std::set_difference(after.begin(), after.end(), before.begin(), before.end(),
                            std::inserter(diff, diff.begin()));

#ifdef USE_LOG
        cerr << "*** NEW DECODED BLOCKS AFTER BP PASS:" << endl;
#endif

        removeDups( diff );

#ifdef USE_LOG
        printVector( diff );
#endif

#ifdef USE_LOG
        cerr << "*************************************" << endl;
        cerr << "AFTER BP PASS:" << endl;
        cerr << str() << endl;
        cerr << "****" << endl;
#endif
#endif

        {
            // cerr << "diff.size()=" << diff.size() << endl;
#ifndef IMMEDIATE_MODE
            for( int k = 0; k < diff.size(); ++k )
            {
                std::shared_ptr< trevi::CodeBlock > cb = _blocks[ diff[k] - _curOffset ];
                std::shared_ptr<trevi::SourceBlock> sb = std::make_shared<trevi::SourceBlock>( cb->payload_size(), (uint8_t*)cb->payload_ptr() );
                uint32_t idx = diff[k];
                trevi::DecodeOutput doutput;
                doutput.block = sb;
                doutput.stream_idx = idx;
                doutput.global_idx = sb->get_global_sequence_idx();
                _output.push_back( doutput );
            }
#endif
        }
    }

    void addEquation( std::vector<uint32_t> components, std::shared_ptr<trevi::CodeBlock> blk )
    {

#ifdef USE_PROFILING
        rmt_ScopedCPUSample(OGESolver_addEquation, 0);
#endif

        int s = -1;
        int componentSize = components.size();
        int component0 = components[0];
        int component0MinusOffset = components[0] - _curOffset;

        if( component0MinusOffset < 0 )
        {
            cerr << "wront ! component0MinusOffset = " << component0MinusOffset << endl;
            return;
        }

        int coeffsCompo0 = _coeffs[ components[0] - _curOffset ].size();
#ifdef USE_LOG
        cerr << "components.size()=" << components.size() << " components[0] - _curOffset=" << components[0] - _curOffset <<" - _coeffs[ components[0] ].size()=" << _coeffs[ components[0] - _curOffset ].size() << endl;
#endif
        while( components.size() > 0 && _coeffs[ components[0] - _curOffset ].size() > 0 )
        {
            int s = components[0];
            if( components.size() >= _coeffs[s - _curOffset].size() )
            {
                if( xorRow( s, components, blk ) )
                {
                    // cerr << "ok good. RECOVERED: " << components[0] << endl;
                }
                //                cerr << "xorRow( " << s << ", components, blk )" << endl;
                //                dump();
                //                cerr << "xor done." << endl;
                //                int popo;
                //                cin >> popo;
            }
            else
            {
                // Swap the existing rwo for the new one, reduce the existing one
                // and see if it fits elsewhere
#ifdef USE_LOG
                cerr << "swap()" << endl;
                dump();
#endif
                std::swap( components, _coeffs[s - _curOffset] );
                std::swap( _blocks[s - _curOffset ], blk );
            }
        }

        if( components.size() > 0 )
        {
            _coeffs[ components[0] - _curOffset ] = components;
            _blocks[ components[0] - _curOffset ] = blk;
        }

        //  dump_good_blocks();

    }

    bool xorRow( int s, std::vector<uint32_t>& indices, std::shared_ptr<trevi::CodeBlock> block )
    {

#ifdef USE_PROFILING
        rmt_ScopedCPUSample(OGESolver_xorRow, 0);
#endif

        std::vector<uint32_t> newIndices;
        std::vector<uint32_t> coeffs = _coeffs[s - _curOffset];
        int i = 0;
        int j = 0;
        while( i < coeffs.size() && j < indices.size() )
        {
            int index = indices[j];
            if( coeffs[i] == index )
            {
                i++;
                j++;
            } else if( coeffs[i] < index )
            {
                newIndices.push_back(coeffs[i]);
                i++;
            } else {
                newIndices.push_back( index );
                j++;
            }
        }

        newIndices.insert( newIndices.end(), coeffs.begin() + i, coeffs.end() );
        newIndices.insert( newIndices.end(), indices.begin() + j, indices.end() );

        // return newIndices, b
        indices = newIndices;
        block->XOR_payload( _blocks[s - _curOffset] );

        return (indices.size() == 1);

    }

    int undeterminedCount()
    {
        int cpt = 0;
        for( int i = 0; i < _coeffs.size(); ++i )
        {
            if( _coeffs[i].size() == 0 )
                cpt++;
        }
        return cpt;
    }


    std::string str()
    {
        std::stringstream sstr;
        sstr << "[";
        for( int i = 0; i < _coeffs.size(); ++i )
        {
            sstr << " [ ";
            for( int j = 0; j < _coeffs[i].size(); ++j )
            {
                sstr << _coeffs[i][j] << ",";
            }
            sstr << "], ";
        }
        sstr << " ]" << std::endl;
        return sstr.str();
    }

    void dump()
    {
        cerr << str() << endl;
    }

    void dump_good_blocks()
    {
        for( int i = 0; i < _coeffs.size(); ++i )
        {
            if( _coeffs[i].size() == 1 )
            {
                //                std::shared_ptr< CodeBlock > cb = _blocks[ i ];
                //                SourceBlock * sb = new SourceBlock( cb->payload_size(), (uint8_t*)cb->payload_ptr() );
                //                cerr << "GOOD_BLOCK_CRC_OK=" << sb->isCorrectCRC() << endl;
                //                delete sb;
            }
        }
    }

    void setOffset( uint32_t offset )
    {
        _curOffset = offset;
    }

    uint32_t offset()
    {
        return _curOffset;
    }

    void reset()
    {
        _curOffset = 0;
        const int pol = _decodingWindowSize;
        _coeffs = std::vector< std::vector<uint32_t> >(pol);
        _blocks = std::vector< std::shared_ptr< trevi::CodeBlock > >(pol);
    }

    void shiftWindowTo( uint32_t minValue )
    {

#ifdef USE_PROFILING
        rmt_ScopedCPUSample(OGESolver_shiftWindowTo, 0);
#endif

        int n = minValue - _curOffset;
        // cerr << "n=" << n << " minValue=" << minValue << " _curOffset=" << _curOffset << endl;
        for( int k = 0; k < n && _coeffs.size() > 0; ++k )
        {
#ifdef IMMEDIATE_MODE
            if( _coeffs[0].size() == 1 )
            {
                // cerr << "## PUSHED OUT: " << _coeffs[0][0] << endl;
                std::shared_ptr< trevi::CodeBlock > cb = _blocks[0];
                std::shared_ptr<trevi::SourceBlock> sb = std::make_shared<trevi::SourceBlock>( cb->payload_size(), (uint8_t*)cb->payload_ptr() );
                uint32_t idx = _coeffs[0][0];
                trevi::DecodeOutput doutput;
                doutput.block = sb;
                doutput.stream_idx = cb->get_stream_sequence_idx();
                doutput.global_idx = sb->get_global_sequence_idx();
                _output.push_back( doutput );
            }
#endif
            _coeffs.erase( _coeffs.begin() );
            _blocks.erase( _blocks.begin() );
        }

        std::vector<uint32_t> nv;
        for( int k = 0; k < min(_decodingWindowSize, n); ++k )
        {
            _coeffs.push_back(nv);
            _blocks.push_back(nullptr);
        }

        _curOffset = minValue;

    }

    std::vector< uint32_t > unique_blocks()
    {
#ifdef USE_PROFILING
        rmt_ScopedCPUSample(OGESolver_unique_blocks, 0);
#endif
        // cerr << "unique_blocks: n = " << _coeffs.size() << endl;

        std::vector< uint32_t > ret;
        for( std::vector< uint32_t > vv : _coeffs )
        {
            if( vv.size() == 1 )
            {
                ret.push_back( vv[0] );
            }
        }
        return ret;
    }

    void propagateBelief( std::vector<uint32_t> blist )
    {

#ifdef USE_PROFILING
        rmt_ScopedCPUSample(OGESolver_propagateBelief, 0);
#endif

        for( uint32_t blockIdx : blist )
        {
            std::shared_ptr< trevi::CodeBlock > cb = _blocks[ blockIdx - _curOffset ];

            // Loop through all coeffs
            for( int i = blockIdx - _curOffset; i >= 0; --i )
            {
                // Try to find a composed block containing blockIdx
                if( _coeffs[i].size() > 1 )
                {
                    if(std::find(_coeffs[i].begin(), _coeffs[i].end(), blockIdx) != _coeffs[i].end())
                    {
#ifdef USE_LOG
                        printVector( _coeffs[i] );
                        cerr << "---- contains " << blockIdx << endl;
#endif
                        // Do something - XOR the block blockIdx from the found block
                        std::shared_ptr< trevi::CodeBlock > block = _blocks[ i ];
                        block->XOR_payload( cb );

#ifdef USE_LOG
                        cerr << "propagating " <<  blockIdx << " to..." << endl;
                        cerr << "before:" << endl;
                        printVector( _coeffs[i] );
#endif

                        // Remove reference from coeffs..
                        _coeffs[i].erase(std::remove(_coeffs[i].begin(), _coeffs[i].end(), blockIdx), _coeffs[i].end());

#ifdef USE_LOG
                        cerr << "after:" << endl;
                        printVector( _coeffs[i] );
#endif
                    }
                }
            }
        }
    }

    void bpPass( std::vector< uint32_t > initialList )
    {

#ifdef USE_PROFILING
        rmt_ScopedCPUSample(OGESolver_bpPass, 0);
#endif
        propagateBelief(initialList);

    }

    std::deque< trevi::DecodeOutput > _output;

private:
    std::vector< std::vector<uint32_t> >        _coeffs;
    std::vector< std::shared_ptr<trevi::CodeBlock> >   _blocks;

    int _decodingWindowSize;
    uint32_t _curOffset;

#ifdef DEBUG_ORDER
    ofstream _ofs;
#endif

protected:

};
