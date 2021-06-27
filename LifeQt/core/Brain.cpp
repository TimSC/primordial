// Brain.cpp
//
// Implements the biot brain
//
#include "Rand.h"
#include "Genotype.h"
#include "Brain.h"
#include "Environ.h"
#include "Biots.h"
#include <iostream>

using namespace rapidjson;

//
// ProductTerm (Picks a subset sensor bits that must be true or false)
//

void ProductTerm::Mutate(int nChance)
{
	Randomizer rand;

	if (rand.Int1024() < nChance)
        m_dwMask = rand.Uint32();

	if (rand.Int1024() < nChance)
        m_dwInvert = rand.Uint32();
}


void ProductTerm::Randomize()
{
	Randomizer rand;

    m_dwMask   = rand.Uint32();
    m_dwInvert = rand.Uint32();
}

// Helpful settings during the debug sessions
void ProductTerm::Debug()
{
	m_dwMask = 0xFFFFFFFF;
	m_dwInvert = 0x00000000;
}

void ProductTerm::SerializeJson(rapidjson::Document &d, rapidjson::Value &v)
{
    Document::AllocatorType& allocator = d.GetAllocator();

    v.AddMember("m_dwMask", m_dwMask, allocator);
    v.AddMember("m_dwInvert", m_dwInvert, allocator);
}

void ProductTerm::SerializeJsonLoad(const rapidjson::Value& v)
{
    m_dwMask = v["m_dwMask"].GetUint();
    m_dwInvert = v["m_dwInvert"].GetUint();
}

//
// ProductArray (Contains an array of product definitions)
//

void ProductArray::Mutate(int nChance)
{
	for (int i = 0; i < GetTermCount(); i++)
		m_productTerm[i].Mutate(nChance);

	for (int i = 0; i < GetSumCount(); i++)
		m_productSum[i].Mutate(nChance);
}


void ProductArray::Randomize()
{
	for (int i = 0; i < GetTermCount(); i++)
		m_productTerm[i].Randomize();

	for (int i = 0; i < GetSumCount(); i++)
		m_productSum[i].Randomize();
}


void ProductArray::Debug()
{
	for (int i = 0; i < GetTermCount(); i++)
		m_productTerm[i].Debug();

	for (int i = 0; i < GetSumCount(); i++)
		m_productSum[i].Debug();
}

void ProductArray::SerializeJson(rapidjson::Document &d, rapidjson::Value &v)
{
    Document::AllocatorType& allocator = d.GetAllocator();

    Value prodTermArr(kArrayType);
    for (int i = 0; i < GetTermCount(); i++)
    {
        Value obj(kObjectType);
        m_productTerm[i].SerializeJson(d, obj);
        prodTermArr.PushBack(obj, allocator);
    }
    v.AddMember("m_productTerm", prodTermArr, allocator);

    Value prodSumArr(kArrayType);
    for (int i = 0; i < GetSumCount(); i++)
    {
        Value obj(kObjectType);
        m_productSum[i].SerializeJson(d, obj);
        prodSumArr.PushBack(obj, allocator);
    }
    v.AddMember("m_productSum", prodSumArr, allocator);
}

void ProductArray::SerializeJsonLoad(const rapidjson::Value& v)
{
    const Value &pt = v["m_productTerm"];
    for (int i = 0; i < pt.Size() and i < MAX_PRODUCT_TERMS; i++)
        m_productTerm[i].SerializeJsonLoad(pt[i]);

    const Value &ps = v["m_productSum"];
    for (int i = 0; i < ps.Size() and i < MAX_PRODUCT_SUMS; i++)
        m_productSum[i].SerializeJsonLoad(ps[i]);
}

void ProductArray::Crossover(ProductArray& productArray)
{
	Randomizer rand;

	for (int i = 0; i < GetTermCount(); i++)
		if (rand.Bool())
			m_productTerm[i] = productArray.m_productTerm[i];

	// Product sums are currently allowed to crossover at the term level.
	for (int i = 0; i < GetSumCount(); i++)
		m_productSum[i].Crossover(productArray.m_productSum[i]);
}

ProductArray& ProductArray::operator=(ProductArray& productArray)
{
	for (int i = 0; i < GetTermCount(); i++)
		m_productTerm[i] = productArray.m_productTerm[i];

	for (int i = 0; i < GetSumCount(); i++)
		m_productSum[i] = productArray.m_productSum[i];

	return *this;
}


//
// ProductSum  (Each command refers to a set of product terms
// defined in the ProductArray)
//
void ProductSum::Mutate(int nChance)
{
	Randomizer rand;

	for (int i = 0; i < GetCount(); i++)
		if (rand.Int1024() < nChance)
			m_reference[i] = rand.Byte(ProductArray::MAX_PRODUCT_TERMS);

	if (rand.Int1024() < nChance)
		m_bTrue = rand.Bool();
}

void ProductSum::Randomize()
{
	Randomizer rand;

	for (int i = 0; i < GetCount(); i++)
		m_reference[i] = rand.Byte(ProductArray::MAX_PRODUCT_TERMS);

	m_bTrue = rand.Bool();
}


void ProductSum::Debug()
{
	for (int i = 0; i < GetCount(); i++)
        m_reference[i] = (uint8_t) i;

    m_bTrue = true;
}

void ProductSum::SerializeJson(rapidjson::Document &d, rapidjson::Value &v)
{
    Document::AllocatorType& allocator = d.GetAllocator();

    Value refArrayJson(kArrayType);
    for (int i = 0; i < GetCount(); i++)
        refArrayJson.PushBack(m_reference[i], allocator);
    v.AddMember("m_reference", refArrayJson, allocator);

    v.AddMember("m_bTrue", m_bTrue, allocator);
}

void ProductSum::SerializeJsonLoad(const rapidjson::Value& v)
{
    const Value &ref = v["m_reference"];
    for (int i = 0; i < ref.Size() and i < MAX_PRODUCT_SUM_TERMS; i++)
         m_reference[i] = ref[i].GetUint();

    m_bTrue = v["m_bTrue"].GetBool();
}

void ProductSum::Crossover(ProductSum& productSum)
{
	Randomizer rand;

	for (int i = 0; i < GetCount(); i++)
		if (rand.Bool())
			m_reference[i] = productSum.m_reference[i];

	if (rand.Bool())
		m_bTrue = productSum.m_bTrue;
}


ProductSum& ProductSum::operator=(ProductSum& productSum)
{
	for (int i = 0; i < GetCount(); i++)
		m_reference[i] = productSum.m_reference[i];

	m_bTrue = productSum.m_bTrue;
	return *this;
}

// Essentially ORs all the product terms together.  The first
// true result short circuits the evaluation and returns
// immediately.
bool ProductSum::IsTrue(ProductArray& productArray, uint32_t dwSensor)
{
	for (int i = 0; i < GetCount(); i++)
		if (productArray[m_reference[i]].IsTrue(dwSensor))
		{
			BTRACE1("TRUE (%d)\n", (int) m_bTrue);
			return (m_bTrue != 0);
		}
	BTRACE1("FALSE (%d)\n", (int) m_bTrue);
	return (m_bTrue == 0);
}



