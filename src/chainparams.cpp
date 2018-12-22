// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Copyright (c) 2015 The BitDaric Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"

#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

using namespace std;

#include "chainparamsseeds.h"

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */

class CMainParams : public CChainParams {
protected:
    Consensus::Params digishieldConsensus;
    Consensus::Params auxpowConsensus;
public:
    CMainParams() {
        strNetworkID = "main";

        // Blocks 0 - 5 are conventional difficulty calculation
        consensus.nSubsidyHalvingInterval = 7000000;
        consensus.nMajorityEnforceBlockUpgrade = 1500;
        consensus.nMajorityRejectBlockOutdated = 1900;
        consensus.nMajorityWindow = 2000;
        consensus.powLimit = uint256S("0x00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 20;
        consensus.nPowTargetTimespan = 1 * 24 * 60 * 60; // (Number of minutes before difficulty of the network is re-adjusted)
        												//  (every 1440 blocks)
        consensus.nPowTargetSpacing = 60; // (Number of minutes it should take to mine a block)
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowAllowDigishieldMinDifficultyBlocks = false;
        consensus.fSimplifiedRewards = true;
        consensus.nAuxpowChainId = 0x0062; // 98 - Josh Wise!
        consensus.fStrictChainId = true;
        consensus.fAllowLegacyBlocks = true;
        consensus.fAllowAuxPow = false;
        consensus.nHeightEffective = 0;
        consensus.fDigishieldDifficultyCalculation = false;
        consensus.nCoinbaseMaturity = 1; // (Number of blocks before a mined block can be spend)

        // Digishield without AuxPoW
        digishieldConsensus = consensus;
        digishieldConsensus.nHeightEffective = 5;
        digishieldConsensus.fSimplifiedRewards = true;
        digishieldConsensus.fDigishieldDifficultyCalculation = true;
        digishieldConsensus.nPowTargetTimespan =  1 * 24 * 60 * 60; // (Number of minutes before difficulty of the network is re-adjusted)
        digishieldConsensus.nCoinbaseMaturity = 240;

        // AuxPoW
        auxpowConsensus = digishieldConsensus;
        auxpowConsensus.nHeightEffective = 10;
        auxpowConsensus.fAllowLegacyBlocks = false;
        auxpowConsensus.fAllowAuxPow = true;

        // Assemble the binary search tree of consensus parameters
        pConsensusRoot = &digishieldConsensus;
        digishieldConsensus.pLeft = &consensus;
        digishieldConsensus.pRight = &auxpowConsensus;

        /** 
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0xc9;
        pchMessageStart[1] = 0xc0;
        pchMessageStart[2] = 0xc0;
        pchMessageStart[3] = 0xc0;
        vAlertPubKey = ParseHex("p2gywa4uhd5y6whs17ikygkeyij27veg5o6id0fwsxjcco333plbr67jn2chtve0ln7kt9hgmrj9w0z5zesb5fu6bt68c4xcs3eek1i1c9kiux7ipy9ceyzork3cyfr2wy");
        nDefaultPort = 22501;
        nMinerThreads = 0;
        nPruneAfterHeight = 100000;

        /**
         * Build the genesis block. Note that the output of its generation
         * transaction cannot be spent since it did not originally exist in the
         * database.
         *
         * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1386325540, nBits=0x1e0ffff0, nNonce=99943, vtx=1)
         *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
         *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
         *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
         *   vMerkleTree: 4a5e1e
         */
        string pszTimestamp_str = "NTQ2IEJDOiBDeXJ1cyB0aGUgR3JlYXQgaW50cm9kdWNlZCBjb2lucyB0byB0aGUgUGVyc2lhbiBFbXBpcmUu";
        string pszTimestamp_decode = DecodeBase64(pszTimestamp_str);
        const char* pszTimestamp = pszTimestamp_decode.c_str();
        CMutableTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 7 * COIN;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f") << OP_CHECKSIG;
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock.SetNull();
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion.SetGenesisVersion(1);
        genesis.nTime    = 1516893938;
        genesis.nBits    = 0x1e0ffff0;
        genesis.nNonce   = 16694;

        consensus.hashGenesisBlock = genesis.GetHash();
        digishieldConsensus.hashGenesisBlock = consensus.hashGenesisBlock;
        auxpowConsensus.hashGenesisBlock = consensus.hashGenesisBlock;
        assert(consensus.hashGenesisBlock == uint256S("0xeb32c4b2f6f4abda7b87ba4326c59d27b9cc9b3a7cd99636ee1e164a3acb0621"));
        assert(genesis.hashMerkleRoot == uint256S("0xe57a762b6fd741a151d13d09eec6a5362cef74204e3112d87ad698ae89384df8"));

        vSeeds.push_back(CDNSSeedData("seed.bitdaric.live", "server1.bitdaric.live"));
        vSeeds.push_back(CDNSSeedData("seed2.bitdaric.live", "server2.bitdaric.live"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,60);  // 0x3c (R)
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,22);  // 0x16
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,158); // 0x9e
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xb2)(0x1e).convert_to_container<std::vector<unsigned char> >(); //xpub
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xad)(0xe4).convert_to_container<std::vector<unsigned char> >(); //xprv

        //TODO: fix this for bitdaric -- plddr
        //vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));
        vFixedSeeds.clear();

        fRequireRPCPassword = true;
        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (Checkpoints::CCheckpointData) {
            boost::assign::map_list_of
            (1, 		uint256S("0x0f1a08ad0267977fd2ef81585782a08398e872cd1e7e7fdac14c0050a59659c4"))
            (2, 		uint256S("0x552b897d864738866a8c1cccd09275267ba736a476a939c6f8ef2f143cdb3984"))
            (3, 		uint256S("0x2824da95d3cbacdaed0e1c6eda5bb2b9e08a3df700dfc5544f4df0e8e1b456e3"))
            (4, 		uint256S("0xe2b8db75be7982d66c7e908d7eabcb80699aa10e294e83e17ad684bd6ecff5aa"))
            (5, 		uint256S("0x8ee4a46dee485b621c41ed8f89f92aadf7eb3a185f5794c95d34dec7fdbd6364"))
            (6, 		uint256S("0x5cd932420ef81f30579c2676fd5fc7ff2f13251069f30ade1360bff99921a1d9"))
            (7, 		uint256S("0xd0d4f3de90c6c562d1771b08c8eb5929929936839e624dc1ae6158f1e1194bc5"))
            (98701,		uint256S("0x12cff61a4fba06c6b7e4a06d5905168639e0bd4fefe70d8231835f4fcf7d3ace"))
            (125910,	uint256S("0x666f2b3f204f0501c6e48bbedcd99c008c47a0c461bd8b1c6b91917f87e49560"))
            (141575,	uint256S("0x9db26acd367237911ef49198abc5c321f80a64dacb9ed660d5191c93fea1cccf")),
            1518514125, 	// * UNIX timestamp of last checkpoint block
            142710,    		// * total number of transactions between genesis and last checkpoint
                        	//   (the tx=... number in the SetBestChain debug.log lines)
            1440.0     		// * estimated number of transactions per day after checkpoint
        };
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CMainParams {
private:
    Consensus::Params minDifficultyConsensus;
public:
    CTestNetParams() {
        strNetworkID = "test";

        // pre-Digishield
        consensus.nPowTargetTimespan = 1 * 24 * 60 * 60; // (Number of minutes before difficulty of the network is re-adjusted)
        consensus.fDigishieldDifficultyCalculation = false;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowAllowDigishieldMinDifficultyBlocks = false;
        consensus.nMajorityEnforceBlockUpgrade = 501;
        consensus.nMajorityRejectBlockOutdated = 750;
        consensus.nMajorityWindow = 1000;
        consensus.fStrictChainId = false;
        consensus.nHeightEffective = 0;
        consensus.fAllowLegacyBlocks = true;
        consensus.fAllowAuxPow = false;

        // Reset links before we copy parameters
        consensus.pLeft = NULL;
        consensus.pRight = NULL;

        // Digishield without minimum difficulty on all blocks
        digishieldConsensus = consensus;
        digishieldConsensus.nHeightEffective = 5;
        digishieldConsensus.nPowTargetTimespan = 1 * 24 * 60 * 60; // (Number of minutes before difficulty of the network is re-adjusted)
        digishieldConsensus.fDigishieldDifficultyCalculation = true;
        digishieldConsensus.fSimplifiedRewards = true;
        digishieldConsensus.fPowAllowMinDifficultyBlocks = false;
        digishieldConsensus.nCoinbaseMaturity = 240;

        // Digishield with minimum difficulty on all blocks
        minDifficultyConsensus = digishieldConsensus;
        minDifficultyConsensus.nHeightEffective = 10;
        minDifficultyConsensus.fPowAllowDigishieldMinDifficultyBlocks = true;
        minDifficultyConsensus.fPowAllowMinDifficultyBlocks = true;

        // Enable AuxPoW at 158100
        auxpowConsensus = minDifficultyConsensus;
        auxpowConsensus.nHeightEffective = 15;
        auxpowConsensus.fPowAllowDigishieldMinDifficultyBlocks = true;
        auxpowConsensus.fAllowLegacyBlocks = false;
        auxpowConsensus.fAllowAuxPow = true;

        // Assemble the binary search tree of parameters
        pConsensusRoot = &digishieldConsensus;
        digishieldConsensus.pLeft = &consensus;
        digishieldConsensus.pRight = &minDifficultyConsensus;
        minDifficultyConsensus.pRight = &auxpowConsensus;

        pchMessageStart[0] = 0x0b;
        pchMessageStart[1] = 0xc1;
        pchMessageStart[2] = 0xb7;
        pchMessageStart[3] = 0xdc;
        vAlertPubKey = ParseHex("lehnac8ujsh2sim4hsh8ded3k7ud3c9wem5hcnnnsfnqg72sd5gzhj1ypd6w14i6eg7ndxpr4nxs9mroq31g3w6uekchb5zypwiz5knvmu158bajl31vrv3qau1cdqbo32");
        nDefaultPort = 22601;
        nMinerThreads = 0;
        nPruneAfterHeight = 1000;

        //! Modify the testnet genesis block so the timestamp is valid for a later start.
        genesis.nTime = 1516893938;
        genesis.nNonce = 16694;
        consensus.hashGenesisBlock = genesis.GetHash();
        digishieldConsensus.hashGenesisBlock = consensus.hashGenesisBlock;
        minDifficultyConsensus.hashGenesisBlock = consensus.hashGenesisBlock;
        auxpowConsensus.hashGenesisBlock = consensus.hashGenesisBlock;
        assert(consensus.hashGenesisBlock == uint256S("0xeb32c4b2f6f4abda7b87ba4326c59d27b9cc9b3a7cd99636ee1e164a3acb0621"));

        vFixedSeeds.clear();
        vSeeds.clear();
        vSeeds.push_back(CDNSSeedData("testseed.bitdaric.live", "testserver1.bitdaric.live"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,127); // 0x7f (t)
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196); // 0xc4
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,241); // 0xf1
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x35)(0x87)(0xcf).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x35)(0x83)(0x94).convert_to_container<std::vector<unsigned char> >();

        //TODO: fix this for bitdaric -- plddr
        //vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fRequireRPCPassword = true;
        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        checkpointData = (Checkpoints::CCheckpointData) {
            boost::assign::map_list_of
            (0, uint256S("0x")),
            0, // * UNIX timestamp of last checkpoint block
            0,    // * total number of transactions between genesis and last checkpoint
            0        // * estimated number of transactions per day after checkpoint
        };

    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CTestNetParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        consensus.nSubsidyHalvingInterval = 1;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        consensus.nPowTargetTimespan = 1 * 24 * 60 * 60; // (Number of minutes before difficulty of the network is re-adjusted)
        consensus.nPowTargetSpacing = 60; // (Number of minutes it should take to mine a block)
        consensus.powLimit = uint256S("0x7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // ~uint256(0) >> 1;
        consensus.fStrictChainId = true;
        consensus.fAllowLegacyBlocks = true;
        consensus.fAllowAuxPow = false;
        consensus.fSimplifiedRewards = true;
        consensus.nCoinbaseMaturity = 60; // For easier testability in RPC tests

        // Reset links before we copy parameters
        consensus.pLeft = NULL;
        consensus.pRight = NULL;

        digishieldConsensus = consensus;
        digishieldConsensus.nHeightEffective = 10;
        digishieldConsensus.nPowTargetTimespan = 1 * 24 * 60 * 60; // (Number of minutes before difficulty of the network is re-adjusted)
        digishieldConsensus.fDigishieldDifficultyCalculation = true;

        auxpowConsensus = digishieldConsensus;
        auxpowConsensus.fAllowLegacyBlocks = false;
        auxpowConsensus.fAllowAuxPow = true;
        auxpowConsensus.nHeightEffective = 20;

        // Assemble the binary search tree of parameters
        digishieldConsensus.pLeft = &consensus;
        digishieldConsensus.pRight = &auxpowConsensus;
        pConsensusRoot = &digishieldConsensus;

        pchMessageStart[0] = 0xfa;
        pchMessageStart[1] = 0xbf;
        pchMessageStart[2] = 0xb5;
        pchMessageStart[3] = 0xda;
        nMinerThreads = 1;
        genesis.nTime = 1516893938;
        genesis.nBits = 0x207fffff;
        genesis.nNonce = 16694;
        consensus.hashGenesisBlock = genesis.GetHash();
        digishieldConsensus.hashGenesisBlock = consensus.hashGenesisBlock;
        auxpowConsensus.hashGenesisBlock = consensus.hashGenesisBlock;
        nDefaultPort = 18444;
        assert(consensus.hashGenesisBlock == uint256S("0x01ae708929489b3796ca343cfbbf0e03b8878da06b32b8879d0f8273b992a6d0"));
        nPruneAfterHeight = 1000;

        vFixedSeeds.clear(); //! Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();  //! Regtest mode doesn't have any DNS seeds.

        fRequireRPCPassword = false;
        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (Checkpoints::CCheckpointData){
            boost::assign::map_list_of
            ( 0, uint256S("0x")),
            0,
            0,
            0
        };
    }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = 0;

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams &Params(CBaseChainParams::Network network) {
    switch (network) {
        case CBaseChainParams::MAIN:
            return mainParams;
        case CBaseChainParams::TESTNET:
            return testNetParams;
        case CBaseChainParams::REGTEST:
            return regTestParams;
        default:
            assert(false && "Unimplemented network");
            return mainParams;
    }
}

void SelectParams(CBaseChainParams::Network network) {
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

bool SelectParamsFromCommandLine()
{
    CBaseChainParams::Network network = NetworkIdFromCommandLine();
    if (network == CBaseChainParams::MAX_NETWORK_TYPES)
        return false;

    SelectParams(network);
    return true;
}
