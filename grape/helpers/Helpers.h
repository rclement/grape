/*
    GRAPE is Romain's Audio Plug-in Extension classes for the JUCE framework

    Copyright (c) 2018 Romain Clement

    MIT License    
    
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
 */

//==============================================================================

#pragma once

#include <JuceHeader.h>
#include <array>

//==============================================================================

namespace grape {
namespace helpers {

//==============================================================================

template <int decimalPlaces>
inline juce::String floatValueToText (float value)
{
    return juce::String (value, decimalPlaces);
}

inline float floatTextToValue (const juce::String& text)
{
    return text.getFloatValue();
}

//==============================================================================

inline juce::String varValueToText (juce::var value)
{
    return value.toString();
}

inline juce::var varTextToValue (const juce::String& text)
{
    return juce::var (text);
}

//==============================================================================

inline float skewFactor (float start, float end, float mid)
{
    jassert (mid >= start && mid <= end);
    return std::log (0.5f) / std::log ((mid - start) / (end - start));
}

//==============================================================================

template<int numSteps, const std::array<juce::String, numSteps>& choices>
inline juce::String choiceIndexToLabel (float value)
{
    const int index = static_cast<int> (std::floor (value));
    if (index >= 0 && index < numSteps)
    {
        return choices[index];
    }

    jassert ("Invalid parameter choice index");
    return juce::String();
}

template< int numSteps, const std::array<juce::String, numSteps>& choices >
inline float choiceLabelToIndex (juce::String text)
{
    for (int i = 0; i < numSteps; ++i) {
        const juce::String step = choices[i];
        if (step.equalsIgnoreCase (text))
        {
            return static_cast<float> (i);
        }
    }

    jassert ("Invalid parameter choice label");
    return 0.0f;
}

//==============================================================================

} // namespace helpers
} // namespace grape

