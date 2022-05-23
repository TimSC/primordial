#include "Environ.h"
#include "Biots.h"
#include <stdexcept>

using namespace rapidjson;

////////////////////////////////////////////////////////
// GeneSegment
//
// Defines a segment of a line
//

GeneSegment::GeneSegment()
{
    m_color[0] = 0;
    m_color[1] = 0;
    m_visible = 0;
    m_radius = 0;
    m_angle = -1;
    m_startSegment = 0;
}

void GeneSegment::SerializeJson(rapidjson::Document &d, rapidjson::Value &v)
{
    Document::AllocatorType& allocator = d.GetAllocator();

    v.AddMember("m_angle", m_angle, allocator);
    v.AddMember("m_radius", m_radius, allocator);
    v.AddMember("m_visible", m_visible, allocator);
    v.AddMember("m_startSegment", m_startSegment, allocator);
    v.AddMember("m_color0", m_color[0], allocator);
    v.AddMember("m_color1", m_color[1], allocator);

}

void GeneSegment::SerializeJsonLoad(const rapidjson::Value& v)
{
    if(!v.IsObject())
        throw std::runtime_error("eror parsing json");

    m_angle = v["m_angle"].GetInt();
    m_radius = v["m_radius"].GetUint();
    m_visible = v["m_visible"].GetUint();
    m_startSegment = v["m_startSegment"].GetUint();
    m_color[0] = v["m_color0"].GetUint();
    m_color[1] = v["m_color1"].GetUint();

    // A little crash protection
    if (m_radius > MAX_SEGMENT_LENGTH)
        m_radius = MAX_SEGMENT_LENGTH;

    if (m_color[0] >= DIM_COLOR - 1)
        m_color[0] = 0;

    if (m_color[1] >= DIM_COLOR)
        m_color[1] = 0;
}

void GeneSegment::Randomize(int segment, bool bIsVisible)
{
    // Perhaps we just do 16 next time, or 32
    m_radius = (uint8_t)  (Integer(MAX_SEGMENT_LENGTH - 1) + 2);

    if (segment == 0)
        m_angle  = (short) (Integer(45) + 1);
    else
        m_angle  = (short) (Integer(350) - 175);
  
    m_visible = (uint8_t) bIsVisible;

    m_startSegment = Byte(MAX_SEGMENTS);

    m_color[0]   = Byte(DIM_COLOR - 1);
//    if (m_color[0] != GREEN_LEAF)
//        m_color[0]  = Byte(DIM_COLOR);

    m_color[1]   = Byte(DIM_COLOR);
    if (m_color[1] != WHITE_LEAF && m_color[1] != GREEN_LEAF)
        m_color[1]  = Byte(DIM_COLOR);
}


// Settings to help debug a biot
void GeneSegment::Debug(int segment, bool bIsVisible)
{
    m_radius = (uint8_t) MAX_SEGMENT_LENGTH;
    m_angle = 0;//5;
    // 0 and 45 put the legs right on top of each other for 8 legs

    m_visible = (uint8_t) bIsVisible;
    m_startSegment = segment - 1;
    if (segment != 2)
    {
        m_color[0]   = GREEN_LEAF;
        m_color[1]   = WHITE_LEAF;
    }
    else
    {
        m_color[0]   = BLUE_LEAF;
        m_color[1]   = RED_LEAF;
    }
}


void GeneSegment::Mutate(int chance, int segment)
{
    if (Int1024() < chance)
        m_radius = (uint8_t) (Integer(MAX_SEGMENT_LENGTH - 1) + 2);

    if (Int1024() < chance)
    {
        if (segment == 0)
            m_angle  = (short)(Integer(45) + 1);
        else
            m_angle = (short)(Integer(350) - 175);
    }

    if (Int1024() < chance)
        m_startSegment = Byte(MAX_SEGMENTS);

    if (Int1024() < chance)
        m_visible = !m_visible;
  
    if (Int1024() < chance)
        m_color[0]   = Byte(DIM_COLOR - 1);

    if (Int1024() < chance)
        m_color[1]   = Byte(DIM_COLOR);

}


////////////////////////////////////////////////////////
// GeneLimb
//
// Contains GeneSegments
//

GeneLimb::GeneLimb()
{


}

