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
#include <grape/settings/Setting.h>
#include <vector>

//==============================================================================

namespace grape {
namespace settings {

//==============================================================================

class SettingManager : private juce::ValueTree::Listener
{
public:
    class Listener
    {
    public:
        virtual ~Listener() {}

    public:
        virtual void settingChanged (const juce::String&, const juce::var&) = 0;
    };

public:
    SettingManager (const std::vector<Setting>& settingsInfo,
                    juce::UndoManager* undoManager,
                    const juce::String& identifier = "settings");
    ~SettingManager();

public:
    juce::var getSetting (const juce::String&);
    void setSetting (const juce::String&, const juce::var&);

    juce::XmlElement* toXml();
    void fromXml (const juce::XmlElement&);

    void addListener (Listener*);
    void removeListener (Listener*);

private:
    void notifySettingChanged (const juce::String&, const juce::var&);

private: // juce::ValueTree::Listener
    void valueTreePropertyChanged (juce::ValueTree&, const juce::Identifier&) override;
    void valueTreeChildAdded (juce::ValueTree&, juce::ValueTree&) override;
    void valueTreeChildRemoved (juce::ValueTree&, juce::ValueTree&, int) override;
    void valueTreeChildOrderChanged (juce::ValueTree&, int, int) override;
    void valueTreeParentChanged (juce::ValueTree&) override;
    void valueTreeRedirected (juce::ValueTree&) override;

private:
    const std::vector<Setting>      mSettingsInfo;
    juce::UndoManager*              mUndoManager;
    juce::ValueTree                 mSettings;
    juce::ListenerList<Listener>    mListeners;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingManager)
};

//==============================================================================

} // namespace settings
} // namespace grape