//
// CommandArgument
//
//

void CommandArgument::SerializeJson(rapidjson::Document &d, rapidjson::Value &v)
{
    Document::AllocatorType& allocator = d.GetAllocator();

    v.AddMember("m_command", Value(m_command), allocator);
    v.AddMember("m_limb", Value(m_limb), allocator);
    v.AddMember("m_segment", Value(m_segment), allocator);
    v.AddMember("m_rate", Value(m_rate), allocator);
    v.AddMember("m_degrees", Value(m_degrees), allocator);
}

void CommandArgument::SerializeJsonLoad(const rapidjson::Value& v)
{
    m_command = v["m_command"].GetInt();
    m_limb = v["m_limb"].GetUint();
    m_segment = v["m_segment"].GetUint();
    m_rate = v["m_rate"].GetUint();
    m_degrees = v["m_degrees"].GetUint();
}

void CommandArgument::Randomize(void)
{
	Randomizer rand;
	m_command    = rand.Integer(COMMAND_MAX_TYPES);
	m_rate       = rand.Byte();
	m_degrees    = rand.Byte();
    m_limb       = rand.Byte(MAX_LIMBS);
	m_segment    = rand.Byte(MAX_SEGMENTS);
}

void CommandArgument::Mutate(int chance)
{
	Randomizer rand;

	if (rand.Int1024() < chance)
		m_command    = rand.Integer(COMMAND_MAX_TYPES);

	if (rand.Int1024() < chance)
		m_rate       = rand.Byte();

	if (rand.Int1024() < chance)
		m_degrees    = rand.Byte();

	if (rand.Int1024() < chance)
        m_limb       = rand.Byte(MAX_LIMBS);

	if (rand.Int1024() < chance)
		m_segment    = rand.Byte(MAX_SEGMENTS);


}


int CommandArgument::GetLimb(int actualLimb)
{ 
    if (m_limb == MAX_LIMBS)
		return (int) m_limb;

    if ((int) m_limb + actualLimb >= MAX_LIMBS)
        return (int) m_limb + actualLimb - MAX_LIMBS;
	 else
		return (int) m_limb + actualLimb;
}

//
// CommandArray (Contains an array of command definitions)
//

void CommandArray::Mutate(int nChance)
{
	for (int i = 0; i < GetCommandCount(); i++)
		m_command[i].Mutate(nChance);

	m_productArray.Mutate(nChance);

	for (int i = 0; i < GetTypeCount(); i++)
		m_commandLimbType[i].Mutate(nChance);
}


void CommandArray::Randomize()
{
	for (int i = 0; i < GetCommandCount(); i++)
		m_command[i].Randomize();

	m_productArray.Randomize();

	for (int i = 0; i < GetTypeCount(); i++)
		m_commandLimbType[i].Randomize();
}

void CommandArray::SerializeJson(rapidjson::Document &d, rapidjson::Value &v)
{
    Document::AllocatorType& allocator = d.GetAllocator();

    Value commArrJson(kArrayType);
    for (int i = 0; i < GetCommandCount(); i++)
    {
        Value commJson(kObjectType);
        m_command[i].SerializeJson(d, commJson);
        commArrJson.PushBack(commJson, allocator);
    }
    v.AddMember("m_command", commArrJson, allocator);

    Value prodArrJson(kObjectType);
    m_productArray.SerializeJson(d, prodArrJson);
    v.AddMember("m_productArray", prodArrJson, allocator);

    Value commLimbArrJson(kArrayType);
    for (int i = 0; i < GetTypeCount(); i++)
    {
        Value commJson(kObjectType);
        m_commandLimbType[i].SerializeJson(d, commJson);
        commLimbArrJson.PushBack(commJson, allocator);
    }
    v.AddMember("m_commandLimbType", commLimbArrJson, allocator);
}

void CommandArray::SerializeJsonLoad(const rapidjson::Value& v)
{
    const Value &commArrJson = v["m_command"];
    for (int i = 0; i < commArrJson.Size() and i < MAX_COMMANDS; i++)
        m_command[i].SerializeJsonLoad(commArrJson[i]);

    m_productArray.SerializeJsonLoad(v["m_productArray"]);

    const Value &commLimbArrJson = v["m_commandLimbType"];
    for (int i = 0; i < commLimbArrJson.Size() and i < MAX_LIMB_TYPES; i++)
        m_commandLimbType[i].SerializeJsonLoad(commLimbArrJson[i]);

}

void CommandArray::Crossover(CommandArray& commandArray)
{
	Randomizer rand;

	for (int i = 0; i < GetCommandCount(); i++)
		if (rand.Bool())
			m_command[i] = commandArray.m_command[i];

	m_productArray.Crossover(commandArray.m_productArray);

	for (int i = 0; i < GetTypeCount(); i++)
		m_commandLimbType[i].Crossover(commandArray.m_commandLimbType[i]);
}

CommandArray& CommandArray::operator=(CommandArray& commandArray)
{
	for (int i = 0; i < GetCommandCount(); i++)
		m_command[i] = commandArray.m_command[i];

	m_productArray = commandArray.m_productArray;

	for (int i = 0; i < GetTypeCount(); i++)
		m_commandLimbType[i] = commandArray.m_commandLimbType[i];

	return *this;
}


// ////////////////////////////////////////////////
// CommandLimbType contains an array of referenes
// to commands in the command array.  This level
// of indirection allows a limb type to pick any
// command in the command array.
//
//
void CommandLimbType::Mutate(int nChance)
{
	Randomizer rand;

	for (int i = 0; i < GetCount(); i++)
	{
		if (rand.Int1024() < nChance)
		{
			m_comref[i] = rand.Byte(CommandArray::MAX_COMMANDS);
		}
		if (rand.Int1024() < nChance)
			m_sumref[i] = rand.Byte(ProductArray::MAX_PRODUCT_SUMS);
	}
}

void CommandLimbType::Randomize()
{
	Randomizer rand;

	for (int i = 0; i < GetCount(); i++)
	{
		m_comref[i] = rand.Byte(CommandArray::MAX_COMMANDS);

		m_sumref[i]    = rand.Byte(ProductArray::MAX_PRODUCT_SUMS);
	}
}

void CommandLimbType::SerializeJson(rapidjson::Document &d, rapidjson::Value &v)
{
    Document::AllocatorType& allocator = d.GetAllocator();

    Value comJson(kArrayType);
    Value sumJson(kArrayType);
    for (int i = 0; i < GetCount(); i++)
    {
        comJson.PushBack(m_comref[i], allocator);
        sumJson.PushBack(m_sumref[i], allocator);
    }
    v.AddMember("m_comref", comJson, allocator);
    v.AddMember("m_sumref", sumJson, allocator);
}

void CommandLimbType::SerializeJsonLoad(const rapidjson::Value& v)
{
    const Value &cr = v["m_comref"];
    for(int i=0; i<cr.Size() and i < MAX_COMMANDS_PER_LIMB; i++)
        m_comref[i] = cr[i].GetUint();

    const Value &sr = v["m_sumref"];
    for(int i=0; i<sr.Size() and i < MAX_COMMANDS_PER_LIMB; i++)
        m_sumref[i] = sr[i].GetUint();

}