void GeneLimb::Randomize(int nSegmentsPerArm)
{
    for (int i = 0; i < MAX_SEGMENTS; i++)
    {
        switch (nSegmentsPerArm)
        {
        default:
        case 0:
            m_segment[i].Randomize(i, Bool());
            break;

        case 1:
            m_segment[i].Randomize(i, (i < 3)?Bool():false);
            break;

        case 2:
            if (i < 4)
                m_segment[i].Randomize(i, true);
            else
                m_segment[i].Randomize(i, (i < 7)?Bool():false);
            break;

        case 3:
            if (i < 7)
                m_segment[i].Randomize(i, true);
            else
                m_segment[i].Randomize(i, (i < 10)?Bool():false);
            break;
        }
    }
    ToggleSegments();
}


void GeneLimb::Debug(int nSegmentsPerArm)
{
    for (int i = 0; i < MAX_SEGMENTS; i++)
    {
        m_segment[i].Debug(i, (i < MAX_SEGMENTS/2)?true:false);
    }
/*        switch (nSegmentsPerArm)
        {
        default:
        case 0:
            m_segment[i].Debug(i, true);
            break;

        case 1:
            m_segment[i].Debug(i, (i < 3)?true:false);
            break;

        case 2:
            if (i < 4)
                m_segment[i].Debug(i, true);
            else
                m_segment[i].Debug(i, (i < 7)?true:false);
            break;

        case 3:
            if (i < 7)
                m_segment[i].Debug(i, true);
            else
                m_segment[i].Debug(i, (i < 10)?true:false);
            break;
        }
*/    
    ToggleSegments();
}


void GeneLimb::Mutate(int chance)
{
    for (int i = 0; i < MAX_SEGMENTS; i++)
        m_segment[i].Mutate(chance, i);

    ToggleSegments();
}


void GeneLimb::Crossover(GeneLimb&  gLine)
{
  for (int i = 0; i < MAX_SEGMENTS; i++)
    if (Bool())
      m_segment[i] = gLine.m_segment[i];

    ToggleSegments();
}

void GeneLimb::SerializeJson(rapidjson::Document &d, rapidjson::Value &v)
{
    Document::AllocatorType& allocator = d.GetAllocator();

    Value segsJson(kArrayType);
    for (int i = 0; i < MAX_SEGMENTS; i++)
    {
        Value segJson(kObjectType);
        m_segment[i].SerializeJson(d, segJson);
        segsJson.PushBack(segJson, allocator);
    }
    v.AddMember("m_segment", segsJson, allocator);
}

void GeneLimb::SerializeJsonLoad(const rapidjson::Value& v)
{
    if(!v.IsObject())
        throw std::runtime_error("eror parsing json");

    const Value &seg = v["m_segment"];
    for (int i = 0; i < seg.Size() and i<MAX_SEGMENTS; i++)
        m_segment[i].SerializeJsonLoad(seg[i]);

    ToggleSegments();
}

int GeneLimb::GetSegmentsVisible()
{
    int j = 0;
    for (int i = 0; i < MAX_SEGMENTS; i++)
        if (m_segment[i].IsVisible())
            j++;

    return j;
}

// This method is saving 0 for non-visible segments
// and toggling between -1 and 1 for visible segments
void GeneLimb::ToggleSegments()
{
    bool bToggle = false;
    for (int i = 0; i < MAX_SEGMENTS; i++)
    {
        if (m_segment[i].IsVisible())
        {
            m_toggleVisibleSegments[i] = bToggle;

            bToggle = !bToggle;
        }
        else
        {
            m_toggleVisibleSegments[i] = false;
        }
    }
}

//////////////////////////////////////////////////////////////////////
// GeneTrait
//
// Contains GeneLines plus overall biot genes
//
const int GeneTrait::mirrorAngle[MAX_LIMBS] = { 0, 180,  0,  180,  90, 90, 270, 270 };
const int GeneTrait::mirrorCoef[MAX_LIMBS]  = { 1,  -1, -1,    1,  -1,  1,  -1,   1 };
const int GeneTrait::mirrorSix[MAX_LIMBS]   = { 0, 120,  0,  120, 240, 240,  0,   0 };

GeneTrait::GeneTrait()
{


}

