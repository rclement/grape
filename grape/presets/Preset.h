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

//==============================================================================

namespace grape {
namespace presets {

//==============================================================================

class Preset
{
public:
    Preset (const juce::File& presetFile = juce::File(),
            const juce::String& presetBank = juce::String(),
            const juce::String& presetAuthor = juce::String(),
            const juce::String& presetComments = juce::String(),
            int presetVersion = 1);
    ~Preset ();

public:
    inline juce::String getName() const { return mName; }
    inline void setName (const juce::String& name) { mName = name; }

    inline juce::String getBank() const { return mBank; }
    inline void setBank (const juce::String& bank) { mBank = bank; }

    inline juce::String getAuthor() const { return mAuthor; }
    inline void setAuthor (const juce::String& author) { mAuthor = author; }

    inline juce::String getComments() const { return mComments; }
    inline void setComments (const juce::String& comments) { mComments = comments; }

    inline bool isModified() const { return mModified; }
    inline void setModified (bool modified) { mModified = modified; }

    bool operator== (const Preset& other) const;
    bool operator!= (const Preset& other) const;

    bool loadFromFile();
    bool saveToFile();

    juce::XmlElement* toXml() const;
    bool fromXml(const juce::XmlElement& xml);

    juce::ValueTree copyState();
    void replaceState (const juce::ValueTree&);
    bool checkState (const juce::ValueTree&);

private:
    juce::File      mFile;
    juce::String    mName;
    juce::String    mBank;
    juce::String    mAuthor;
    juce::String    mComments;
    int         	mVersion;
    bool        	mModified;
    juce::ValueTree mState;
};

//==============================================================================

} // namespace presets
} // namespace grape