void CommandLimbType::Crossover(CommandLimbType& commandLimbType)
{
	Randomizer rand;

	for (int i = 0; i < GetCount(); i++)
	{
		if (rand.Bool())
			m_comref[i] = commandLimbType.m_comref[i];
		if (rand.Bool())
			m_sumref[i] = commandLimbType.m_sumref[i];
	}
}


CommandLimbType& CommandLimbType::operator=(CommandLimbType& commandLimbType)
{
	for (int i = 0; i < GetCount(); i++)
	{
		m_comref[i] = commandLimbType.m_comref[i];
        assert(m_comref[i] < 64);
		m_sumref[i]    = commandLimbType.m_sumref[i];
	}
	return *this;
}


// ///////////////////////////////////////////////////////////////////
// CommandLimbStore
//
// Each limb, regardless of type, has associated state information
// to store.
//

void CommandLimbStore::SerializeJson(rapidjson::Document &d, rapidjson::Value &v, Biot& biot)
{
    Document::AllocatorType& allocator = d.GetAllocator();

    v.AddMember("m_nLimbType", m_nLimbType, allocator);
    v.AddMember("m_nLimb", m_nLimb, allocator);

    Value commandArr(kArrayType);
    for(int m_index=0; m_index<CommandLimbType::MAX_COMMANDS_PER_LIMB; m_index++)
    {
        m_pArg = &biot.m_commandArray.GetCommandArgument(m_nLimbType, m_index);
        Value commandJson(kObjectType);

        switch (m_pArg->GetCommand())
        {
            case CommandArgument::COMMAND_FLAP_LIMB_SEGMENT:
                command[m_index].flapLimbSegment.SerializeJson(d, commandJson);
                break;

            case CommandArgument::COMMAND_FLAP_LIMB_TYPE_SEGMENT:
                command[m_index].flapLimbTypeSegment.SerializeJson(d, commandJson);
                break;

            case CommandArgument::COMMAND_MOVE_LIMB_SEGMENT:
                command[m_index].moveLimbSegment.SerializeJson(d, commandJson);
                break;

            case CommandArgument::COMMAND_MOVE_LIMB_SEGMENTS:
                command[m_index].moveLimbSegments.SerializeJson(d, commandJson);
                break;

            case CommandArgument::COMMAND_MOVE_LIMB_TYPE_SEGMENT:
                command[m_index].moveLimbTypeSegment.SerializeJson(d, commandJson);
                break;

            case CommandArgument::COMMAND_MOVE_LIMB_TYPE_SEGMENTS:
                command[m_index].moveLimbTypeSegments.SerializeJson(d, commandJson);
                break;

            case CommandArgument::COMMAND_RETRACT_LIMB_TYPE:
                command[m_index].retractLimbType.SerializeJson(d, commandJson);
                break;

            case CommandArgument::COMMAND_RETRACT_LIMB:
                command[m_index].retractLimb.SerializeJson(d, commandJson);
                break;

            case CommandArgument::COMMAND_NOP:
                command[m_index].nop.SerializeJson(d, commandJson);
                break;

            case CommandArgument::COMMAND_MEMORY:
                command[m_index].memory.SerializeJson(d, commandJson);
                break;

            default:
                assert(0);
                break;
        }
        commandArr.PushBack(commandJson, allocator);
    }
    v.AddMember("command", commandArr, allocator);
}

void CommandLimbStore::SerializeJsonLoad(const rapidjson::Value& v, Biot& biot)
{
    m_nLimbType = v["m_nLimbType"].GetInt();
    m_nLimb = v["m_nLimb"].GetInt();

    const Value &comm = v["command"];
    for(int m_index=0; m_index<comm.Size() and m_index<CommandLimbType::MAX_COMMANDS_PER_LIMB; m_index++)
    {
        m_pArg = &biot.m_commandArray.GetCommandArgument(m_nLimbType, m_index);
        Value commandJson(kObjectType);

        switch (m_pArg->GetCommand())
        {
            case CommandArgument::COMMAND_FLAP_LIMB_SEGMENT:
                command[m_index].flapLimbSegment.SerializeJsonLoad(comm[m_index]);
                break;

            case CommandArgument::COMMAND_FLAP_LIMB_TYPE_SEGMENT:
                command[m_index].flapLimbTypeSegment.SerializeJsonLoad(comm[m_index]);
                break;

            case CommandArgument::COMMAND_MOVE_LIMB_SEGMENT:
                command[m_index].moveLimbSegment.SerializeJsonLoad(comm[m_index]);
                break;

            case CommandArgument::COMMAND_MOVE_LIMB_SEGMENTS:
                command[m_index].moveLimbSegments.SerializeJsonLoad(comm[m_index]);
                break;

            case CommandArgument::COMMAND_MOVE_LIMB_TYPE_SEGMENT:
                command[m_index].moveLimbTypeSegment.SerializeJsonLoad(comm[m_index]);
                break;

            case CommandArgument::COMMAND_MOVE_LIMB_TYPE_SEGMENTS:
                command[m_index].moveLimbTypeSegments.SerializeJsonLoad(comm[m_index]);
                break;

            case CommandArgument::COMMAND_RETRACT_LIMB_TYPE:
                command[m_index].retractLimbType.SerializeJsonLoad(comm[m_index]);
                break;

            case CommandArgument::COMMAND_RETRACT_LIMB:
                command[m_index].retractLimb.SerializeJsonLoad(comm[m_index]);
                break;

            case CommandArgument::COMMAND_NOP:
                command[m_index].nop.SerializeJsonLoad(comm[m_index]);
                break;

            case CommandArgument::COMMAND_MEMORY:
                command[m_index].memory.SerializeJsonLoad(comm[m_index]);
                break;

            default:
                assert(0);
                break;
        }

    }
}

void CommandLimbStore::Initialize(int nLimbType, int nLimb, Biot& biot)
{
    assert(nLimbType < MAX_LIMB_TYPES && nLimbType >= 0);
	m_nLimbType = nLimbType;
	m_nLimb     = nLimb;
	m_pBiot     = &biot;

	for (m_index = 0; m_index < CommandLimbType::MAX_COMMANDS_PER_LIMB; m_index++)
	{
		m_pArg = &biot.m_commandArray.GetCommandArgument(nLimbType, m_index);

		switch (m_pArg->GetCommand())
		{
			case CommandArgument::COMMAND_FLAP_LIMB_SEGMENT:
				command[m_index].flapLimbSegment.Initialize(*this);
				break;

			case CommandArgument::COMMAND_FLAP_LIMB_TYPE_SEGMENT:
				command[m_index].flapLimbTypeSegment.Initialize(*this);
				break;

			case CommandArgument::COMMAND_MOVE_LIMB_SEGMENT:
				command[m_index].moveLimbSegment.Initialize(*this);
				break;

			case CommandArgument::COMMAND_MOVE_LIMB_SEGMENTS:
				command[m_index].moveLimbSegments.Initialize(*this);
				break;

			case CommandArgument::COMMAND_MOVE_LIMB_TYPE_SEGMENT:
				command[m_index].moveLimbTypeSegment.Initialize(*this);
				break;

			case CommandArgument::COMMAND_MOVE_LIMB_TYPE_SEGMENTS:
				command[m_index].moveLimbTypeSegments.Initialize(*this);
				break;

			case CommandArgument::COMMAND_RETRACT_LIMB_TYPE:
				command[m_index].retractLimbType.Initialize(*this);
				break;

			case CommandArgument::COMMAND_RETRACT_LIMB:
				command[m_index].retractLimb.Initialize(*this);
				break;

			case CommandArgument::COMMAND_NOP:
				command[m_index].nop.Initialize(*this);
				break;

			case CommandArgument::COMMAND_MEMORY:
				command[m_index].memory.Initialize(*this);
				break;

			default:
                assert(0);
				break;
		}
	}
}