GeneTrait::~GeneTrait()
{


}

void GeneTrait::SerializeJson(rapidjson::Document &d, rapidjson::Value &v)
{
    Document::AllocatorType& allocator = d.GetAllocator();

    Value lineJson1(kArrayType);
    for (int i = 0; i < MAX_LIMB_TYPES; i++)
    {
        Value lineJson2(kObjectType);
        m_geneLine[i].SerializeJson(d, lineJson2);
        lineJson1.PushBack(lineJson2, allocator);
    }
    v.AddMember("m_geneLine", lineJson1, allocator);

    v.AddMember("m_disperse", m_disperse, allocator);
    v.AddMember("m_children", m_children, allocator);
    v.AddMember("m_attackChildren", m_attackChildren, allocator);
    v.AddMember("m_attackSiblings", m_attackSiblings, allocator);
    v.AddMember("m_species", m_species, allocator);
    v.AddMember("m_adultRatio0", m_adultRatio[0], allocator);
    v.AddMember("m_adultRatio1", m_adultRatio[1], allocator);
    v.AddMember("m_lineCount", m_lineCount, allocator);
    v.AddMember("m_offset", m_offset, allocator);

    Value linesJson(kArrayType);
    for(int i=0; i<MAX_LIMBS; i++)
        linesJson.PushBack(m_lineRef[i], allocator);
    v.AddMember("m_lineRef", linesJson, allocator);

    v.AddMember("m_mirrored", m_mirrored, allocator);
    v.AddMember("m_sex", m_sex, allocator);
    v.AddMember("m_asexual", m_asexual, allocator);
    v.AddMember("m_chanceMale", m_chanceMale, allocator);
    v.AddMember("m_maxAge", m_maxAge, allocator);
}

void GeneTrait::SerializeJsonLoad(const rapidjson::Value& v)
{
    if(!v.IsObject())
        throw std::runtime_error("eror parsing json");

    const Value &gl = v["m_geneLine"];
    for (int i = 0; i < gl.Size() and i<MAX_LIMB_TYPES; i++)
            m_geneLine[i].SerializeJsonLoad(gl[i]);

    m_disperse = v["m_disperse"].GetUint();
    m_children = v["m_children"].GetUint();
    m_attackChildren = v["m_attackChildren"].GetUint();
    m_attackSiblings = v["m_attackSiblings"].GetUint();
    m_species = v["m_species"].GetUint();
    m_adultRatio[0] = v["m_adultRatio0"].GetUint();
    m_adultRatio[1] = v["m_adultRatio1"].GetUint();
    m_lineCount = v["m_lineCount"].GetUint();
    m_offset = v["m_offset"].GetInt();

    for (int i = 0; i < MAX_LIMBS; i++)
        m_lineRef[i] = 0;
    const Value &lr = v["m_lineRef"];
    for(int i=0; i<lr.Size() and i<MAX_LIMBS; i++)
    {
        uint8_t val = lr[i].GetUint(); //Can't be negative due to unsigned type
        if(val >= MAX_LIMB_TYPES) throw std::range_error("Invalid limb type in m_lineRef");
        m_lineRef[i] = val;
    }

    m_mirrored = v["m_mirrored"].GetUint();
    m_sex = v["m_sex"].GetUint();
    m_asexual = v["m_asexual"].GetUint();
    m_chanceMale = v["m_chanceMale"].GetUint();
    m_maxAge = v["m_maxAge"].GetInt();

    if (m_children > 8 || m_children == 0)
        m_children = 8;

    if (m_species >= 16)
        m_species = 16;

    if (m_adultRatio[0] > 6 || m_adultRatio[0] == 0)
        m_adultRatio[0] = 6;

    if (m_adultRatio[1] > 6 || m_adultRatio[1] == 0)
        m_adultRatio[1] = 6;

    if (m_lineCount > 8 || m_lineCount == 0)
        m_lineCount = 8;

    if (m_sex > 1)
        m_sex = 1;

    if (m_asexual > 1)
        m_asexual = 1;

    CalculateAngles();

}

