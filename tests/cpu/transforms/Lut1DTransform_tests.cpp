// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the OpenColorIO Project.


#include "transforms/Lut1DTransform.cpp"

#include "testutils/UnitTest.h"

namespace OCIO = OCIO_NAMESPACE;


OCIO_ADD_TEST(Lut1DTransform, basic)
{
    const OCIO::Lut1DTransformRcPtr lut = OCIO::Lut1DTransform::Create();

    OCIO_CHECK_EQUAL(lut->getLength(), 2);
    OCIO_CHECK_EQUAL(lut->getDirection(), OCIO::TRANSFORM_DIR_FORWARD);
    OCIO_CHECK_EQUAL(lut->getHueAdjust(), OCIO::HUE_NONE);
    OCIO_CHECK_EQUAL(lut->getInputHalfDomain(), false);
    OCIO_CHECK_EQUAL(lut->getOutputRawHalfs(), false);
    float r = 0.f;
    float g = 0.f;
    float b = 0.f;
    lut->getValue(0, r, g, b);
    OCIO_CHECK_EQUAL(r, 0.f);
    OCIO_CHECK_EQUAL(g, 0.f);
    OCIO_CHECK_EQUAL(b, 0.f);
    lut->getValue(1, r, g, b);
    OCIO_CHECK_EQUAL(r, 1.f);
    OCIO_CHECK_EQUAL(g, 1.f);
    OCIO_CHECK_EQUAL(b, 1.f);

    lut->setDirection(OCIO::TRANSFORM_DIR_INVERSE);
    OCIO_CHECK_EQUAL(lut->getDirection(), OCIO::TRANSFORM_DIR_INVERSE);

    lut->setLength(3);
    OCIO_CHECK_EQUAL(lut->getLength(), 3);
    lut->getValue(0, r, g, b);
    OCIO_CHECK_EQUAL(r, 0.f);
    OCIO_CHECK_EQUAL(g, 0.f);
    OCIO_CHECK_EQUAL(b, 0.f);
    lut->getValue(1, r, g, b);
    OCIO_CHECK_EQUAL(r, 0.5f);
    OCIO_CHECK_EQUAL(g, 0.5f);
    OCIO_CHECK_EQUAL(b, 0.5f);
    lut->getValue(2, r, g, b);
    OCIO_CHECK_EQUAL(r, 1.f);
    OCIO_CHECK_EQUAL(g, 1.f);
    OCIO_CHECK_EQUAL(b, 1.f);

    r = 0.51f;
    g = 0.52f;
    b = 0.53f;
    lut->setValue(1, r, g, b);

    r = 0.f;
    g = 0.f;
    b = 0.f;
    lut->getValue(1, r, g, b);

    OCIO_CHECK_EQUAL(r, 0.51f);
    OCIO_CHECK_EQUAL(g, 0.52f);
    OCIO_CHECK_EQUAL(b, 0.53f);

    OCIO_CHECK_EQUAL(lut->getFileOutputBitDepth(), OCIO::BIT_DEPTH_UNKNOWN);

    lut->setFileOutputBitDepth(OCIO::BIT_DEPTH_UINT8);
    OCIO_CHECK_EQUAL(lut->getFileOutputBitDepth(), OCIO::BIT_DEPTH_UINT8);

    // File out bit-depth does not affect values.
    r = 0.f;
    g = 0.f;
    b = 0.f;
    lut->getValue(1, r, g, b);

    OCIO_CHECK_EQUAL(r, 0.51f);
    OCIO_CHECK_EQUAL(g, 0.52f);
    OCIO_CHECK_EQUAL(b, 0.53f);

    OCIO_CHECK_NO_THROW(lut->validate());

    OCIO_CHECK_THROW_WHAT(lut->setValue(3, 0.f, 0.f, 0.f), OCIO::Exception,
                          "should be less than the length");
    OCIO_CHECK_THROW_WHAT(lut->getValue(3, r, g, b), OCIO::Exception,
                          "should be less than the length");

    lut->setInputHalfDomain(true);
    OCIO_CHECK_THROW_WHAT(lut->validate(), OCIO::Exception,
                          "65536 required for halfDomain 1D LUT");

    OCIO_CHECK_THROW_WHAT(lut->setLength(1024*1024+1), OCIO::Exception,
                          "must not be greater than");

    lut->setInputHalfDomain(false);
    lut->setValue(0, -0.2f, 0.1f, -0.3f);
    lut->setValue(2, 1.2f, 1.3f, 0.8f);

    std::ostringstream oss;
    oss << *lut;
    OCIO_CHECK_EQUAL(oss.str(), "<Lut1DTransform direction=inverse, fileoutdepth=8ui,"
        " interpolation=default, inputhalf=0, outputrawhalf=0, hueadjust=0,"
        " length=3, minrgb=[-0.2 0.1 -0.3], maxrgb=[1.2 1.3 0.8]>");
}

OCIO_ADD_TEST(Lut1DTransform, create_with_parameters)
{
    const auto lut0 = OCIO::Lut1DTransform::Create(65536, true);

    OCIO_CHECK_EQUAL(lut0->getLength(), 65536);
    OCIO_CHECK_EQUAL(lut0->getDirection(), OCIO::TRANSFORM_DIR_FORWARD);
    OCIO_CHECK_EQUAL(lut0->getHueAdjust(), OCIO::HUE_NONE);
    OCIO_CHECK_EQUAL(lut0->getInputHalfDomain(), true);
    OCIO_CHECK_NO_THROW(lut0->validate());

    const auto lut1 = OCIO::Lut1DTransform::Create(10, true);

    OCIO_CHECK_EQUAL(lut1->getLength(), 10);
    OCIO_CHECK_EQUAL(lut1->getInputHalfDomain(), true);
    OCIO_CHECK_THROW_WHAT(lut1->validate(), OCIO::Exception,
                          "65536 required for halfDomain 1D LUT");

    const auto lut2 = OCIO::Lut1DTransform::Create(8, false);

    OCIO_CHECK_EQUAL(lut2->getLength(), 8);
    OCIO_CHECK_EQUAL(lut2->getInputHalfDomain(), false);
    OCIO_CHECK_NO_THROW(lut2->validate());
}