void CommandLimbStore::Execute(Biot& biot, uint32_t dwSensor)
{
	m_pBiot     = &biot;
	m_dwSensor  = dwSensor;

	for (m_index = 0; m_index < CommandLimbType::MAX_COMMANDS_PER_LIMB; m_index++)
	{
		m_pArg = &biot.m_commandArray.GetCommandArgument(m_nLimbType, m_index);
		switch (m_pArg->GetCommand())
		{
		    case CommandArgument::COMMAND_FLAP_LIMB_SEGMENT:
				command[m_index].flapLimbSegment.Execute(*this);
				break;

			case CommandArgument::COMMAND_FLAP_LIMB_TYPE_SEGMENT:
				command[m_index].flapLimbTypeSegment.Execute(*this);
				break;

			case CommandArgument::COMMAND_MOVE_LIMB_SEGMENT:
				command[m_index].moveLimbSegment.Execute(*this);
				break;

			case CommandArgument::COMMAND_MOVE_LIMB_SEGMENTS:
				command[m_index].moveLimbSegments.Execute(*this);
				break;

			case CommandArgument::COMMAND_MOVE_LIMB_TYPE_SEGMENT:
				command[m_index].moveLimbTypeSegment.Execute(*this);
				break;

			case CommandArgument::COMMAND_MOVE_LIMB_TYPE_SEGMENTS:
				command[m_index].moveLimbTypeSegments.Execute(*this);
				break;

			case CommandArgument::COMMAND_RETRACT_LIMB_TYPE:
				command[m_index].retractLimbType.Execute(*this);
				break;

			case CommandArgument::COMMAND_RETRACT_LIMB:
				command[m_index].retractLimb.Execute(*this);
				break;

			case CommandArgument::COMMAND_NOP:
//				command[m_index].nop.Execute(*this);
				break;

			case CommandArgument::COMMAND_MEMORY:
				command[m_index].memory.Execute(*this);
				break;

			default:
                assert(0);
				break;
		}
	}
}


inline bool CommandLimbStore::IsSensorTrue()
{
	return m_pBiot->m_commandArray.IsTrue(m_nLimbType, m_index, m_dwSensor);
}


// /////////////////////////////////////////////////////////////
//CommandFlapLine
//
// Don't give me no flap!  Flaps an individual line.
//
void CommandFlapLimbTypeSegment::Initialize(CommandLimbStore& store)
{
	// Determine limb type to move
	m_nLimbType = store.m_pArg->GetLimbType();
	m_nSegment  = store.m_pArg->GetSegment();

	// Is this line type even visible?
	if (!store.m_pBiot->trait.IsLineTypeVisible(m_nLimbType) ||
		!store.m_pBiot->trait.GetLineType(m_nLimbType).GetSegment(m_nSegment).IsVisible())
	{
		// Record we should ignore this command
		m_nLimbType = MAX_LIMB_TYPES;
		return;
	}

	// How far are we going to move it?
    m_nMaxDegrees     = store.m_pArg->GetDegrees() % 80 + 10;

	// How far have we moved it thus far?
	m_nAppliedDegrees = 0; 

	// What rate?
	m_nRate = store.m_pArg->GetRate();

	m_bGoingUp = true;
}

void CommandFlapLimbTypeSegment::Execute(CommandLimbStore& store)
{
	if (m_nLimbType == MAX_LIMB_TYPES)
		return;

	if (store.IsSensorTrue())
	{
		if (m_bGoingUp)
		{
			bool bIsNegative = (m_nAppliedDegrees < 0);

			// If we are true, we need to adjust back full extension
			if (m_nAppliedDegrees < m_nMaxDegrees)
			{
                m_nAppliedDegrees += store.m_pBiot->MoveLimbTypeSegment(m_nSegment, m_nLimbType, std::min(m_nRate, m_nMaxDegrees-m_nAppliedDegrees));
				m_bGoingUp = (m_nAppliedDegrees == m_nMaxDegrees);
			}

			if (bIsNegative && m_nAppliedDegrees >= 0)
			{
				// Apply impulse
				FlapLimbTypeSegments(*store.m_pBiot);
			}
		}
		else
		{
			bool bIsPositive = (m_nAppliedDegrees > 0);

			// If we are true, we need to adjust back full extension
			if (m_nAppliedDegrees > -m_nMaxDegrees)
			{
                m_nAppliedDegrees += store.m_pBiot->MoveLimbTypeSegment(m_nSegment, m_nLimbType, -std::min(m_nRate, m_nAppliedDegrees + m_nMaxDegrees));

				m_bGoingUp = (m_nAppliedDegrees == -m_nMaxDegrees);
			}

			if (bIsPositive && m_nAppliedDegrees <= 0)
			{
				// Apply impulse
				FlapLimbTypeSegments(*store.m_pBiot);
			}
		}
	}
	else
	{
		// Return the segment to the mormal position.
		if (m_nAppliedDegrees > 0)
		{
            m_nAppliedDegrees += store.m_pBiot->MoveLimbTypeSegment(m_nSegment, m_nLimbType, -std::min(m_nRate, m_nAppliedDegrees));
		}
		
		if (m_nAppliedDegrees < 0)
		{
            m_nAppliedDegrees += store.m_pBiot->MoveLimbTypeSegment(m_nSegment, m_nLimbType, std::min(m_nRate, m_nAppliedDegrees));
		}
	}
}


void CommandFlapLimbTypeSegment::FlapLimbTypeSegments(Biot& biot)
{
	for (int i = 0; i < biot.trait.GetLines(); i++)
	{
		if (m_nLimbType == biot.trait.GetLineTypeIndex(i))
		{
            int nPeno = m_nSegment * MAX_LIMBS + i;
			Flap(biot, nPeno);
		}
	}
}