int GeneTrait::GetCompressedToggle(int nAngle, int nLine, int nSegment)
{
    assert(nLine < MAX_LIMBS);
    assert(nSegment < MAX_SEGMENTS);

    if (IsMirrored())
    {
        if (mirrorCoef[nLine] == -1)
            if (m_geneLine[m_lineRef[nLine]].m_toggleVisibleSegments[nSegment])
                return -nAngle;
            else
                return nAngle;
    }

    if (m_geneLine[m_lineRef[nLine]].m_toggleVisibleSegments[nSegment])
        return nAngle;
    else
        return -nAngle;
}


void GeneTrait::CalculateAngles()
{
    const int* pAngle;
    if (GetLines() == 6)
        pAngle = &mirrorSix[0];
    else
        pAngle = &mirrorAngle[0];

    for (int line = 0; line < GetLines(); line++)
    {
        GeneLimb& gLine = m_geneLine[m_lineRef[line]];
        for (int segment = 0; segment < MAX_SEGMENTS; segment++)
        {
            GeneSegment& gSegment = gLine.GetSegment(segment);
            if (gSegment.IsVisible())
            {
                if (IsMirrored())
                    m_angle[line][segment] = (short) (GetOffset() + (gSegment.GetAngle() * mirrorCoef[line]) + pAngle[line]);
                else
                    m_angle[line][segment] = (short) (GetOffset() + gSegment.GetAngle() + (line * 360) / GetLines());
            }
        }    
    }
}


void GeneTrait::Randomize(int nArmsPerBiot, int nTypesPerBiot, int nSegmentsPerArm)
{
    int i;

    m_disperse       = (uint8_t) Bool();
    m_children       = (uint8_t) (Integer(8) + 1);
    m_attackChildren = (uint8_t) Bool();
    m_attackSiblings = (uint8_t) Bool();
    m_species        = (uint8_t) Integer(2);  // Make fewer species to begin with
    m_adultRatio[0]  = (uint8_t) (Integer(6) + 1);
    m_adultRatio[1]  = (uint8_t) (Integer(6) + 1);
    m_mirrored       = (uint8_t) Bool();

    switch(nArmsPerBiot)
    {
    default:
    case 0:
        m_lineCount  = (uint8_t) (Integer(MAX_LIMBS) + 1);
        break;

    case 1:
        m_lineCount  = (uint8_t) (Integer(2) + 1);
        break;

    case 2:
        m_lineCount  = (uint8_t) (Integer(2) + 3);
        break;

    case 3:
        m_lineCount  = (uint8_t) (Integer(2) + 5);
        break;

    case 4:
        m_lineCount  = (uint8_t) (Integer(2) + 7);
        break;
    }

    // Initially,we don't want odd mirrored creatures
    if ((m_lineCount  & 0x01) == 0x01)
        m_mirrored = false;

    m_offset         = (short) Integer(360);

    for (i = 0; i < MAX_LIMB_TYPES; i++)
        m_geneLine[i].Randomize(nSegmentsPerArm);

    m_sex        = (uint8_t) Bool();
    m_asexual    = (uint8_t) Bool();
    m_chanceMale = (uint8_t) (Int256() / 2 + 64);
    m_maxAge     = (short) Int256();

    // We start out with uniform appearance
    for (i = 0; i < MAX_LIMBS; i++)
    {
        m_lineRef[i] = Byte(nTypesPerBiot + 1);
        assert(m_lineRef[i] < MAX_LIMB_TYPES && m_lineRef[i] >= 0);
    }

    CalculateAngles();
}


