// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Copyright (c) 2014-2015 The BitDaric Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "pow.h"

#include "arith_uint256.h"
#include "chain.h"
#include "bitdaric.h"
#include "timedata.h"
#include "primitives/block.h"
#include "uint256.h"
#include "util.h"

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();

    // Genesis block
    if (pindexLast == NULL)
        return nProofOfWorkLimit;

    // BitDaric: Special rules for minimum difficulty blocks with Digishield
    if (AllowDigishieldMinDifficultyForBlock(pindexLast, pblock, params))
    {
        // Special difficulty rule for testnet:
        // If the new block's timestamp is more than 2* nTargetSpacing minutes
        // then allow mining of a min-difficulty block.
        return nProofOfWorkLimit;
    }

	if(pindexLast->nHeight < EDA_EFFECTIVE_HEIGHT) {
	//LogPrintf("EDA not effective nHeight = %d\n",pindexLast->nHeight);

	    // Only change once per difficulty adjustment interval
	    if ((pindexLast->nHeight+1) % params.DifficultyAdjustmentInterval() != 0)
	    {
	        if (params.fPowAllowMinDifficultyBlocks)
	        {
	            // Special difficulty rule for testnet:
	            // If the new block's timestamp is more than 2* 10 minutes
	            // then allow mining of a min-difficulty block.
	            if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing*2)
	                return nProofOfWorkLimit;
	            else
	            {
	                // Return the last non-special-min-difficulty-rules-block
	                const CBlockIndex* pindex = pindexLast;
	                while (pindex->pprev && pindex->nHeight % params.DifficultyAdjustmentInterval() != 0 && pindex->nBits == nProofOfWorkLimit)
	                    pindex = pindex->pprev;
	                return pindex->nBits;
	            }
	        }
	        return pindexLast->nBits;
	    }

	    // Litecoin: This fixes an issue where a 51% attack can change difficulty at will.
	    // Go back the full period unless it's the first retarget after genesis. Code courtesy of Art Forz
	    int blockstogoback = params.DifficultyAdjustmentInterval()-1;
	    if ((pindexLast->nHeight+1) != params.DifficultyAdjustmentInterval())
	        blockstogoback = params.DifficultyAdjustmentInterval();

	    // Go back by what we want to be 14 days worth of blocks
	    int nHeightFirst = pindexLast->nHeight - blockstogoback;
	    assert(nHeightFirst >= 0);
	    const CBlockIndex* pindexFirst = pindexLast->GetAncestor(nHeightFirst);
	    assert(pindexFirst);

	    return CalculateBitDaricNextWorkRequired(pindexLast, pindexFirst->GetBlockTime(), params);
	} else {
		// Emergency Difficulty Adjustement (EDA)
		//LogPrintf("EDA effective nHeight = %d\n", pindexLast->nHeight);

		// Stalled Network Recovery (SNR)
        // If the new block's timestamp is more than 72h
        // then allow mining of a min-difficulty block.
	    if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + 72 * 3600)
	    {
    		//LogPrintf("SNR effective nHeight = %d\n", pindexLast->nHeight);
    		return nProofOfWorkLimit;
		}
		
	    // If producing the last 6 block took less than 12h, we keep the same
	    // difficulty.
	    const CBlockIndex *pindex6 = pindexLast->GetAncestor(pindexLast->nHeight - 7);
	    assert(pindex6);
	    int64_t mtp6blocks = pindexLast->GetMedianTimePast() - pindex6->GetMedianTimePast();

	    if (mtp6blocks < 12 * 3600)
	    {
			//LogPrintf("EDA keep the same diff. mtp = %d\n", mtp6blocks);
		    // Only change once per difficulty adjustment interval
		    if ((pindexLast->nHeight+1) % params.DifficultyAdjustmentInterval() != 0)
		    {
		        if (params.fPowAllowMinDifficultyBlocks)
		        {
		            // Special difficulty rule for testnet:
		            // If the new block's timestamp is more than 2* 10 minutes
		            // then allow mining of a min-difficulty block.
		            if (pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing*2)
		                return nProofOfWorkLimit;
		            else
		            {
		                // Return the last non-special-min-difficulty-rules-block
		                const CBlockIndex* pindex = pindexLast;
		                while (pindex->pprev && pindex->nHeight % params.DifficultyAdjustmentInterval() != 0 && pindex->nBits == nProofOfWorkLimit)
		                    pindex = pindex->pprev;
		                return pindex->nBits;
		            }
		        }
		        return pindexLast->nBits;
		    }

	    	// Litecoin: This fixes an issue where a 51% attack can change difficulty at will.
	    	// Go back the full period unless it's the first retarget after genesis. Code courtesy of Art Forz
	    	int blockstogoback = params.DifficultyAdjustmentInterval()-1;
	    	if ((pindexLast->nHeight+1) != params.DifficultyAdjustmentInterval())
	        	blockstogoback = params.DifficultyAdjustmentInterval();
		
	    	// Go back by what we want to be 14 days worth of blocks
	    	int nHeightFirst = pindexLast->nHeight - blockstogoback;
	    	assert(nHeightFirst >= 0);
	    	const CBlockIndex* pindexFirst = pindexLast->GetAncestor(nHeightFirst);
	    	assert(pindexFirst);
		
	    	return CalculateBitDaricNextWorkRequired(pindexLast, pindexFirst->GetBlockTime(), params);
		}
		//LogPrintf("EDA begin\n");

	   	// If producing the last 6 block took more than 12h, increase the difficulty
	   	// target by 1/4 (which reduces the difficulty by 20%). This ensure the
	   	// chain do not get stuck in case we lose hashrate abruptly.
	   	arith_uint256 nPow;
	   	nPow.SetCompact(pindexLast->nBits);

	    if (mtp6blocks >= 12 * 3600)
	    {
	    	//LogPrintf("EDA set\n");
	    	nPow += (nPow >> 2);
		}

	    // Make sure we do not go bellow allowed values.
	    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
	    if (nPow > bnPowLimit)
	        nPow = bnPowLimit;
		
    	//LogPrintf("EDA end\n");
	    return nPow.GetCompact();
	}
}