void CommandFlapLimbTypeSegment::Flap(Biot& biot, int nPeno)
{
	// If we are damaged, we don't get a boost from flapping
	if (!biot.IsSegmentMissing(nPeno))
	{
        double Vx = biot.startPt[nPeno].x() - biot.stopPt[nPeno].x();
        double Vy = biot.startPt[nPeno].y() - biot.stopPt[nPeno].y();
		double dr = biot.vector.rotationComponent(
            (double) biot.startPt[nPeno].x(),
            (double) biot.startPt[nPeno].y(),
            biot.startPt[nPeno].x() + Vx,
            biot.startPt[nPeno].y() + Vy);

        double radius = biot.vector.distance(biot.startPt[nPeno].x(), biot.startPt[nPeno].y());

		if (dr != 0)
		{
			double dv = biot.vector.motionComponent(biot.vector.distance(Vx, Vy), dr);
            Vx = -biot.vector.fraction(dv, (int) biot.startPt[nPeno].x(), radius);
            Vy = -biot.vector.fraction(dv, (int) biot.startPt[nPeno].y(), radius);
		}
		else
		{
//			Vx = -Vx;
//			Vy = -Vy;
		}
		biot.vector.accelerateX(Vx*20);
		biot.vector.accelerateY(Vy*20);
		biot.vector.accelerateRotation(dr*10);
	}
}

void CommandFlapLimbTypeSegment::SerializeJson(rapidjson::Document &d, rapidjson::Value &v)
{
    Document::AllocatorType& allocator = d.GetAllocator();

    v.AddMember("type", "CommandFlapLimbTypeSegment", allocator);

    v.AddMember("m_nLimbType", m_nLimbType, allocator);
    v.AddMember("m_nSegment", m_nSegment, allocator);
    v.AddMember("m_nRate", m_nRate, allocator);
    v.AddMember("m_nMaxDegrees", m_nMaxDegrees, allocator);
    v.AddMember("m_nAppliedDegrees", m_nAppliedDegrees, allocator);
    v.AddMember("m_bGoingUp", m_bGoingUp, allocator);

}

void CommandFlapLimbTypeSegment::SerializeJsonLoad(const rapidjson::Value& v)
{
    m_nLimbType = v["m_nLimbType"].GetInt();
    m_nSegment = v["m_nSegment"].GetInt();
    m_nRate = v["m_nRate"].GetInt();
    m_nMaxDegrees = v["m_nMaxDegrees"].GetInt();
    m_nAppliedDegrees = v["m_nAppliedDegrees"].GetInt();
    m_bGoingUp = v["m_bGoingUp"].GetBool();
}

// /////////////////////////////////////////////////////////////
//Don't give me no flap!
//
void CommandFlapLimbSegment::Initialize(CommandLimbStore& store)
{
	// Determine limb type to move
	m_nLimb     = store.m_pArg->GetLimb(store.m_nLimb);
	m_nSegment  = store.m_pArg->GetSegment();

	// Is this line type even visible?
	if (m_nLimb >= store.m_pBiot->trait.GetLines() ||
		!store.m_pBiot->trait.GetLineType(store.m_pBiot->trait.GetLineTypeIndex(m_nLimb)).GetSegment(m_nSegment).IsVisible())
	{
		// Record we should ignore this command
        m_nLimb = MAX_LIMBS;
		return;
	}

	// How far are we going to move it?
    m_nMaxDegrees     = store.m_pArg->GetDegrees() % 80 + 10;

	// How far have we moved it thus far?
	m_nAppliedDegrees = 0; 

	// What rate?
	m_nRate = store.m_pArg->GetRate();

	m_bGoingUp = true;
}

void CommandFlapLimbSegment::Execute(CommandLimbStore& store)
{
    if (m_nLimb == MAX_LIMBS)
		return;

	if (store.IsSensorTrue())
	{
		if (m_bGoingUp)
		{
			bool bIsNegative = (m_nAppliedDegrees < 0);

			// If we are true, we need to adjust back full extension
			if (m_nAppliedDegrees < m_nMaxDegrees)
			{
                m_nAppliedDegrees += store.m_pBiot->MoveLimbSegment(m_nSegment, m_nLimb, std::min(m_nRate, m_nMaxDegrees-m_nAppliedDegrees));
				m_bGoingUp = (m_nAppliedDegrees < m_nMaxDegrees);
			}

			if (bIsNegative && m_nAppliedDegrees >= 0)
			{
				// Apply impulse
				Flap(*store.m_pBiot);
			}
		}
		else
		{
			bool bIsPositive = (m_nAppliedDegrees > 0);

			// If we are true, we need to adjust back full extension
			if (m_nAppliedDegrees > -m_nMaxDegrees)
			{
                m_nAppliedDegrees += store.m_pBiot->MoveLimbSegment(m_nSegment, m_nLimb, -std::min(m_nRate, m_nAppliedDegrees + m_nMaxDegrees));

				m_bGoingUp = (m_nAppliedDegrees <= -m_nMaxDegrees);
			}

			if (bIsPositive && m_nAppliedDegrees <= 0)
			{
				// Apply impulse
				Flap(*store.m_pBiot);
			}
		}
	}
	else
	{
		// Return the segment to the mormal position.
		if (m_nAppliedDegrees > 0)
		{
            m_nAppliedDegrees += store.m_pBiot->MoveLimbSegment(m_nSegment, m_nLimb, -std::min(m_nRate, m_nAppliedDegrees));
		}
		
		if (m_nAppliedDegrees < 0)
		{
            m_nAppliedDegrees += store.m_pBiot->MoveLimbSegment(m_nSegment, m_nLimb, std::min(m_nRate, m_nAppliedDegrees));
		}
	}
}


void CommandFlapLimbSegment::Flap(Biot& biot)
{
    int nPeno = m_nSegment * MAX_LIMBS + m_nLimb;

	// If we are damaged, we don't get a boost from flapping
	if (!biot.IsSegmentMissing(nPeno))
	{

		//TODO: This vector is calculated with the assumption that the origin is the
		// center of mass.  We want to change that to the center of the bounding
		// rectangle.
        double Vx = biot.startPt[nPeno].x() - biot.stopPt[nPeno].x();
        double Vy = biot.startPt[nPeno].y() - biot.stopPt[nPeno].y();
		double dr = biot.vector.rotationComponent(
            (double) biot.startPt[nPeno].x(),
            (double) biot.startPt[nPeno].y(),
            biot.startPt[nPeno].x() + Vx,
            biot.startPt[nPeno].y() + Vy);

        double radius = biot.vector.distance(biot.startPt[nPeno].x() , biot.startPt[nPeno].y());

		if (dr != 0)
		{
			double dv = biot.vector.motionComponent(biot.vector.distance(Vx, Vy), dr);
            Vx = -biot.vector.fraction(dv, (int) biot.startPt[nPeno].x(), radius);
            Vy = -biot.vector.fraction(dv, (int) biot.startPt[nPeno].y(), radius);
		}
		else
		{
//			Vx = -Vx;
//			Vy = -Vy;
		}
		biot.vector.accelerateX(Vx*20);
		biot.vector.accelerateY(Vy*20);
		biot.vector.accelerateRotation(dr*10);
	}
}

void CommandFlapLimbSegment::SerializeJson(rapidjson::Document &d, rapidjson::Value &v)
{
    Document::AllocatorType& allocator = d.GetAllocator();

    v.AddMember("type", "CommandFlapLimbSegment", allocator);

    v.AddMember("m_nLimb", m_nLimb, allocator);
    v.AddMember("m_nSegment", m_nSegment, allocator);
    v.AddMember("m_nRate", m_nRate, allocator);
    v.AddMember("m_nMaxDegrees", m_nMaxDegrees, allocator);
    v.AddMember("m_nAppliedDegrees", m_nAppliedDegrees, allocator);
    v.AddMember("m_bGoingUp", m_bGoingUp, allocator);

}

