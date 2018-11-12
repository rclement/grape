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
#include <grape/presets/Preset.h>
#include <grape/presets/PresetChecker.h>
#include <grape/parameters/ParameterManager.h>

//==============================================================================

namespace grape {
namespace presets {

//==============================================================================

class PresetManager
{
public:
    class Listener
    {
    public:
        virtual ~Listener() {}

    public:
        virtual void presetChanged (const Preset&) = 0;
    };

public:
    PresetManager (parameters::ParameterManager&);
    ~PresetManager();

public:
    juce::File getFactoryPresetsLocation() const;
    juce::File getUserPresetsLocation() const;

    juce::Array<Preset> getFactoryPresets() const;
    juce::Array<Preset> getUserPresets() const;
    juce::Array<Preset> getAllPresets() const;

    Preset getFactoryPreset (const juce::String& presetName,
                             const juce::String& presetBank) const;
    Preset getUserPreset (const juce::String& presetName,
                          const juce::String& presetBank) const;
    Preset getCurrentPreset() const;

    void loadDefaultPreset();
    void loadPreset (const Preset&);
    void loadPreviousPreset();
    void loadNextPreset();
    bool canLoadPreviousPreset();
    bool canLoadNextPreset();
    bool saveCurrentPreset (const juce::String& presetName,
                            const juce::String& presetBank,
                            const juce::String& presetAuthor,
                            const juce::String& presetComments);

    juce::XmlElement* toXml();
    void fromXml (const juce::XmlElement&);

    void checkPresetChanged();

    void addListener (Listener*);
    void removeListener (Listener*);

private:
    juce::Array<Preset> findPresets (const juce::File&) const;
    juce::String findPresetBank (const juce::File&, const juce::File&) const;
    void notifyPresetChanged();
    void findCurrentPresetIndex();
    void loadPresetAtIndex (int presetIndex);

private:
    parameters::ParameterManager&   mParameters;
    Preset                          mCurrentPreset;
    int                             mCurrentPresetIndex;
    PresetChecker                   mPresetChecker;
    juce::ListenerList<Listener>    mListeners;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetManager)
};

//==============================================================================

} // namespace presets
} // namespace grape