unsigned int CalculateNextWorkRequired(const CBlockIndex* pindexLast, int64_t nFirstBlockTime, const Consensus::Params& params)
{
    // Limit adjustment step
    int64_t nActualTimespan = pindexLast->GetBlockTime() - nFirstBlockTime;
    if (nActualTimespan < params.nPowTargetTimespan/4)
        nActualTimespan = params.nPowTargetTimespan/4;
    if (nActualTimespan > params.nPowTargetTimespan*4)
        nActualTimespan = params.nPowTargetTimespan*4;

    // Retarget
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    arith_uint256 bnNew;
    arith_uint256 bnOld;
    bnNew.SetCompact(pindexLast->nBits);
    bnOld = bnNew;
    bnNew *= nActualTimespan;
    bnNew /= params.nPowTargetTimespan;

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    /// debug print
    LogPrintf("GetNextWorkRequired RETARGET\n");
    LogPrintf("params.nPowTargetTimespan = %d    nActualTimespan = %d\n", params.nPowTargetTimespan, nActualTimespan);
    LogPrintf("Before: %08x  %s\n", pindexLast->nBits, bnOld.ToString());
    LogPrintf("After:  %08x  %s\n", bnNew.GetCompact(), bnNew.ToString());

    return bnNew.GetCompact();
}

bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(params.powLimit))
        return error("CheckProofOfWork(): nBits below minimum work");

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > bnTarget)
        return error("CheckProofOfWork(): hash doesn't match nBits");

    return true;
}

arith_uint256 GetBlockProof(const CBlockIndex& block)
{
    arith_uint256 bnTarget;
    bool fNegative;
    bool fOverflow;
    bnTarget.SetCompact(block.nBits, &fNegative, &fOverflow);
    if (fNegative || fOverflow || bnTarget == 0)
        return 0;
    // We need to compute 2**256 / (bnTarget+1), but we can't represent 2**256
    // as it's too large for a arith_uint256. However, as 2**256 is at least as large
    // as bnTarget+1, it is equal to ((2**256 - bnTarget - 1) / (bnTarget+1)) + 1,
    // or ~bnTarget / (nTarget+1) + 1.
    return (~bnTarget / (bnTarget + 1)) + 1;
}

int64_t GetBlockProofEquivalentTime(const CBlockIndex& to, const CBlockIndex& from, const CBlockIndex& tip, const Consensus::Params& params)
{
    arith_uint256 r;
    int sign = 1;
    if (to.nChainWork > from.nChainWork) {
        r = to.nChainWork - from.nChainWork;
    } else {
        r = from.nChainWork - to.nChainWork;
        sign = -1;
    }
    r = r * arith_uint256(params.nPowTargetSpacing) / GetBlockProof(tip);
    if (r.bits() > 63) {
        return sign * std::numeric_limits<int64_t>::max();
    }
    return sign * r.GetLow64();
}