void CommandFlapLimbSegment::SerializeJsonLoad(const rapidjson::Value& v)
{
    m_nLimb = v["m_nLimb"].GetInt();
    m_nSegment = v["m_nSegment"].GetInt();
    m_nRate = v["m_nRate"].GetInt();
    m_nMaxDegrees = v["m_nMaxDegrees"].GetInt();
    m_nAppliedDegrees = v["m_nAppliedDegrees"].GetInt();
    m_bGoingUp = v["m_bGoingUp"].GetBool();
}

// /////////////////////////////////////////////////////////////
void CommandMoveLimbSegment::Initialize(CommandLimbStore& store)
{
		// Determine limb type to move
	m_nLimb     = store.m_pArg->GetLimb(store.m_nLimb);
	m_nSegment  = store.m_pArg->GetSegment();

	// Is this line type even visible?
	if (m_nLimb >= store.m_pBiot->trait.GetLines() ||
		!store.m_pBiot->trait.GetLineType(store.m_pBiot->trait.GetLineTypeIndex(m_nLimb)).GetSegment(m_nSegment).IsVisible())
	{
		// Record we should ignore this command
        m_nLimb = MAX_LIMBS;
		return;
	}

	// How far are we going to move it?
	m_nMaxDegrees     = store.m_pArg->GetDegrees();

	// How far have we moved it thus far?
	m_nAppliedDegrees = 0; 

	// What rate?
	m_nRate = store.m_pArg->GetRate();
}

void CommandMoveLimbSegment::Execute(CommandLimbStore& store)
{
    if (m_nLimb == MAX_LIMBS)
		return;

	if (store.IsSensorTrue())
	{
		// If we are true, we need to adjust back full extension
		if (m_nAppliedDegrees < m_nMaxDegrees)
		{
            m_nAppliedDegrees += store.m_pBiot->MoveLimbSegment(m_nSegment, m_nLimb, std::min(m_nRate, m_nMaxDegrees-m_nAppliedDegrees));
		}
	}
	else
	{
		// If we are false, we need to adjust back full extension
		if (m_nAppliedDegrees > 0)
		{
            m_nAppliedDegrees += store.m_pBiot->MoveLimbSegment(m_nSegment, m_nLimb, -std::min(m_nRate, m_nAppliedDegrees));
		}
	}
}

void CommandMoveLimbSegment::SerializeJson(rapidjson::Document &d, rapidjson::Value &v)
{
    Document::AllocatorType& allocator = d.GetAllocator();

    v.AddMember("type", "CommandMoveLimbSegment", allocator);

    v.AddMember("m_nLimb", m_nLimb, allocator);
    v.AddMember("m_nSegment", m_nSegment, allocator);
    v.AddMember("m_nRate", m_nRate, allocator);
    v.AddMember("m_nMaxDegrees", m_nMaxDegrees, allocator);
    v.AddMember("m_nAppliedDegrees", m_nAppliedDegrees, allocator);
}

void CommandMoveLimbSegment::SerializeJsonLoad(const rapidjson::Value& v)
{
    m_nLimb = v["m_nLimb"].GetInt();
    m_nSegment = v["m_nSegment"].GetInt();
    m_nRate = v["m_nRate"].GetInt();
    m_nMaxDegrees = v["m_nMaxDegrees"].GetInt();
    m_nAppliedDegrees = v["m_nAppliedDegrees"].GetInt();
}

// /////////////////////////////////////////////////////////////
void CommandMoveLimbTypeSegment::Initialize(CommandLimbStore& store)
{
	// Determine limb type to move
	m_nLimbType = store.m_pArg->GetLimbType();
	m_nSegment  = store.m_pArg->GetSegment();

	// Is this line type even visible?
	if (!store.m_pBiot->trait.IsLineTypeVisible(m_nLimbType) ||
		!store.m_pBiot->trait.GetLineType(m_nLimbType).GetSegment(m_nSegment).IsVisible())
	{
		// Record we should ignore this command
		m_nLimbType = MAX_LIMB_TYPES;
		return;
	}

	// How far are we going to move it?
	m_nMaxDegrees     = store.m_pArg->GetDegrees();

	// How far have we moved it thus far?
	m_nAppliedDegrees = 0; 

	// What rate?
	m_nRate = store.m_pArg->GetRate();
}


void CommandMoveLimbTypeSegment::Execute(CommandLimbStore& store)
{
	if (m_nLimbType == MAX_LIMB_TYPES)
		return;

	if (store.IsSensorTrue())
	{
		// If we are true, we need to adjust back full extension
		if (m_nAppliedDegrees < m_nMaxDegrees)
		{
            m_nAppliedDegrees += store.m_pBiot->MoveLimbTypeSegment(m_nSegment, m_nLimbType, std::min(m_nRate, m_nMaxDegrees-m_nAppliedDegrees));
		}
	}
	else
	{
		// If we are false, we need to adjust back full extension
		if (m_nAppliedDegrees > 0)
		{
            m_nAppliedDegrees += store.m_pBiot->MoveLimbTypeSegment(m_nSegment, m_nLimbType, -std::min(m_nRate, m_nAppliedDegrees));
		}
	}
}

void CommandMoveLimbTypeSegment::SerializeJson(rapidjson::Document &d, rapidjson::Value &v)
{
    Document::AllocatorType& allocator = d.GetAllocator();

    v.AddMember("type", "CommandMoveLimbTypeSegment", allocator);

    v.AddMember("m_nLimbType", m_nLimbType, allocator);
    v.AddMember("m_nSegment", m_nSegment, allocator);
    v.AddMember("m_nRate", m_nRate, allocator);
    v.AddMember("m_nMaxDegrees", m_nMaxDegrees, allocator);
    v.AddMember("m_nAppliedDegrees", m_nAppliedDegrees, allocator);
}

void CommandMoveLimbTypeSegment::SerializeJsonLoad(const rapidjson::Value& v)
{
    m_nLimbType = v["m_nLimbType"].GetInt();
    m_nSegment = v["m_nSegment"].GetInt();
    m_nRate = v["m_nRate"].GetInt();
    m_nMaxDegrees = v["m_nMaxDegrees"].GetInt();
    m_nAppliedDegrees = v["m_nAppliedDegrees"].GetInt();
}

// /////////////////////////////////////////////////////////////
void CommandMoveLimbSegments::Initialize(CommandLimbStore& store)
{
	// Determine limb type to move
	m_nLimb = store.m_pArg->GetLimb(store.m_nLimb);

	// Is this line type even visible?
	if (m_nLimb >= store.m_pBiot->trait.GetLines())
	{
		// Record we should ignore this command
        m_nLimb = MAX_LIMBS;
		return;
	}

	// How far are we going to move it?
	m_nMaxDegrees  = store.m_pArg->GetDegrees();

	// How far have we moved it thus far?
	m_nAppliedDegrees = 0; 

	// What rate?
	m_nRate = store.m_pArg->GetRate();

}