void GeneTrait::Debug(int nArmsPerBiot, int nTypesPerBiot, int nSegmentsPerArm)
{
    int i;

    m_disperse       = (uint8_t) true;
    m_children       = (uint8_t) 1;
    m_attackChildren = (uint8_t) false;
    m_attackSiblings = (uint8_t) false;
    m_species        = (uint8_t) 0;  // Make fewer species to begin with
    m_adultRatio[0]  = (uint8_t) 1;
    m_adultRatio[1]  = (uint8_t) 1;
    m_mirrored       = (uint8_t) true;

    m_lineCount  = 1;//(uint8_t) MAX_LIMBS;
/*    switch(nArmsPerBiot)
    {
    default:
    case 0:
        m_lineCount  = (uint8_t) MAX_LIMBS;
        break;

    case 1:
        m_lineCount  = (uint8_t) 2;
        break;

    case 2:
        m_lineCount  = (uint8_t) 4;
        break;

    case 3:
        m_lineCount  = (uint8_t) 6;
        break;

    case 4:
        m_lineCount  = (uint8_t) 8;
        break;
    }
*/
    // Initially,we don't want odd mirrored creatures
    if ((m_lineCount  & 0x01) == 0x01)
        m_mirrored = false;

    m_offset         = (short) 0; 

    for (i = 0; i < MAX_LIMB_TYPES; i++)
        m_geneLine[i].Debug(nSegmentsPerArm);

    m_sex        = (uint8_t) false;
    m_asexual    = (uint8_t) true;
    m_chanceMale = (uint8_t) 0;
    m_maxAge     = (short) 255;

    // We start out with uniform appearance
    for (i = 0; i < MAX_LIMBS; i++)
    {
        m_lineRef[i] = 0;
    }

    CalculateAngles();
}


void GeneTrait::Mutate(int chance)
{
    int i;

    if (Int1024() < chance)
        m_disperse       = (uint8_t) Bool();

    if (Int1024() < chance)
        m_children       = (uint8_t) (Integer(8) + 1);

    if (Int1024() < chance)
        m_attackChildren = (uint8_t) Bool();

    if (Int1024() < chance)
        m_attackSiblings = (uint8_t) Bool();

    if (Int1024() < chance)
    {
        if (Sign() > 0)
            m_species++;
        else
            m_species--;
    }

    if (Int1024() < chance)
        m_adultRatio[0]     = (uint8_t) (Integer(6) + 1);

    if (Int1024() < chance)
        m_adultRatio[1]     = (uint8_t) (Integer(6) + 1);

    if (Int1024() < chance)
        m_lineCount         = (uint8_t) (Integer(8) + 1);

    if (Int1024() < chance)
        m_mirrored       = (uint8_t) Bool();

    if (Int1024() < chance)
        m_offset         = (short) Integer(360);

    for (i = 0; i < MAX_LIMB_TYPES; i++)
        m_geneLine[i].Mutate(chance);

    if (Int1024() < chance)
        m_sex = (uint8_t) Bool();

    if (Int1024() < chance)
        m_asexual = (uint8_t) Bool();

    if (Int1024() < chance)
        m_chanceMale = Byte();

    if (Int1024() < chance)
        m_maxAge = (short) Int256();

    for (i = 0; i < MAX_LIMBS; i++)
        if (Int1024() < chance)
            m_lineRef[i] = Byte(MAX_LIMB_TYPES);

    CalculateAngles();
}


void GeneTrait::Crossover(GeneTrait&  gTrait)
{
    int i;

    for (i = 0; i < MAX_LIMB_TYPES; i++)
        m_geneLine[i].Crossover(gTrait.m_geneLine[i]);

    for (i = 0; i < MAX_LIMBS; i++)
        if (Bool())
        {
            m_lineRef[i] = gTrait.m_lineRef[i];
            assert(m_lineRef[i] < MAX_LIMB_TYPES && m_lineRef[i] >= 0);
        }

    if (Bool())
        m_offset = gTrait.m_offset;
    
    if (Bool())
        m_disperse       = gTrait.m_disperse;

    if (Bool())
        m_children       = gTrait.m_children;

    if (Bool())
        m_attackChildren = gTrait.m_attackChildren;

    if (Bool())
        m_attackSiblings = gTrait.m_attackSiblings;

    if (Bool())
        m_species        = gTrait.m_species;

    if (Bool())
        m_adultRatio[0]  = gTrait.m_adultRatio[0];

    if (Bool())
        m_adultRatio[1]  = gTrait.m_adultRatio[1];

    if (Bool())
        m_lineCount      = gTrait.m_lineCount;

    if (Bool())
        m_mirrored       = gTrait.m_mirrored;

    if (Bool())
        m_asexual        = gTrait.m_asexual;

    if (Bool())
        m_chanceMale     = gTrait.m_chanceMale;

    CalculateAngles();
}


bool GeneTrait::IsLineTypeVisible(int lineType)
{
    for (int i = 0; i < m_lineCount; i++)
        if (lineType == m_lineRef[i])
            return true;

    return false;
}