void CommandMoveLimbSegments::Execute(CommandLimbStore& store)
{
    if (m_nLimb == MAX_LIMBS)
		return;
    assert (m_nLimb < MAX_LIMBS && m_nLimb >= 0);

	if (store.IsSensorTrue())
	{
		// If we are true, we need to adjust back full extension
		if (m_nAppliedDegrees < m_nMaxDegrees)
		{
            m_nAppliedDegrees += store.m_pBiot->MoveLimbSegments(m_nLimb, std::min(m_nRate, m_nMaxDegrees-m_nAppliedDegrees));
		}
	}
	else
	{
		// If we are false, we need to adjust back full extension
		if (m_nAppliedDegrees > 0)
		{
            m_nAppliedDegrees += store.m_pBiot->MoveLimbSegments(m_nLimb, -std::min(m_nRate, m_nAppliedDegrees));
		}
	}
}

void CommandMoveLimbSegments::SerializeJson(rapidjson::Document &d, rapidjson::Value &v)
{
    Document::AllocatorType& allocator = d.GetAllocator();

    v.AddMember("type", "CommandMoveLimbSegments", allocator);

    v.AddMember("m_nLimb", m_nLimb, allocator);
    v.AddMember("m_nRate", m_nRate, allocator);
    v.AddMember("m_nMaxDegrees", m_nMaxDegrees, allocator);
    v.AddMember("m_nAppliedDegrees", m_nAppliedDegrees, allocator);

}

void CommandMoveLimbSegments::SerializeJsonLoad(const rapidjson::Value& v)
{
    m_nLimb = v["m_nLimb"].GetInt();
    m_nRate = v["m_nRate"].GetInt();
    m_nMaxDegrees = v["m_nMaxDegrees"].GetInt();
    m_nAppliedDegrees = v["m_nAppliedDegrees"].GetInt();
}

// /////////////////////////////////////////////////////////////
void CommandMoveLimbTypeSegments::Initialize(CommandLimbStore& store)
{
	// Determine limb type to move
	m_nLimbType = store.m_pArg->GetLimbType();

	// Is this line type even visible?
	if (!store.m_pBiot->trait.IsLineTypeVisible(m_nLimbType))
	{
		// Record we should ignore this command
		m_nLimbType = MAX_LIMB_TYPES;
		return;
	}

	// How far are we going to move it?
	m_nMaxDegrees  = store.m_pArg->GetDegrees();

	// How far have we moved it thus far?
	m_nAppliedDegrees = 0; 

	// What rate?
	m_nRate = store.m_pArg->GetRate();

}

void CommandMoveLimbTypeSegments::Execute(CommandLimbStore& store)
{
	if (m_nLimbType == MAX_LIMB_TYPES)
		return;
    assert (m_nLimbType < MAX_LIMB_TYPES && MAX_LIMB_TYPES >= 0);

	if (store.IsSensorTrue())
	{
		// If we are true, we need to adjust back full extension
		if (m_nAppliedDegrees < m_nMaxDegrees)
		{
            m_nAppliedDegrees += store.m_pBiot->MoveLimbTypeSegments(m_nLimbType, std::min(m_nRate, m_nMaxDegrees-m_nAppliedDegrees));
		}
	}
	else
	{
		// If we are false, we need to adjust back full extension
		if (m_nAppliedDegrees > 0)
		{
            m_nAppliedDegrees += store.m_pBiot->MoveLimbTypeSegments(m_nLimbType, -std::min(m_nRate, m_nAppliedDegrees));
		}
	}
}

void CommandMoveLimbTypeSegments::SerializeJson(rapidjson::Document &d, rapidjson::Value &v)
{
    Document::AllocatorType& allocator = d.GetAllocator();

    v.AddMember("type", "CommandMoveLimbTypeSegments", allocator);

    v.AddMember("m_nLimbType", m_nLimbType, allocator);
    v.AddMember("m_nRate", m_nRate, allocator);
    v.AddMember("m_nMaxDegrees", m_nMaxDegrees, allocator);
    v.AddMember("m_nAppliedDegrees", m_nAppliedDegrees, allocator);
}

void CommandMoveLimbTypeSegments::SerializeJsonLoad(const rapidjson::Value& v)
{
    m_nLimbType = v["m_nLimbType"].GetInt();
    m_nRate = v["m_nRate"].GetInt();
    m_nMaxDegrees = v["m_nMaxDegrees"].GetInt();
    m_nAppliedDegrees = v["m_nAppliedDegrees"].GetInt();
}

// /////////////////////////////////////////////////////////////
void CommandRetractLimb::Initialize(CommandLimbStore& store)
{
	// Lets indicate there is nothing yet to do
	m_nSegment        = MAX_SEGMENTS;

	// We may be controlling a different limb
	m_nLimb     = store.m_pArg->GetLimb(store.m_nLimb);

	// That limb may not be visible
	if (m_nLimb >= store.m_pBiot->trait.GetLines())
		return;

	// What limb type is this limb?
	int nRealLimbType = store.m_pBiot->trait.GetLineTypeIndex(m_nLimb);

	// Walk all the segments and determine the max segment 
	int i = 0;
	for (i = MAX_SEGMENTS - 1; i >= 0; i--)
	{
		GeneSegment& segment = store.m_pBiot->trait.GetSegmentType(nRealLimbType, i);

		if (m_nSegment == MAX_SEGMENTS)
		{
			if (segment.IsVisible())
			{
				m_nMaxRadius     = segment.GetRawRadius();
				m_nAppliedRadius = m_nMaxRadius;
				m_nSegment       = i;
			}
		}
		else
		{
			// We found another visible segment (can't retract when only one line exists!)
			if (segment.IsVisible())
				break;
		}
	}

	// We must not have found another visible segment!
	if (i < 0)
	{
		m_nSegment = MAX_SEGMENTS;
	}
}

void CommandRetractLimb::Execute(CommandLimbStore& store)
{
	if (m_nSegment == MAX_SEGMENTS)
		return;

	if (store.IsSensorTrue())
	{
		// If we are true, we need to adjust back full extension
		if (m_nAppliedRadius > 0)
		{
			m_nAppliedRadius -= store.m_pBiot->RetractLine(m_nSegment, m_nLimb, m_nMaxRadius);
		}
	}
	else
	{
		// If we are false, we need to adjust back full extension
		if (m_nAppliedRadius < m_nMaxRadius)
		{
			m_nAppliedRadius += store.m_pBiot->ExtendLine(m_nSegment, m_nLimb);
		}
	}
}

void CommandRetractLimb::SerializeJson(rapidjson::Document &d, rapidjson::Value &v)
{
    Document::AllocatorType& allocator = d.GetAllocator();

    v.AddMember("type", "CommandRetractLimb", allocator);

    v.AddMember("m_nSegment", m_nSegment, allocator);
    v.AddMember("m_nLimb", m_nLimb, allocator);
    v.AddMember("m_nMaxRadius", m_nMaxRadius, allocator);
    v.AddMember("m_nAppliedRadius", m_nAppliedRadius, allocator);
}

void CommandRetractLimb::SerializeJsonLoad(const rapidjson::Value& v)
{
    m_nSegment = v["m_nSegment"].GetInt();
    m_nLimb = v["m_nLimb"].GetInt();
    m_nMaxRadius = v["m_nMaxRadius"].GetInt();
    m_nAppliedRadius = v["m_nAppliedRadius"].GetInt();
}


// /////////////////////////////////////////////////////////////
//
// Tell all the limbs of a particular type to retract
void CommandRetractLimbType::Initialize(CommandLimbStore& store)
{
	// Lets indicate there is nothing yet to do
	m_nSegment        = MAX_SEGMENTS;

	// What limb type is this limb?
	m_nLimbType = store.m_pArg->GetLimbType();

	// Maybe this limb type isn't visible
	if (!store.m_pBiot->trait.IsLineTypeVisible(m_nLimbType))
		return;

	// Walk all the segments and determine the max segment 
	int i = 0;
	for (i = MAX_SEGMENTS - 1; i >= 0; i--)
	{
		GeneSegment& segment = store.m_pBiot->trait.GetSegmentType(m_nLimbType, i);

		if (m_nSegment == MAX_SEGMENTS)
		{
			if (segment.IsVisible())
			{
				m_nMaxRadius     = segment.GetRawRadius();
				m_nAppliedRadius = m_nMaxRadius;
				m_nSegment       = i;
			}
		}
		else
		{
			// We found another visible segment (can't retract when only one line exists!)
			if (segment.IsVisible())
				break;
		}
	}
	// We must not have found another visible segment!
	if (i < 0)
	{
		m_nSegment = MAX_SEGMENTS;
	}
}


void CommandRetractLimbType::Execute(CommandLimbStore& store)
{
	if (m_nSegment == MAX_SEGMENTS)
		return;

	if (store.IsSensorTrue())
	{
		// If we are true, we need to adjust back full extension
		if (m_nAppliedRadius > 0)
		{
			m_nAppliedRadius -= store.m_pBiot->RetractLimbType(m_nSegment, m_nLimbType, m_nMaxRadius);
		}
	}
	else
	{
		// If we are false, we need to adjust back full extension
		if (m_nAppliedRadius < m_nMaxRadius)
		{
			m_nAppliedRadius += store.m_pBiot->ExtendLimbType(m_nSegment, m_nLimbType);
		}
	}
}

void CommandRetractLimbType::SerializeJson(rapidjson::Document &d, rapidjson::Value &v)
{
    Document::AllocatorType& allocator = d.GetAllocator();

    v.AddMember("type", "CommandRetractLimbType", allocator);

    v.AddMember("m_nSegment", m_nSegment, allocator);
    v.AddMember("m_nLimbType", m_nLimbType, allocator);
    v.AddMember("m_nMaxRadius", m_nMaxRadius, allocator);
    v.AddMember("m_nAppliedRadius", m_nAppliedRadius, allocator);
}

void CommandRetractLimbType::SerializeJsonLoad(const rapidjson::Value& v)
{
    m_nSegment = v["m_nSegment"].GetInt();
    m_nLimbType = v["m_nLimbType"].GetInt();
    m_nMaxRadius = v["m_nMaxRadius"].GetInt();
    m_nAppliedRadius = v["m_nAppliedRadius"].GetInt();
}

// /////////////////////////////////////////////////////////////
void CommandNOP::Initialize(CommandLimbStore& /*store*/)
{
 // Nothing to do!
}

void CommandNOP::SerializeJson(rapidjson::Document &d, rapidjson::Value &v)
{
    Document::AllocatorType& allocator = d.GetAllocator();

    v.AddMember("type", "CommandNOP", allocator);
}

void CommandNOP::SerializeJsonLoad(const rapidjson::Value& v)
{


}

void CommandNOP::Execute(CommandLimbStore& /*store*/)
{
 	BTRACE0("CommandNOP::Execute\n");
  // Nothing to do!
}


// /////////////////////////////////////////////////////////////
void CommandMemory::Initialize(CommandLimbStore& store)
{
	CommandArgument& arg = *store.m_pArg;

	m_bSet = false;
	if (arg.SetAlgorithmOne())
		m_type = WAIT_FOR_TRUE_SET;
	else
		m_type = WAIT_AND_SET;

	m_time   = arg.SetDuration();

}

void CommandMemory::Execute(CommandLimbStore& store)
{
	CommandArgument& arg = *store.m_pArg;

	if (!m_bSet)
	{
		// Before set:
		// Case one:  Certain things are true for a period of time, we set the bit
		// Case two:  A period of time passes, we set the bit
		switch (m_type)
		{
		case WAIT_FOR_TRUE_SET:
			if (!store.IsSensorTrue())
				m_time = arg.SetDuration();

		case WAIT_AND_SET:
			break;
		}

		m_time--;
		if (m_time <= 0)
		{
			// We can either clear or set the state bit
			if (store.m_pArg->WhatIsConsideredSet())
			{
				store.m_pBiot->m_internalState |= arg.WhichStateBit();
			}
			else
			{
				store.m_pBiot->m_internalState &= (~arg.WhichStateBit());
			}

			// Now that we triggered, set the time we should maintain this state
			m_time = arg.ClearDuration();
			m_bSet = true;

			// Set our clearing algorithm
			if (arg.ClearAlgorithmOne())
				m_type = WAIT_FOR_FALSE_CLEAR;
			else
				m_type = WAIT_AND_CLEAR;
			BTRACE1("Memory %d set\n", arg.WhichStateBit());
		}
	}
	else
	{
		// After set:
		// Case one:    Once set, certain things are false for a period of time, we clear the bit
		// Case two:    Once set, we clear the bit after a period of time
		// Case three:  Once set, we never clear the bit
		switch (m_type)
		{
		case WAIT_AND_CLEAR:
			break;

		case WAIT_FOR_FALSE_CLEAR:
			if (!store.IsSensorTrue())
				m_time = arg.ClearDuration();
			break;
		}

		m_time--;
		if (m_time <= 0)
		{
			if (arg.WhatIsConsideredSet())
			{
				store.m_pBiot->m_internalState &= (~arg.WhichStateBit());
			}
			else
			{
				store.m_pBiot->m_internalState |= arg.WhichStateBit();
			}
			m_time = arg.SetDuration();
			m_bSet = false;

			if (arg.SetAlgorithmOne())
				m_type = WAIT_FOR_FALSE_CLEAR;
			else
				m_type = WAIT_AND_CLEAR;
		}
	}
}

void CommandMemory::SerializeJson(rapidjson::Document &d, rapidjson::Value &v)
{
    Document::AllocatorType& allocator = d.GetAllocator();

    v.AddMember("type", "CommandMemory", allocator);

    v.AddMember("m_time", m_time, allocator);
    v.AddMember("m_type", m_type, allocator);
    v.AddMember("m_bSet", m_bSet, allocator);
}

void CommandMemory::SerializeJsonLoad(const rapidjson::Value& v)
{
    m_time = v["m_time"].GetInt();
    m_type = v["m_type"].GetInt();
    m_bSet = v["m_bSet"].GetBool();
}






